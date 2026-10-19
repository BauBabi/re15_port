/* test_r39_elzazombies.c — RIEGEL Runde 39 Spur Q "elzazombies": Elza, Zombie-Durchbrueche in der Panikphase (Montage-
 * Form wie Leon nach Irons' Tod). Dossier analysis/befunde_runde39/Q_elzazombies.md, Mechanik include/re15_elzazombies_r39.h
 * und engine/src/elzazombies_r39.c.
 *
 * NUTZER (AUFTRAG.md Nachtrag 10, woertlich): "Und auch hier sollen jetzt Zombies durchbrechen - einmal in den Flur hinein
 * von 10C1 aus - so viele wie im medical room - also 10E1 am Leben gelassen wurden., die sollen dann, wenn man in ROOM
 * 1061 die Etage von unten ueber die Treppe erreicht aber moeglichst weit weg an der Seite stehen. Dann das gleiche
 * nochmal wie bei Leon in den ROOM 1071, falls die Zombies nicht besiegt wurden und im ROOM 1031 ueber die
 * Absperrung/Das Rollo, so wie bei der cutscene."
 * NACHTRAG 11 (woertlich): "Ich korrigiere, die Zombies sollen nicht in ROM 1071 noch durchbrechen, sondern dann noch wie
 * bei leon im ROOM 1041." (Orchestrator: wie Leons 1040-Rolltor; "falls die Zombies nicht besiegt wurden" = die aus
 * 10C1/1061; spielbar, z.B. beim naechsten Betreten von 1041.)
 *
 * Verfahren wie test_r38_montage.c (ausgelieferte RDT, Raumaufbau ueber scd_room_reenter = derselbe Einhaengepunkt wie im
 * Spiel, Bilder in der Reihenfolge von platform/pc/main.c, RE2-Gegnerbaenke wie die exe), Elza = g_gameflow.character 4.
 *
 * Teile (argv[1]):
 *   zensus    Quell-Records (ROOM10E1 @0x00D56.. Tot-Bits 0xe0..0xe3, ROOM1071 @0x015CA.. 0xc6..0xca) = Konstanten; die 16
 *             Kopie-Bits (Zone 7) kommen in KEINEM Sce_em_set von STAGE1-3 vor, die 11 Elza-Bits nicht in Leons Kopien, die 5
 *             1031-Cut-7-Bits = Leons Vorlage 6 (Nachtrag 12); Raum-Paare 1060/1061, 10E0/10E1, 10C0/10C1, 1070/1071 byte-gleich;
 *             Programm L (Leon 1060) + Weiche; die Zeilen des 1031-Programms = RDT-Bytes ROOM1031 sub08
 *             (@0x02858/@0x0286E/@0x0287A/@0x02882..), sub06/07/09 = ROOM1030; ROOM1041 = ROOM1040 (Rolltor-Szene sub08,
 *             Rolltor-Objekt, Schalter-Rechteck, Limit); Programm G traegt die 1041-sub08-Zeilen woertlich; Kamera-Saetze =
 *             RID-Bytes; Tuer-Slot 20 / Ereignis 30 / Bank-5-Bits 28/29/30 frei.
 *   kette_<a> echte Kette im Prozess: ROOM1041 (Elza), Flanke (9,97) -> Raum fuer Raum bis zur Rueckkehr; <a> = lebende
 *             10E1-/1071-Zombies (4/5, 0/5, 2/3, 00 = 0/0). Prueft Raumfolge, Schnitte, Zahl der Kopien (= so viele wie leben),
 *             jede Kopie kommt ihrer Kamera naeher (>= 400 = Koerperradius), Spieler-HP unveraendert, Rueckkehr an die gemerkte
 *             Stelle mit Steuerung, (9,95)=1, 10E1-Quellen gesetzt; NACHTRAG 12: mit 1071-Ueberlebenden zuerst 1031 Cut 7 (Leons
 *             Vorlage 6 + Original-Se_on @0x02862), Tuerknall davor, Kopie-Bits = Leons 47,48,49,75,76. Gegenprobe (Modul aus).
 *   leon_<a>  NACHTRAG 12, Leon: Kette ab ROOM1150 (Szene) bis 1030 mit <a> lebenden 10E0-Zombies: 4 (1140 voll, Gegenprobe +
 *             Vergleich des 1040-Schritts), 2 (1140 leer: Szene -> 1060), 0 (Kette wie bisher). 1060 Cut 2 = Elzas Form
 *             (K3-Folge, Kopien naeher an der Kamera, HP 100, Schnitt 176 Bilder).
 *   leon_nach1060  Leon betritt 1060 nach der Szene: Kopien an der Westseite, gehalten bis Etage 4, ohne (9,73) nichts;
 *             NACHBESSERUNG 3: ohne (9,96) (Umzug nie gelaufen) nichts.
 *   altstand  NACHBESSERUNG 3, Mangel 1 der Abnahme 2: alter Leon-Spielstand nach Irons' Tod (v0.8.22..v0.8.26: (9,73)=1, Kopie-
 *             Bits 124..127 nie gesetzt, (9,96)=0) -> ROOM1060 ohne Gegner, egal ob die 10E0-Zombies getoetet sind (Karte der
 *             Abnahme M10) oder leben (dann stehen sie weiter in 10E0, nicht doppelt); GEGENPROBE derselbe Stand mit (9,96)=1 ->
 *             vier Kopien (der Riegel sieht den Unterschied); re15_r39q_leon_umzug setzt (9,96) und die Kopie-Bits.
 *   oben      NACHBESSERUNG 3, Mangel 2 + ORCHESTRATOR-ENTSCHEID E1: Ankunft im Treppenhaus von OBEN (Tuer Etage 8, Elza 1061 und
 *             Leon 1060): Kopien an der Ostseite (Lagen 3 = weiteste Packung vom Ende der Treppe A), im Spawn-Bild OHNE Haltebit
 *             (INIT laeuft), danach gehalten und initialisiert (state != 0, HP > 0, Weg <= 50 in 200 Bildern auf Etage 8, kein
 *             Griff); frei bei der Ankunft am Ende der Treppe A (20364/20432,25368) mit kleinstem Abstand >= 4591 (= gemessen
 *             von unten), danach kommen sie naeher, Spieler-HP 100. Ankunft durch die 10C1-Tuer (Etage 4): Westseite, sofort
 *             frei, Abstand >= 4591.
 *   nach1061  Nachspawn: die 10E1-Kopien stehen an der Westseite (moeglichst weit weg vom gemessenen Ankunftspunkt der
 *             Treppe), gehalten solange die Spielerin unten ist (200 Bilder Etage 0, Weg 0), frei auf Etage 4 (Abstand
 *             >= 4000 beim Freigeben), danach kommen sie naeher; tote Kopien erscheinen nicht.
 *   nach1031  Nachspawn ROOM1031 nach der Montage: 3 Kriecher + die lebenden 1071-Kopien an Leons Nachspawn-Lagen (Nachtrag 12),
 *             keine 1061-/1041-Kopie; ohne (9,95) nichts.
 *   durch1041 Nachtrag 11 (+ H1-Entscheid: nur bei (9,97)=1, sonst nichts):
 *             Betreten von 1041 durch die 1061-Tuer mit lebenden 1061-Kopien -> Rolltor-Durchbruch (erstes Bild
 *             Cut 1, Elza auf Leons Parkplatz, Rolltor faehrt hoch, die umgezogenen Kopien kommen durch und naeher an die
 *             Kamera, HP 100, danach Elza an ihrem Eintrittspunkt mit Steuerung, Haltebits los); naechstes Betreten:
 *             kein zweiter Durchbruch, Nachspawn der Durchgebrochenen; Eintritt im Nordteil / alle 1061-Kopien tot: nichts.
 *   ohne      ohne Flanke keine Kette (Bit 97 beim Laden gesetzt = keine Flanke); Leon (Figur 0) mit Flanke und (9,95):
 *             nichts (Leons Treppenhaus haengt an Leons Kette/(9,73), nicht an Elzas Bits); Modul aus: nichts.
 */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#  define _DEFAULT_SOURCE 1   /* unsetenv (POSIX) unter -std=c11 ohne GNU-Erweiterungen (CMAKE_C_EXTENSIONS OFF); Muster
                               * test_r36_zombieki_affen_block.c */
#endif
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
static int s_mit_irons = 0;                                 /* Leon-Teile (Nachtrag 12): Irons' RE1.5-Bank laden */
static void bank_irons(void);

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
    if (s_mit_irons) bank_irons();                         /* Leon-Teile: Irons (Typ 0x45) fuer die Szene in 1150 */
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
    /* ROOM1041 = ROOM1040 dort, wo Programm G Leons 1040-Schritt uebernimmt (Nachtrag 11) */
    {
        size_t n40 = 0, n41 = 0;
        uint8_t *d40 = raum_bytes(0x1040, &n40), *d41 = raum_bytes(0x1041, &n41);
        int ok = d40 && d41;
        if (ok) {
            ok &= bytes_gleich(d41 + 0x019C6, d40 + 0x0199C, 180);                          /* sub08 = Rolltor-Szene */
            ok &= bytes_gleich(d41 + 0x01118, d40 + 0x01118, 20) && les16(d41 + 0x01118 + 6) == -26000 &&
                  les16(d41 + 0x01118 + 8) == -16000;                                      /* Schalter-Rechteck Slot 4 */
            ok &= d41[0x01190] == 0x2d && les16(d41 + 0x01190 + 10) == -25200 && les16(d41 + 0x01190 + 14) == RE15_IT_TOR_Z_1040;
            ok &= d41[0x01218] == 0x24 && d41[0x01219] == 0x12 && d41[0x0121A] == 0x05;    /* Limit 5 */
            const uint8_t *r40 = d40 + le32(d40 + 0x24) + 32, *r41 = d41 + le32(d41 + 0x24) + 32;
            ok &= bytes_gleich(r40, r41, 32) && (int32_t)le32(r41 + 4) == -26262 && (int32_t)le32(r41 + 12) == -10692;
        }
        PRUEF(ok, "ROOM1041 = ROOM1040: sub08 @0x019C6 (180 B, Rolltor), Schalter-Rechteck @0x01118, Rolltor @0x01190 z %d, "
              "Limit @0x01218 `24 12 05`, RID Cut 1 @0x00080", RE15_IT_TOR_Z_1040);
        /* Programm G im Bau: die 1041-sub08-Zeilen woertlich */
        elza_flags();
        re15_elzazombies_r39_reset();
        re15_elzazombies_r39_zustand_setzen(RE15_R39Q_D1041);
        g_current_room_id = 0x1041;
        for (int s = 0; s < SCD_THREAD_COUNT; s++) g_scd.threads[s].active = 0;
        const uint8_t *pg = re15_elzazombies_r39_ereignis(0x1041, RE15_R39Q_EREIGNIS);
        int plen = 0; re15_elzazombies_r39_laufprogramm(&plen);
        PRUEF(pg && d41 && im_programm(pg, plen, d41, 0x019DC, 12) && im_programm(pg, plen, d41, 0x019EC, 12) &&
              im_programm(pg, plen, d41, 0x01A08, 0x01A54 - 0x01A08) && im_programm(pg, plen, d41, 0x02234, 2),
              "Programm G (%d B): Se_on @0x019DC/@0x019EC, Rolltor-Fahrt @0x01A08..@0x01A53 (Speed/For/Cut_chg 1/Sca/Se_on/Sleep), "
              "Cut_auto @0x02234 woertlich", plen);
        re15_elzazombies_r39_zustand_setzen(RE15_R39Q_AUS);
        free(d40); free(d41);
    }
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
        if (i < 11) for (unsigned j = 0; j < sizeof leon / sizeof leon[0]; j++) if (kb[i] == leon[j]) fremd = 0;
        for (int j = i + 1; j < nkb; j++) if (kb[i] == kb[j]) doppelt++;
    }
    PRUEF(raeume >= 130 && s_desync == 0 && nkb == 16, "Zensus vollstaendig (%d RDTs, 0 Desync), 16 Kopie-Bits", raeume);
    PRUEF(frei && fremd && !doppelt, "Kopie-Bits frei im Original, die 11 Elza-Bits (1061/1041/Kriecher) nicht Leons Kopien, alle 16 "
          "paarweise verschieden");
    /* Nachtrag 12: die 1031-Cut-7-Kopien tragen GENAU Leons 1030-Cut-7-Bits (Vorlage 6 = EM_1070_DURCH, pc[7] je Satz) in derselben
     * Reihenfolge wie die Quell-Records ROOM1071 0xc6..0xca */
    {
        int nv = 0; const uint8_t *v6 = re15_irons_tod_programm(6, &nv);
        int gleich = v6 && nv >= 100;
        for (int i = 0; i < 5 && gleich; i++) gleich = v6[20 * i] == 0x44 && v6[20 * i + 7] == kb[11 + i];
        PRUEF(gleich, "1031-Cut-7-Kopie-Bits %u,%u,%u,%u,%u = Leons Vorlage 6 (irons_tod_1150.c EM_1070_DURCH) Satz fuer Satz",
              kb[11], kb[12], kb[13], kb[14], kb[15]);
    }
    /* Raum-Paare Leon/Elza: Datei fuer Datei gleich (Leons Treppenhaus-Schritt = Elzas, Elzas 1071-Quelle = Leons 1070) */
    {
        static const uint16_t paar[4][2] = { { 0x1060, 0x1061 }, { 0x10E0, 0x10E1 }, { 0x10C0, 0x10C1 }, { 0x1070, 0x1071 } };
        int ok = 1;
        for (int i = 0; i < 4; i++) {
            size_t na = 0, nb = 0; uint8_t *a = raum_bytes(paar[i][0], &na), *b = raum_bytes(paar[i][1], &nb);
            const int g = a && b && na == nb && memcmp(a, b, na) == 0;
            if (!g) printf("    ROOM%04X != ROOM%04X\n", paar[i][0], paar[i][1]);
            ok &= g;
            free(a); free(b);
        }
        PRUEF(ok, "ROOM1060 = ROOM1061, ROOM10E0 = ROOM10E1, ROOM10C0 = ROOM10C1, ROOM1070 = ROOM1071 (Datei-Bytes gleich)");
    }
    /* Quell-Records ROOM1071 sub00 @0x015CA.. (Liegende) = 0xc6..0xca, Typen 0x10/0x10/0x10/0x11/0x11 */
    {
        int ok = 1;
        static const uint8_t typ[5] = { 0x10, 0x10, 0x10, 0x11, 0x11 };
        for (int i = 0; i < RE15_R39Q_N_1071; i++) {
            const uint8_t *a = d71 + 0x015CA + 20 * i;
            ok &= a[0] == 0x44 && a[2] == typ[i] && a[7] == RE15_R39Q_BIT_1071_0 + i;
        }
        PRUEF(ok, "ROOM1071 sub00 @0x015CA..@0x0161A: fuenf Records, Tot-Bits 0x%02x..0x%02x", RE15_R39Q_BIT_1071_0, RE15_R39Q_BIT_1071_0 + 4);
    }
    PRUEF(s_bit_belegt[RE15_R39Q_BIT_10E1_0] && s_bit_belegt[0x14], "Gegenprobe: Quell-Bit 0x%02x und Rolltor-Record-Bit 0x14 SIND belegt",
          RE15_R39Q_BIT_10E1_0);
    /* 1031 = 1030 dort, wo das Programm Leons Form uebernimmt */
    PRUEF(bytes_gleich(d31 + 0x028CC, d30 + 0x027E0, 0x5A), "ROOM1031 sub09 @0x028CC = ROOM1030 sub09 @0x027E0 (Kriech-Regel, 90 B)");
    PRUEF(bytes_gleich(d31 + 0x025B4, d30 + 0x024C8, 0x28C) && bytes_gleich(d31 + 0x02840, d30 + 0x02754, 16),
          "ROOM1031 sub06 @0x025B4 / sub07 @0x02840 = ROOM1030 sub06 @0x024C8 / sub07 @0x02754");
    PRUEF(d31[0x025CC] == 0x3e && d31[0x025CC + 2] == 0x0f && d31[0x025CC + 4] == 0x05 && d31[0x02844] == 0x26 && le16(d31 + 0x02848) == 0x1000 && d31[0x025D4] == 0x22,
          "Konstanten der Kriech-Regel: Stempel 5 @0x025CC, Bit 0x1000 @0x02844, Set(5,i) @0x025D4");
    /* Kamera-Saetze */
    static const struct { uint16_t raum; int cut; int32_t kx, kz, zx, zz; } kam[5] = {
        { 0x1061, 2, 20199, 21273, 24670, 27767 }, { 0x1031, 12, -11790, -15282, -8640, -26478 }, { 0x1041, 1, -26262, -10692, -25020, -1818 },
        { 0x1031, 7, -26820, -2574, -15858, -5040 }, { 0x1060, 2, 20199, 21273, 24670, 27767 } };   /* Nachtrag 12 */
    int kok = 1;
    for (int i = 0; i < 5; i++) {
        size_t nk = 0; uint8_t *d = raum_bytes(kam[i].raum, &nk);
        if (!d) { kok = 0; continue; }
        const uint8_t *r = d + le32(d + 0x24) + 32 * kam[i].cut;
        kok &= (int32_t)le32(r + 4) == kam[i].kx && (int32_t)le32(r + 12) == kam[i].kz && (int32_t)le32(r + 16) == kam[i].zx &&
               (int32_t)le32(r + 24) == kam[i].zz;
        free(d);
    }
    PRUEF(kok, "Kamera-Saetze 1061 Cut 2 @0x000A0, 1031 Cut 12 @0x001E0, 1041 Cut 1 @0x00080, 1031 Cut 7 @0x00140, 1060 Cut 2 @0x000A0 "
          "= RID-Bytes");
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
        static const uint16_t raeume3[4] = { 0x1041, 0x1061, 0x1031, 0x1060 };   /* 1060: Leons Treppenhaus-Schritt (Nachtrag 12) */
        int slot_frei = 1, ev_frei = 1, b5_frei = 1;
        for (int r = 0; r < 4; r++) {
            size_t n = 0; uint8_t *d = raum_bytes(raeume3[r], &n);
            re15_rdt_t rd; memset(&rd, 0, sizeof rd);
            if (!d || re15_rdt_parse(d, n, &rd) < 0) { slot_frei = 0; continue; }
            if (rd.sub_scd_count > RE15_R39Q_EREIGNIS) ev_frei = 0;
            if (raeume3[r] == 0x1060 && rd.sub_scd_count > RE15_IT_EREIGNIS) ev_frei = 0;   /* Leons Montage-Ereignis 21 in 1060 */
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
                            (d[pc + 2] == RE15_R39Q_SIG_KNALL_TUER || d[pc + 2] == RE15_R39Q_SIG_KNALL_1030 ||
                             d[pc + 2] == RE15_R39Q_SIG_ALLE_DURCH)) b5_frei = 0;
                        uint32_t t = sprungziel(d, pc); if (t > maxf) maxf = t;
                        if (op == 0x01 && pc + 2 > maxf) break;
                        pc += (uint32_t)sz;
                    }
                }
            }
            free(d);
        }
        PRUEF(slot_frei && ev_frei && b5_frei, "Tuer-Slot %d, Ereignis %d (> Sub-Zahl; 1060 auch Leons %d), Bank-5-Bits %d/%d/%d frei in "
              "1041/1061/1031/1060", RE15_R39Q_TUER_SLOT, RE15_R39Q_EREIGNIS, RE15_IT_EREIGNIS, RE15_R39Q_SIG_KNALL_TUER,
              RE15_R39Q_SIG_KNALL_1030, RE15_R39Q_SIG_ALLE_DURCH);
    }
    /* Nachtrag 12, Leon: Programm L (Treppenhaus-Schritt) und die Weiche der Tuer nach 1130 bzw. nach der Szene */
    {
        re15_game_state_init(); g_gameflow.character = 0;          /* Leon, 10E0 nie betreten = alle vier leben */
        re15_elzazombies_r39_reset();
        re15_r39q_leon_umzug();
        PRUEF(re15_r39q_leon_1060_zahl() == 4 && re15_game_flag_get(7, RE15_R39Q_BIT_10E1_0) && !re15_game_flag_get(7, 124),
              "Leon-Umzug: 4 lebende 10E0-Records -> Quellen still, 1060-Kopien (124..127) scharf (%d)", re15_r39q_leon_1060_zahl());
        for (int a = 1; a < RE15_ACTOR_MAX; a++) g_actors[a].active = 0;
        uint8_t buf[640]; int n = re15_r39q_leon_1060_bauen(buf, (int)sizeof buf);
        const re15_r39q_lage_t *lm = NULL; re15_elzazombies_r39_lagen(0, &lm);
        int em_ok = n > 80;
        for (int i = 0; i < 4 && em_ok; i++) {
            const uint8_t *e = buf + 20 * i;
            em_ok = e[0] == 0x44 && e[2] == 0x10 && e[3] == 0x0d && e[4] == RE15_R39Q_ETAGE_1061 && e[7] == 124 + i &&
                    les16(e + 8) == lm[i].x && les16(e + 10) == RE15_R39Q_Y_1061 && les16(e + 12) == lm[i].z && le16(e + 16) == (uint16_t)lm[i].gier;
        }
        int nv1 = 0; const uint8_t *v1 = re15_irons_tod_programm(1, &nv1);
        const int ende_wie_1130 = v1 && n >= 44 && nv1 >= 44 && bytes_gleich(buf + n - 44, v1 + nv1 - 44, 44);
        PRUEF(em_ok && ende_wie_1130 && buf[n - 44 + 2] == RE15_IT_SIG_KNALL_1030 && buf[n - 36 + 23] == 0x04 && buf[n - 36 + 24] == RE15_IT_CUT_1040,
              "Programm L (%d B): vier Sce_em_set Typ 0x10 grid 0x0d Etage 4 Bits 124..127 an den Montage-Lagen (= Elzas Programm B); "
              "Ende Byte fuer Byte wie Leons Programm 1 (k_p_1130): Knall Satz 1, Sleep 45, Tuer -> ROOM1040 Cut 1", n);
        int nv0 = 0; const uint8_t *v0 = re15_irons_tod_programm(1, &nv0);
        uint8_t p1[640]; memcpy(p1, v0, (size_t)nv0);
        re15_r39q_leon_tuer(p1, nv0);
        const uint8_t *d = p1 + nv0 - 36;
        PRUEF(d[0] == 0x3b && d[23] == 0x06 && d[24] == RE15_R39Q_CUT_1061 && d[25] == RE15_R39Q_ETAGE_1061 && les16(d + 14) == 20200 &&
              les16(d + 16) == RE15_R39Q_Y_1061 && les16(d + 18) == 20400 && p1[nv0 - 44 + 2] == RE15_IT_SIG_KNALL_TUER,
              "Weiche: Leons 1130-Tuer -> Raum 0x06 (ROOM1060) Cut 2 Etage 4 auf (20200,%d,20400), Knall davor = Tuerknall Satz 0",
              RE15_R39Q_Y_1061);
        for (int i = 0; i < 4; i++) re15_game_flag_set(7, (uint8_t)(RE15_R39Q_BIT_10E1_0 + i), 1);
        re15_game_flag_set(7, 124, 0);
        re15_r39q_leon_umzug();
        memcpy(p1, v0, (size_t)nv0);
        re15_r39q_leon_tuer(p1, nv0);
        PRUEF(re15_r39q_leon_1060_zahl() == 0 && !memcmp(p1, v0, (size_t)nv0) && re15_game_flag_get(7, 124),
              "10E0 leer: Zahl 0, Leons 1130-Programm bleibt Byte fuer Byte (Tuer -> 1040, Knall Satz 1), Kopien still");
        g_gameflow.character = 4;
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

/* Index des ersten Folge-Eintrags (Elzas Ereignisfolge) der Art/des Raums ab `ab`; -1 = keiner. */
static int q_finde(char art, uint16_t raum, int ab)
{
    for (int i = ab; i < re15_elzazombies_r39_folge_n(); i++) {
        const re15_r39q_folge_t *f = re15_elzazombies_r39_folge(i);
        if (f->art == art && f->raum == raum) return i;
    }
    return -1;
}
static int ist_bit_1071_kopie(int bit) { return bit == 47 || bit == 48 || bit == 49 || bit == 75 || bit == 76; }
/* Programm `pg` traegt die Vorlage `v` (n Byte) woertlich, bis auf die Slot-Bytes pc[1] der Sce_em_set-Saetze (slots_zuweisen). */
static int traegt_vorlage(const uint8_t *pg, int plen, const uint8_t *v, int n)
{
    for (int o = 0; v && n > 0 && o + n <= plen; o++) {
        int gleich = 1;
        for (int j = 0; j < n && gleich; ) {
            const int l = (v[j] == 0x44) ? 20 : 1;
            for (int t = 0; t < l && gleich; t++) if (!(l == 20 && t == 1) && pg[o + j + t] != v[j + t]) gleich = 0;
            j += l;
        }
        if (gleich) return 1;
    }
    return 0;
}

static void teil_kette(int lebend_10e1, int lebend_1071, int an)
{
    printf("== kette %d/%d%s (10E1 lebend %d, 1071 lebend %d) ==\n", lebend_10e1, lebend_1071, an ? "" : " GEGENPROBE Modul aus",
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
        if (ziel == 0x1031) {                                   /* Programm C: Cut-7-Teil nur mit 1071-Ueberlebenden (Nachtrag 12) */
            size_t n31 = 0; uint8_t *d31 = raum_bytes(0x1031, &n31);
            int plen = 0; const uint8_t *pg = re15_elzazombies_r39_laufprogramm(&plen);
            int nv = 0; const uint8_t *v6 = re15_irons_tod_programm(6, &nv);
            const int knall = d31 && im_programm(pg, plen, d31, 0x02862, 12);
            const int leon6 = traegt_vorlage(pg, plen, v6, nv);
            if (lebend_1071 > 0)
                PRUEF(knall && leon6, "Programm C traegt Leons Vorlage 6 (1030-Cut-7-Teil, %d B) und den Original-Se_on ROOM1031 sub08 "
                      "@0x02862 woertlich (Nachtrag 12: 1071-Schritt)", nv);
            else
                PRUEF(!knall && !leon6, "Programm C ohne Cut-7-Teil (1071 leer): kein Original-Se_on @0x02862, der Knall kam als Satz 1 "
                      "am Ende des vorigen Schritts (Leons Form ohne 1070-Kopien)");
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
    uint16_t soll[4]; int nsoll = 0;
    if (lebend_10e1 > 0) soll[nsoll++] = 0x1061;
    soll[nsoll++] = 0x1031;
    soll[nsoll++] = 0x1041;
    int folge_ok = nfolge == nsoll;
    for (int i = 0; i < nsoll && folge_ok; i++) folge_ok = folge[i] == soll[i];
    printf("  Raumfolge:"); for (int i = 0; i < nfolge; i++) printf(" %04X(Cut %d, %d Gegner, %d Bilder)", folge[i], cut_erst[i], spawn_erst[i], bilder[i]); printf("\n");
    printf("  Folge:");
    for (int i = 0; i < re15_elzazombies_r39_folge_n(); i++) {
        const re15_r39q_folge_t *f = re15_elzazombies_r39_folge(i);
        printf(" %c%04X/T%d/%d", f->art, f->raum, f->takt, f->wert);
    }
    printf("\n");
    PRUEF(folge_ok, "Raumfolge 1041 -> %s1031 -> 1041", lebend_10e1 > 0 ? "1061 -> " : "");
    int i1031 = lebend_10e1 > 0 ? 1 : 0;
    if (lebend_10e1 > 0)
        PRUEF(cut_erst[0] == RE15_R39Q_CUT_1061 && spawn_erst[0] == lebend_10e1,
              "1061: erstes Bild Cut %d mit %d Kopien (= lebende 10E1-Zombies %d)", cut_erst[0], spawn_erst[0], lebend_10e1);
    const int cut1031 = lebend_1071 > 0 ? RE15_R39Q_CUT_1031_TUER : RE15_R39Q_CUT_1031_TOR;
    PRUEF(cut_erst[i1031] == cut1031, "1031: erstes Bild Cut %d (%s), soll %d", cut_erst[i1031],
          lebend_1071 > 0 ? "Nachtrag 12: die 1071-Tuer" : "das aufgebrochene Rollo", cut1031);
    /* Knall vor dem 1031-Schritt: Tuerknall Satz 0 zur 1071-Tuer bzw. Satz 1 wie ROOM1030 (Leons knall_setzen-Weiche) */
    {
        const int r31 = q_finde('R', 0x1031, 0);
        int k = -1;
        for (int i = 0; i < r31; i++) if (re15_elzazombies_r39_folge(i)->art == 'K') k = i;
        const re15_r39q_folge_t *K = re15_elzazombies_r39_folge(k);
        PRUEF(K && K->wert == (lebend_1071 > 0 ? 0 : 1), "Knall vor dem 1031-Schritt = Satz %d (%s): %d", lebend_1071 > 0 ? 0 : 1,
              lebend_1071 > 0 ? "Tuerknall, 1071-Tuer" : "wie ROOM1030", K ? K->wert : -1);
        if (lebend_1071 > 0) {
            const int c7 = q_finde('C', 0x1031, 0), k2 = q_finde('K', 0x1031, 0);
            int c12 = -1;
            for (int i = (k2 < 0 ? 0 : k2); i < re15_elzazombies_r39_folge_n(); i++) {
                const re15_r39q_folge_t *f = re15_elzazombies_r39_folge(i);
                if (f->art == 'C' && f->raum == 0x1031 && f->wert == 12) { c12 = i; break; }
            }
            const re15_r39q_folge_t *C7 = re15_elzazombies_r39_folge(c7), *K2 = re15_elzazombies_r39_folge(k2),
                                    *C12 = re15_elzazombies_r39_folge(c12);
            PRUEF(C7 && C7->wert == 7 && K2 && K2->wert == 2 && C12 && c7 < k2 && k2 < c12 && C7->takt < K2->takt,
                  "1031: Cut 7 (T%d) < Original-Knall @0x02862 (T%d) < Cut 12 (T%d) - Leons 1030-Folge", C7 ? C7->takt : -1,
                  K2 ? K2->takt : -1, C12 ? C12->takt : -1);
        }
    }
    const re15_r39q_spur_t *sp = NULL; int n = re15_elzazombies_r39_spuren(&sp);
    int k1061 = 0, n1061 = 0, kriech = 0, nkriech = 0, fremd = 0, n71 = 0, k71n = 0;
    for (int i = 0; i < n; i++) {
        const re15_r39q_spur_t *p = &sp[i];
        const int naeher = p->d0 - p->d1;
        if (p->raum == 0x1061 && p->bit >= 124 && p->bit <= 127) { n1061++; if (naeher >= KOERPERRADIUS && !p->halt) k1061++; }
        if (p->raum == 0x1031 && p->cut == 12 && (p->bit == 133 || p->bit == 144 || p->bit == 145)) { nkriech++; if (naeher >= KOERPERRADIUS) kriech++; }
        if (p->raum == 0x1031 && p->cut == 7 && ist_bit_1071_kopie(p->bit)) { n71++; if (naeher >= KOERPERRADIUS) k71n++; }
        if (p->bit >= 128 && p->bit <= 131) fremd++;
        printf("    Spur ROOM%04X Cut %d Bit %d: %d -> %d (%+d) angehalten %d\n", p->raum, p->cut, p->bit, p->d0, p->d1, -naeher, p->halt);
    }
    if (lebend_10e1 > 0) PRUEF(n1061 == lebend_10e1 && k1061 == n1061, "1061 Cut 2: alle %d Kopien >= %d naeher an der Kamera, keine hinter ihr (%d)", n1061, KOERPERRADIUS, k1061);
    else PRUEF(n1061 == 0, "1061 entfaellt: keine 10E1-Kopie");
    if (lebend_1071 > 0)
        PRUEF(n71 == lebend_1071 && k71n == n71, "1031 Cut 7 (Nachtrag 12): alle %d Kopien der 1071-Zombies >= %d naeher an der Kamera (%d)",
              n71, KOERPERRADIUS, k71n);
    else PRUEF(n71 == 0, "1031 ohne Cut-7-Teil: keine 1071-Kopie");
    PRUEF(nkriech == RE15_R39Q_N_KRIECHER && kriech == nkriech, "1031 Cut 12: drei Kriecher kommen >= %d naeher (%d von %d)", KOERPERRADIUS, kriech, nkriech);
    PRUEF(fremd == 0, "keine 1041-Kopie in der Montage (Bits 128..131: %d Spuren)", fremd);
    PRUEF(hp_min == 100 && re15_elzazombies_r39_hp_min() == 100, "Spieler-HP waehrend der Kette unveraendert (min %d / Takt-Messung %d)",
          hp_min, re15_elzazombies_r39_hp_min());
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    PRUEF(g_current_room_id == 0x1041 && pl->x == -21008 && pl->z == -13134 && pl->rot_y == 2112 &&
          re15_elzazombies_r39_zustand() == RE15_R39Q_AUS && !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27),
          "Rueckkehr nach 1041 an die gemerkte Stelle (%d,%d) Gier %d, Kette AUS, Riegel (2,7)/(1,27) los", (int)pl->x, (int)pl->z, (int)pl->rot_y);
    int quellen = 1, q71 = 0, k71 = 0, k41 = 0;
    static const int b71[5] = { 47, 48, 49, 75, 76 };
    for (int i = 0; i < RE15_R39Q_N_10E1; i++) quellen &= re15_game_flag_get(7, (uint8_t)(RE15_R39Q_BIT_10E1_0 + i));
    for (int i = 0; i < RE15_R39Q_N_1071; i++) q71 += re15_game_flag_get(7, (uint8_t)(RE15_R39Q_BIT_1071_0 + i));   /* ROOM1071 @0x015CA.. */
    for (int i = 0; i < 5; i++) k71 += !re15_game_flag_get(7, (uint8_t)b71[i]);
    for (int i = 128; i <= 131; i++) k41 += re15_game_flag_get(7, (uint8_t)i);
    PRUEF(re15_game_flag_get(9, 95) && quellen, "(9,95)=1, alle 10E1-Records stillgelegt (umgezogen oder tot)");
    PRUEF(q71 == RE15_R39Q_N_1071 && k71 == lebend_1071, "ROOM1071 (Nachtrag 12): alle Quell-Records stillgelegt (%d/5), %d Kopien "
          "scharf (Bits 47,48,49,75,76 = Leons 1030-Cut-7-Bits) = die Ueberlebenden", q71, k71);
    PRUEF(k41 == 4, "1041-Kopien bis zum Durchbruch still (Bits 128..131 = 1: %d)", k41);
    re15_game_flag_set(9, 97, 0); frame(); re15_game_flag_set(9, 97, 1);
    int b2 = 0; int w2 = fahren(300, &b2);
    PRUEF(!w2, "zweite Flanke von (9,97) nach der Montage: keine zweite Kette ((9,95)=1)");
}

/* ================================================================================================================== */
/* Teil durch1041 (Nachtrag 11)                                                                                       */
/* ================================================================================================================== */
static int kopien_bits(int von, int bis, int nord_von_tor)   /* aktive Gegner mit Bit in [von,bis]; nord: nur z > Tor-z */
{
    int n = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->em_flag_id < von || a->em_flag_id > bis) continue;
        if (nord_von_tor && a->z <= RE15_IT_TOR_Z_1040) continue;
        n++;
    }
    return n;
}
/* Zustand nach der Montage: (9,95), 10E1-Quellen still, die ersten `k` 1061-Kopien leben, 1041-Kopien still; die Panikphase
 * laeuft noch ((9,97)=1, Spur P loescht es bei Johns Rettung; H1-Entscheid: Durchbruch nur dann). */
static void nach_montage_flags(int k)
{
    elza_flags();
    re15_game_flag_set(9, 95, 1);
    re15_game_flag_set(9, 97, 1);
    for (int i = 0; i < RE15_R39Q_N_10E1; i++) {
        re15_game_flag_set(7, (uint8_t)(RE15_R39Q_BIT_10E1_0 + i), 1);
        re15_game_flag_set(7, (uint8_t)(124 + i), i < k ? 0 : 1);
        re15_game_flag_set(7, (uint8_t)(128 + i), 1);
    }
}
static void teil_durch1041(void)
{
    printf("== durch1041 ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    const int k = 3;
    nach_montage_flags(k);
    re15_game_flag_set(7, 125, 1);                             /* eine der vier wurde in 1061 getoetet (Bit 125) */
    re15_elzazombies_r39_reset();
    /* Eintritt durch die 1061-Tuer: ROOM1061 Tuer Slot 2 @0x00D52 Nutzlast = Spawn (-21008,0,-13134) Gier 0x0840 Cut 5 */
    if (room_boot(0x1041, -21008, 0, -13134, 2112, 5) != 0) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int lebend = k - 1;
    PRUEF(re15_elzazombies_r39_zustand() == RE15_R39Q_D1041 && re15_elzazombies_r39_durchbruch_zahl() == lebend,
          "Durchbruch startet beim Betreten (Zustand %d, %d umgezogen = lebende 1061-Kopien %d)", re15_elzazombies_r39_zustand(),
          re15_elzazombies_r39_durchbruch_zahl(), lebend);
    PRUEF(g_scd.cam_id == RE15_R39Q_CUT_1041 && pl->x == RE15_IT_PARK_1040_X && pl->z == RE15_IT_PARK_1040_Z,
          "erstes Bild Cut %d (Rolltor-Schnitt), Elza auf Leons Parkplatz (%d,%d) hinter der Kamera", (int)g_scd.cam_id, (int)pl->x, (int)pl->z);
    PRUEF(kopien_bits(128, 131, 1) == lebend && kopien_bits(124, 127, 0) == 0,
          "%d Kopien (Bits 128..131) stehen im Raumaufbau hinter dem Rolltor (z > %d), keine 1061-Kopie", kopien_bits(128, 131, 1),
          RE15_IT_TOR_Z_1040);
    int hp_min = 100, bilder = 0, tor_auf = -1, alle_bild = -1, noch_nord = -1;
    for (int f = 0; f < 2400 && re15_elzazombies_r39_zustand() == RE15_R39Q_D1041; f++) {
        frame(); bilder = f + 1;
        if (pl->hp < hp_min) hp_min = pl->hp;
        if (tor_auf < 0 && re15_game_flag_get(4, 5)) tor_auf = f + 1;
        if (alle_bild < 0 && re15_game_flag_get(5, RE15_R39Q_SIG_ALLE_DURCH)) { alle_bild = f + 1; noch_nord = kopien_bits(128, 131, 1); }
        if (g_room_change.pending) break;
    }
    PRUEF(alle_bild > tor_auf && noch_nord == 0, "\"alle durch\" (5,%d) erst nach dem Rolltor (Bild %d > %d) und wenn keine Kopie mehr "
          "hinter dem Rolltor steht (%d)", RE15_R39Q_SIG_ALLE_DURCH, alle_bild, tor_auf, noch_nord);
    re15_elzazombies_r39_abschnitt_beenden();
    PRUEF(re15_elzazombies_r39_zustand() == RE15_R39Q_AUS && !g_room_change.pending, "Szene endet im Raum nach %d Bildern (kein Raumwechsel)", bilder);
    PRUEF(tor_auf > 0 && re15_game_flag_get(4, 5) && re15_game_flag_get(4, 4), "Rolltor fuhr hoch: (4,5)=1 ab Bild %d, (4,4)=1 (sub01 @0x015A4/@0x015B0)", tor_auf);
    int durch = 0, frei = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->em_flag_id < 128 || a->em_flag_id > 131) continue;
        if (a->z < RE15_IT_TOR_Z_1040) durch++;
        if (!(a->grid_id & RE15_AI_GRID_SKIP)) frei++;
    }
    PRUEF(durch == lebend && frei == lebend, "alle %d Kopien durchs Rolltor (z < %d: %d), am Ende nicht mehr gehalten (%d)", lebend,
          RE15_IT_TOR_Z_1040, durch, frei);
    const re15_r39q_spur_t *sp = NULL; int n = re15_elzazombies_r39_spuren(&sp);
    int nk = 0, kn = 0;
    for (int i = 0; i < n; i++) {
        const re15_r39q_spur_t *p = &sp[i];
        if (p->raum != 0x1041 || p->cut != RE15_R39Q_CUT_1041 || p->bit < 128 || p->bit > 131) continue;
        nk++; if (p->d0 - p->d1 >= KOERPERRADIUS) kn++;
        printf("    Spur ROOM1041 Cut 1 Bit %d: %d -> %d angehalten %d\n", p->bit, p->d0, p->d1, p->halt);
    }
    PRUEF(nk == lebend && kn == lebend, "Cut 1: alle %d Kopien >= %d naeher an der Kamera (%d)", nk, KOERPERRADIUS, kn);
    PRUEF(hp_min == 100, "Spieler-HP waehrend der Szene unveraendert (min %d)", hp_min);
    PRUEF(pl->x == -21008 && pl->z == -13134 && pl->rot_y == 2112 && !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27),
          "Elza zurueck am Eintrittspunkt (%d,%d) Gier %d, Riegel los", (int)pl->x, (int)pl->z, (int)pl->rot_y);
    int b61 = 0, b41 = 0;
    for (int i = 0; i < 4; i++) { b61 += re15_game_flag_get(7, (uint8_t)(124 + i)); b41 += !re15_game_flag_get(7, (uint8_t)(128 + i)); }
    PRUEF(b61 == 4 && b41 == lebend, "Umzug: alle 1061-Kopien still (%d/4), %d 1041-Kopien scharf", b61, b41);
    /* naechstes Betreten: kein zweiter Durchbruch, die Durchgebrochenen sind da (Nachspawn H) */
    if (room_boot(0x1041, -21008, 0, -13134, 2112, 5) != 0) return;
    PRUEF(re15_elzazombies_r39_zustand() == RE15_R39Q_AUS && kopien_bits(128, 131, 0) == lebend,
          "naechstes Betreten: kein zweiter Durchbruch (Zustand %d), %d Durchgebrochene erscheinen", re15_elzazombies_r39_zustand(),
          kopien_bits(128, 131, 0));
    /* Eintritt im Nordteil (ROOM1021 -> 1041, Spawn (-24000,16500)): wartet */
    nach_montage_flags(k);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1041, -24000, 0, 16500, 2048, 0) != 0) return;
    PRUEF(re15_elzazombies_r39_zustand() == RE15_R39Q_AUS && !re15_game_flag_get(7, 124),
          "Eintritt im Nordteil (z 16500 > %d): kein Durchbruch, die 1061-Kopien bleiben", RE15_IT_TOR_Z_1040);
    /* alle 1061-Kopien besiegt: nichts */
    nach_montage_flags(0);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1041, -21008, 0, -13134, 2112, 5) != 0) return;
    PRUEF(re15_elzazombies_r39_zustand() == RE15_R39Q_AUS && kopien_bits(124, 131, 0) == 0,
          "alle Zombies aus 1061 besiegt: kein Durchbruch (\"falls die Zombies nicht besiegt wurden\")");
    /* H1-ENTSCHEID (Nachbesserung 2): John gerettet = (9,97)=0 -> kein Durchbruch mehr; die 1061-Kopien bleiben unberuehrt
     * (1061-Bits 0, 1041-Bits 1), keine Szene, kein Umparken, Rolltor zu. Danach dieselbe Lage mit (9,97)=1: Durchbruch. */
    nach_montage_flags(k);
    re15_game_flag_set(9, 97, 0);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1041, -21008, 0, -13134, 2112, 5) != 0) return;
    {
        int b61 = 0, b41 = 0;
        for (int i = 0; i < 4; i++) { b61 += !re15_game_flag_get(7, (uint8_t)(124 + i)); b41 += re15_game_flag_get(7, (uint8_t)(128 + i)); }
        PRUEF(re15_elzazombies_r39_zustand() == RE15_R39Q_AUS && re15_elzazombies_r39_durchbruch_zahl() == 0 && b61 == k && b41 == 4 &&
              kopien_bits(124, 131, 0) == 0 && pl->x == -21008 && pl->z == -13134 && !re15_game_flag_get(4, 5),
              "H1: (9,97)=0 (John gerettet): kein Durchbruch, %d 1061-Kopien bleiben im Treppenhaus, 1041-Kopien still (%d/4), Elza "
              "steht an der Tuer (%d,%d), Rolltor zu", b61, b41, (int)pl->x, (int)pl->z);
        int w = fahren(200, NULL);
        PRUEF(!w && re15_elzazombies_r39_zustand() == RE15_R39Q_AUS, "H1: 200 Bilder danach kein Durchbruch, kein Raumwechsel");
    }
    nach_montage_flags(k);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1041, -21008, 0, -13134, 2112, 5) != 0) return;
    PRUEF(re15_elzazombies_r39_zustand() == RE15_R39Q_D1041 && re15_elzazombies_r39_durchbruch_zahl() == k,
          "H1 Gegenprobe: dieselbe Lage mit (9,97)=1 -> Durchbruch mit %d Kopien", re15_elzazombies_r39_durchbruch_zahl());
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
    /* Nachtrag 12: Zustand nach der Montage mit zwei 1071-Ueberlebenden (Kopie-Bits 47/48 scharf, 49/75/76 still) */
    re15_game_flag_set(7, 49, 1); re15_game_flag_set(7, 75, 1); re15_game_flag_set(7, 76, 1);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1031, -8200, 0, -22500, 1024, 0) != 0) return;
    const int kr = kopien_im_raum(133, 133) + kopien_im_raum(144, 145), k = kopien_im_raum(124, 132);
    int k71 = 0, an_lage = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || !ist_bit_1071_kopie(a->em_flag_id)) continue;
        k71++;
        /* Leons EM_1070_LISTE (Vorlage 12): Bit 47 (-18600,-3500), Bit 48 (-20600,-3000), Gier 2048, Steher grid 0x00 */
        if ((a->em_flag_id == 47 && a->x == -18600 && a->z == -3500) || (a->em_flag_id == 48 && a->x == -20600 && a->z == -3000)) an_lage++;
    }
    PRUEF(kr == 3 && k == 0 && k71 == 2 && an_lage == 2, "mit (9,95): drei Kriecher, die zwei 1071-Kopien (Bits 47/48) an Leons "
          "Nachspawn-Lagen vor der 1071-Tuer, keine 1061-/1041-Kopie (%d/%d/%d, Lage %d)", kr, k71, k, an_lage);
    elza_flags(); re15_game_flag_set(4, 15, 1);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1031, -8200, 0, -22500, 1024, 0) != 0) return;
    int k71o = 0; for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && ist_bit_1071_kopie(g_actors[i].em_flag_id)) k71o++;
    PRUEF(kopien_im_raum(124, 145) == 0 && k71o == 0, "ohne (9,95): keine Kopie (auch keine 1071-Kopie)");
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
    /* Leon ROOM1040 mit (9,95) und "lebenden 1061-Kopien": kein Rolltor-Durchbruch (nur Elza) */
    re15_game_state_init(); g_gameflow.character = 0; re15_game_flag_set(9, 95, 1);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1040, -21008, 0, -13134, 2112, 5) != 0) return;
    PRUEF(re15_elzazombies_r39_zustand() == RE15_R39Q_AUS && !re15_game_flag_get(4, 5) && kopien_im_raum(124, 145) == 0,
          "Leon ROOM1040 mit (9,95): kein Durchbruch, Rolltor bleibt zu");
    g_gameflow.character = 4;
}

/* ================================================================================================================== */
/* Teile leon_<a>, leon_nach1060 (Nachtrag 12: Leon zusaetzlich das Treppenhaus)                                     */
/* ================================================================================================================== */
/* Irons (Typ 0x45) braucht fuer die Szene in ROOM1150 seine RE1.5-Bank (Muster test_r36_szene1150.c bank_laden). */
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static uint8_t  s_bank45[0x60000];
static void bank_irons(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x45);
    if (eb && eb->ok) return;
    if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
    if (!s_ems15) return;
    int idx = re15_ems_index_for_type(0x45);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) != 0 || len > sizeof s_bank45) return;
    if (!eb) eb = re15_enemy_alloc(0x45);
    if (!eb) return;
    memcpy(s_bank45, s_ems15 + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank45, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; return; }
    eb->ok = 1; eb->buf = NULL;
}
/* Leon (Figur 0) nach der 10F0-Szene (9,71) und der ersten Irons-Szene (3,94) = Freigabe der Todesszene (re15_irons_tod.h);
 * `lebend_10e0` von 4 Medical-Room-Zombies (ROOM10E0 sub00 @0x00D56.. 0xe0..0xe3), `lebend_1140` von 5 Briefing-Room-Zombies
 * (ROOM1140 sub00 @0x00BAA.. 0xd3..0xd7) am Leben. */
static void leon_flags(int lebend_10e0, int lebend_1140)
{
    re15_game_state_init();
    g_gameflow.character = 0;
    re15_game_flag_set(9, 71, 1); re15_game_flag_set(3, 94, 1);
    for (int i = lebend_10e0; i < 4; i++) re15_game_flag_set(7, (uint8_t)(0xe0 + i), 1);
    for (int i = lebend_1140; i < 5; i++) re15_game_flag_set(7, (uint8_t)(0xd3 + i), 1);
}
typedef struct { uint16_t raum; int cut, gegner, bilder; } leon_schritt_t;
/* Leons Kette ab ROOM1150 (Szene) ueber die echten Tuer-Anforderungen bis zum Raumaufbau von ROOM1030; R[i] = Zielraum, erstes
 * Bild (Cut, Gegner im Raumaufbau), Bilder bis zur naechsten Tuer. Ergebnis = Zahl der Raeume. */
static int leon_kette(leon_schritt_t *R, int maxr)
{
    int nr = 0;
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    s_mit_irons = 1;                                           /* room_boot laedt Irons' Bank nach dem Bank-Reset */
    if (room_boot(0x1150, -17150, 0, -11960, 1600, 0) != 0) { s_mit_irons = 0; return 0; }
    PRUEF(re15_irons_tod_zustand() == RE15_IT_SZENE, "Leon: Szene in ROOM1150 gestartet (Zustand %d)", re15_irons_tod_zustand());
    int b = 0;
    if (!fahren(4000, &b)) { PRUEF(0, "Leon 1150: keine Tuer"); s_mit_irons = 0; return 0; }
    while (nr < maxr) {
        re15_elzazombies_r39_abschnitt_beenden();
        const re15_room_change_t rc = g_room_change;
        const uint16_t ziel = (uint16_t)rc.room_id;
        if (room_boot(ziel, rc.x, rc.y, rc.z, (int16_t)rc.yaw_4096, (uint8_t)rc.target_cut) != 0) break;
        R[nr].raum = ziel; R[nr].cut = (int)g_scd.cam_id; R[nr].gegner = aktive_gegner(); R[nr].bilder = 0;
        nr++;
        if (ziel == 0x1030) break;
        if (!fahren(2500, &R[nr - 1].bilder)) { PRUEF(0, "Leon ROOM%04X: keine Tuer", ziel); break; }
    }
    s_mit_irons = 0;
    return nr;
}
static void leon_folge_drucken(void)
{
    printf("  Leons Ereignisfolge:");
    for (int i = 0; i < re15_irons_tod_folge_n(); i++) {
        const re15_it_folge_t *f = re15_irons_tod_folge(i);
        printf(" %c%04X/T%d/%d%s", f->art, f->raum, f->takt, f->wert, f->aufbau ? "a" : "");
    }
    printf("\n");
}
static int leon_finde(char art, uint16_t raum, int ab)
{
    for (int i = ab; i < re15_irons_tod_folge_n(); i++) {
        const re15_it_folge_t *f = re15_irons_tod_folge(i);
        if (f->art == art && f->raum == raum) return i;
    }
    return -1;
}
/* K3-Form je Montage-Raum (Runde 36 Spur K, test_r36_szene1150.c schritt_pruefen): Knall (Ende des vorigen Schritts) < Spawn <
 * Kamera, Spawn und erstes Kamerabild im Raumaufbau, der Knall davor = Satz `satz`. */
static void leon_schritt_pruefen(uint16_t raum, int satz, const char *fall)
{
    const int s = leon_finde('S', raum, 0), c = leon_finde('C', raum, 0);
    int k = -1; for (int i = 0; i < s; i++) if (re15_irons_tod_folge(i)->art == 'K') k = i;
    const re15_it_folge_t *K = re15_irons_tod_folge(k), *Sf = re15_irons_tod_folge(s), *Cf = re15_irons_tod_folge(c);
    PRUEF(K && Sf && Cf && k < s && s < c && K->takt < Sf->takt && Sf->takt <= Cf->takt && Sf->aufbau && Cf->aufbau && K->wert == satz,
          "%s ROOM%04X: Knall Satz %d (#%d T%d) < Spawn (#%d T%d) < Kamera (#%d T%d), beide im Raumaufbau", fall, raum, satz, k,
          K ? K->takt : -1, s, Sf ? Sf->takt : -1, c, Cf ? Cf->takt : -1);
}

static int s_leon_1040_bilder[2] = { -1, -1 };   /* [0] = mit 1060-Schritt, [1] = Gegenprobe ohne (Modul aus) */
static void teil_leon(int lebend_10e0, int lebend_1140, int an)
{
    printf("== leon %d/%d%s (10E0 lebend %d, 1140 lebend %d) ==\n", lebend_10e0, lebend_1140, an ? "" : " GEGENPROBE Modul aus",
           lebend_10e0, lebend_1140);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    leon_flags(lebend_10e0, lebend_1140);
    re15_elzazombies_r39_reset();
    re15_elzazombies_r39_enable(an);
    leon_schritt_t R[8]; memset(R, 0, sizeof R);
    const int nr = leon_kette(R, 8);
    leon_folge_drucken();
    printf("  Raeume:"); for (int i = 0; i < nr; i++) printf(" %04X(Cut %d, %d Gegner, %d Bilder)", R[i].raum, R[i].cut, R[i].gegner, R[i].bilder); printf("\n");
    const int mit1060 = an && lebend_10e0 > 0;
    uint16_t soll[4]; int ns = 0;
    if (lebend_1140 > 0) soll[ns++] = 0x1130;
    if (mit1060) soll[ns++] = 0x1060;
    soll[ns++] = 0x1040; soll[ns++] = 0x1030;
    int ok = nr == ns; for (int i = 0; i < ns && ok; i++) ok = R[i].raum == soll[i];
    PRUEF(ok, "Leon: Raumfolge 1150 -> %s%s1040 -> 1030 (%d Raeume)", lebend_1140 > 0 ? "1130 -> " : "", mit1060 ? "1060 -> " : "", nr);
    int i1060 = -1, i1040 = -1;
    for (int i = 0; i < nr; i++) { if (R[i].raum == 0x1060) i1060 = i; if (R[i].raum == 0x1040) i1040 = i; }
    if (i1040 >= 0 && !an) s_leon_1040_bilder[1] = R[i1040].bilder;
    if (i1040 >= 0 && an && lebend_10e0 > 0) s_leon_1040_bilder[0] = R[i1040].bilder;
    if (!mit1060) {
        PRUEF(i1060 < 0 && leon_finde('S', 0x1060, 0) < 0, "%s: kein Treppenhaus-Schritt", an ? "10E0 leer" : "Gegenprobe Modul aus");
        int k = leon_finde('S', 0x1040, 0) - 1; while (k >= 0 && re15_irons_tod_folge(k)->art != 'K') k--;
        const re15_it_folge_t *K = re15_irons_tod_folge(k);
        PRUEF(K && K->wert == 1, "ohne 1060: der Knall vor 1040 ist wie bisher Satz 1 (%d)", K ? K->wert : -1);
        PRUEF(re15_game_flag_get(RE15_R39Q_BANK, RE15_R39Q_BIT_LEON_1060) == (an ? 1 : 0),
              "NB3: (9,96) = %d (%s)", re15_game_flag_get(RE15_R39Q_BANK, RE15_R39Q_BIT_LEON_1060),
              an ? "Umzug lief, 0 Kopien" : "Modul aus: kein Umzug");
        re15_elzazombies_r39_enable(1);
        re15_irons_tod_zustand_setzen(RE15_IT_AUS);
        return;
    }
    PRUEF(i1060 >= 0 && R[i1060].cut == RE15_R39Q_CUT_1061 && R[i1060].gegner == lebend_10e0,
          "1060: erstes Bild Cut %d mit %d Kopien (= in 10E0 am Leben gelassen %d)", i1060 >= 0 ? R[i1060].cut : -1,
          i1060 >= 0 ? R[i1060].gegner : -1, lebend_10e0);
    PRUEF(i1060 >= 0 && R[i1060].bilder == 1 + 130 + 45, "1060: Schnittlaenge %d Bilder = Leons 1130-Schritt (1 + 130 + 45)",
          i1060 >= 0 ? R[i1060].bilder : -1);
    /* K3 (Runde 36 Spur K) fuer den neuen Schritt und die Knaelle drumherum */
    leon_schritt_pruefen(0x1060, 0, "Leon");                    /* Tuerknall Satz 0: die 10C0-Tuer bricht */
    leon_schritt_pruefen(0x1040, 1, "Leon");                    /* Knall Satz 1 wie ROOM1030, jetzt am Ende von 1060 */
    {
        const int c = leon_finde('C', 0x1060, 0), k = leon_finde('K', 0x1060, 0);
        PRUEF(c >= 0 && k > c && re15_irons_tod_folge(k)->wert == 1, "1060: der Knall des 1040-Schritts (Satz 1) erst NACH dem Bild der Zombies");
    }
    const re15_r39q_spur_t *sp = NULL; const int n = re15_elzazombies_r39_spuren(&sp);
    int nk = 0, kn = 0;
    for (int i = 0; i < n; i++) {
        const re15_r39q_spur_t *p = &sp[i];
        if (p->raum != 0x1060 || p->cut != RE15_R39Q_CUT_1061 || p->bit < 124 || p->bit > 127) continue;
        nk++; if (p->d0 - p->d1 >= KOERPERRADIUS && !p->halt) kn++;
        printf("    Spur ROOM1060 Cut 2 Bit %d: %d -> %d angehalten %d\n", p->bit, p->d0, p->d1, p->halt);
    }
    PRUEF(nk == lebend_10e0 && kn == nk, "1060 Cut 2: alle %d Kopien >= %d naeher an der Kamera, keine vor ihr angehalten (%d)", nk,
          KOERPERRADIUS, kn);
    int hp0 = 0, hpmin = 0; re15_irons_tod_hp(&hp0, &hpmin);
    PRUEF(hp0 == 100 && hpmin == 100, "Leon: Spieler-HP bis 1030 unveraendert (%d -> min %d)", hp0, hpmin);
    int quellen = 1; for (int i = 0; i < 4; i++) quellen &= re15_game_flag_get(7, (uint8_t)(0xe0 + i));
    int scharf = 0; for (int i = 0; i < 4; i++) scharf += !re15_game_flag_get(7, (uint8_t)(124 + i));
    PRUEF(quellen && scharf == lebend_10e0 && !re15_game_flag_get(9, 95) && re15_game_flag_get(RE15_R39Q_BANK, RE15_R39Q_BIT_LEON_1060),
          "Umzug: 10E0-Quellen still, %d 1060-Kopien scharf (= Elzas 1061-Bits 124..127); Elzas (9,95) bleibt 0; (9,96)=1 (NB3)", scharf);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}
/* Vergleich: der 1040-Schritt ist nach dem Treppenhaus derselbe wie vorher (gleiche Bildzahl bis zur Tuer nach 1030). */
static void leon_vergleich(void)
{
    printf("  1040-Schritt: mit 1060 %d Bilder, ohne (Gegenprobe) %d Bilder\n", s_leon_1040_bilder[0], s_leon_1040_bilder[1]);
    PRUEF(s_leon_1040_bilder[0] > 0 && s_leon_1040_bilder[0] == s_leon_1040_bilder[1],
          "Leons 1040-Schritt nach dem Treppenhaus unveraendert (%d = %d Bilder)", s_leon_1040_bilder[0], s_leon_1040_bilder[1]);
}

static void teil_leon_nach1060(void)
{
    printf("== leon_nach1060 ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    /* Zustand nach Leons Montage: (9,73) Szene gesehen, 10E0-Quellen still, 1060-Kopien 124/125 scharf, 126/127 still */
    re15_game_state_init(); g_gameflow.character = 0;
    re15_game_flag_set(9, 71, 1); re15_game_flag_set(3, 94, 1); re15_game_flag_set(9, 73, 1);
    re15_game_flag_set(RE15_R39Q_BANK, RE15_R39Q_BIT_LEON_1060, 1);   /* NB3: Umzug gelaufen (re15_r39q_leon_umzug) */
    for (int i = 0; i < 4; i++) re15_game_flag_set(7, (uint8_t)(0xe0 + i), 1);
    re15_game_flag_set(7, 126, 1); re15_game_flag_set(7, 127, 1);
    re15_elzazombies_r39_reset();
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    /* Eintritt von unten: ROOM1040 Tuer Slot 2 @0x010D8 -> (26000,0,25300) Gier 2048 Cut 7 (= ROOM1041 @0x010D8) */
    if (room_boot(0x1060, 26000, 0, 25300, 2048, 7) != 0) return;
    const re15_r39q_lage_t *lagen = NULL; re15_elzazombies_r39_lagen(1, &lagen);
    int n = 0, an_lage = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->em_flag_id < 124 || a->em_flag_id > 127) continue;
        n++;
        const int k = a->em_flag_id - 124;
        if (a->x == lagen[k].x && a->z == lagen[k].z && a->y == RE15_R39Q_Y_1061 && a->floor == RE15_R39Q_ETAGE_1061) an_lage++;
    }
    PRUEF(n == 2 && an_lage == 2 && re15_elzazombies_r39_1061_gehalten(),
          "Leon ROOM1060 nach der Szene: die zwei lebenden Kopien an ihren Westseiten-Lagen auf Etage 4, gehalten (%d/%d)", an_lage, n);
    for (int f = 0; f < 120; f++) frame();
    PRUEF(re15_elzazombies_r39_1061_gehalten(), "120 Bilder mit Leon auf Etage 0: weiter gehalten");
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 25023; pl->y = RE15_R39Q_Y_1061; pl->z = 24568; pl->rot_y = 3072;
    pl->floor = RE15_R39Q_ETAGE_1061; re15_collision_set_band(RE15_R39Q_ETAGE_1061);
    frame();
    int32_t px = 0, pz = 0, dmin = -1;
    const int nf = re15_elzazombies_r39_freigabe(&px, &pz, &dmin);
    PRUEF(nf == 2 && dmin >= 4000 && !re15_elzazombies_r39_1061_gehalten(),
          "Leon auf Etage 4: %d Kopien frei, kleinster Abstand %d >= 4000 (wie Elza in 1061)", nf, dmin);
    /* ohne (9,96) (Umzug nie gelaufen, Nachbesserung 3): nichts */
    re15_game_flag_set(RE15_R39Q_BANK, RE15_R39Q_BIT_LEON_1060, 0);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1060, 26000, 0, 25300, 2048, 7) != 0) return;
    PRUEF(kopien_im_raum(124, 127) == 0, "Leon ROOM1060 ohne (9,96): keine Kopie");
    re15_game_flag_set(RE15_R39Q_BANK, RE15_R39Q_BIT_LEON_1060, 1);
    /* ohne (9,73) (Szene nicht gesehen): nichts */
    re15_game_flag_set(9, 73, 0);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1060, 26000, 0, 25300, 2048, 7) != 0) return;
    PRUEF(kopien_im_raum(124, 127) == 0, "Leon ROOM1060 ohne (9,73): keine Kopie");
    g_gameflow.character = 4;
}

/* ================================================================================================================== */
/* Teil altstand (Nachbesserung 3, Mangel 1 der Abnahme 2)                                                            */
/* ================================================================================================================== */
/* Leon nach Irons' Tod OHNE den Umzug dieser Spur = jeder Spielstand v0.8.22..v0.8.26 (und master vor Nachbesserung 3):
 * (9,71) 10F0-Szene, (3,94) erste Irons-Szene, (9,73) Todesszene gesehen; Kopie-Bits 124..127 und (9,96) nie gesetzt. */
static void altstand_flags(int tot_10e0)
{
    re15_game_state_init(); g_gameflow.character = 0;
    re15_game_flag_set(9, 71, 1); re15_game_flag_set(3, 94, 1); re15_game_flag_set(9, 73, 1);
    if (tot_10e0) for (int i = 0; i < 4; i++) re15_game_flag_set(7, (uint8_t)(0xe0 + i), 1);
    re15_elzazombies_r39_reset();
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}
static void teil_altstand(void)
{
    printf("== altstand ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    /* (a) Karte der Abnahme M10: alle vier 10E0-Zombies getoetet. Eintritt von unten (ROOM1040 Tuer Slot 2 @0x010D8). */
    altstand_flags(1);
    if (room_boot(0x1060, 26000, 0, 25300, 2048, 7) != 0) return;
    int n0 = aktive_gegner();
    for (int f = 0; f < 90; f++) frame();
    PRUEF(n0 == 0 && aktive_gegner() == 0 && kopien_im_raum(124, 127) == 0,
          "alter Spielstand, 10E0 alle getoetet: ROOM1060 ohne Gegner (Raumaufbau %d, Bild 90 %d) = v0.8.26 (Abnahme 2 M10 Gegenprobe)",
          n0, aktive_gegner());
    /* (b) dieselben Zombies leben noch (10E0 nie betreten): kein Nachspawn in 1060 - sie stehen weiter in ROOM10E0 */
    altstand_flags(0);
    if (room_boot(0x1060, 26000, 0, 25300, 2048, 7) != 0) return;
    const int n1060 = kopien_im_raum(124, 127);
    if (room_boot(0x10E0, -2000, 0, -2000, 0, 0) != 0) return;   /* irgendein Platz: gezaehlt werden nur die Records */
    const int n10e0 = kopien_im_raum(0xe0, 0xe3);
    PRUEF(n1060 == 0 && n10e0 == 4, "alter Spielstand, 10E0 lebt: ROOM1060 %d Kopien, ROOM10E0 %d Zombies (Bits 0xe0..0xe3) - nicht doppelt",
          n1060, n10e0);
    /* (c) GEGENPROBE: Stand (a) mit (9,96)=1 (= Umzug mit vier Lebenden) -> vier Kopien. Ohne den Merker sah der alte Code genau
     *     das in (a). */
    altstand_flags(1);
    re15_game_flag_set(RE15_R39Q_BANK, RE15_R39Q_BIT_LEON_1060, 1);
    if (room_boot(0x1060, 26000, 0, 25300, 2048, 7) != 0) return;
    PRUEF(kopien_im_raum(124, 127) == 4, "Gegenprobe (9,96)=1: %d Kopien (der Riegel unterscheidet die Staende)", kopien_im_raum(124, 127));
    /* (d) der Umzug beim Szenenstart setzt den Merker und die Kopie-Bits: 0xe1/0xe3 leben, 0xe0/0xe2 tot */
    altstand_flags(0);
    re15_game_flag_set(7, 0xe0, 1); re15_game_flag_set(7, 0xe2, 1);
    re15_r39q_leon_umzug();
    int quellen = 1; for (int i = 0; i < 4; i++) quellen &= re15_game_flag_get(7, (uint8_t)(0xe0 + i));
    PRUEF(re15_game_flag_get(RE15_R39Q_BANK, RE15_R39Q_BIT_LEON_1060) && re15_r39q_leon_1060_zahl() == 2 && quellen &&
          re15_game_flag_get(7, 124) && !re15_game_flag_get(7, 125) && re15_game_flag_get(7, 126) && !re15_game_flag_get(7, 127),
          "re15_r39q_leon_umzug: (9,96)=1, Zahl %d, Quellen still, Kopien 125/127 scharf, 124/126 still", re15_r39q_leon_1060_zahl());
    g_gameflow.character = 4;
}

/* ================================================================================================================== */
/* Teil oben (Nachbesserung 3, Mangel 2 der Abnahme 2 + ORCHESTRATOR-ENTSCHEID E1)                                     */
/* ================================================================================================================== */
static int oben_kopien(int32_t *xs, int32_t *zs, int *init, int *halt)
{
    int n = 0; *init = 0; *halt = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->em_flag_id < 124 || a->em_flag_id > 127) continue;
        if (xs) xs[i] = a->x;
        if (zs) zs[i] = a->z;
        n++;
        if (a->state != 0 && a->hp > 0) (*init)++;
        if (a->grid_id & RE15_AI_GRID_SKIP) (*halt)++;
    }
    return n;
}
static void oben_lauf(uint16_t raum, int32_t ank_x, const char *wer)
{
    /* Eintritt von oben: Tuer Etage 8 (ROOM1120/1121 Tuer Slot 0 @0x00C76 -> Raum 0x06, (26000,-14400,25300) Gier 2048, Cut 0) */
    if (room_boot(raum, 26000, -14400, 25300, 2048, 0) != 0) return;
    const re15_r39q_lage_t *lagen = NULL; re15_elzazombies_r39_lagen(3, &lagen);
    int an_lage = 0, ost = 0, init = 0, halt = 0;
    int32_t x0[RE15_ACTOR_MAX] = {0}, z0[RE15_ACTOR_MAX] = {0};
    const int n = oben_kopien(NULL, NULL, &init, &halt);
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->em_flag_id < 124 || a->em_flag_id > 127) continue;
        const int k = a->em_flag_id - 124;
        if (a->x == lagen[k].x && a->z == lagen[k].z && a->y == RE15_R39Q_Y_1061 && a->floor == RE15_R39Q_ETAGE_1061) an_lage++;
        if (a->x >= 25500) ost++;
    }
    PRUEF(n == 4 && an_lage == 4 && ost == 4 && re15_elzazombies_r39_nach_oben() == 1,
          "%s ROOM%04X von oben: 4 Kopien an den Ostseiten-Lagen (Lagen 3) auf Etage 4 (%d/%d, x >= 25500: %d)", wer, raum, an_lage, n, ost);
    PRUEF(init == 0 && halt == 0, "%s: im Spawn-Bild noch kein INIT und KEIN Haltebit (%d initialisiert, %d gehalten) - sonst liefe der INIT nie",
          wer, init, halt);
    frame();
    oben_kopien(NULL, NULL, &init, &halt);
    PRUEF(init == 4, "%s: nach dem ersten KI-Takt alle 4 initialisiert (state != 0, HP > 0): %d", wer, init);
    frame();
    oben_kopien(x0, z0, &init, &halt);
    PRUEF(halt == 4 && re15_elzazombies_r39_1061_gehalten(), "%s: ab dem zweiten Bild gehalten (%d)", wer, halt);
    for (int f = 0; f < 200; f++) frame();
    int still = 1, init2 = 0, halt2 = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->em_flag_id < 124 || a->em_flag_id > 127) continue;
        if (labs((long)a->x - x0[i]) + labs((long)a->z - z0[i]) > 50) still = 0;
    }
    oben_kopien(NULL, NULL, &init2, &halt2);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    PRUEF(still && init2 == 4 && halt2 == 4 && pl->hp == 100,
          "%s: 200 Bilder auf Etage 8: stehen initialisiert und gehalten (Weg <= 50, %d initialisiert), Spieler-HP %d", wer, init2, pl->hp);
    /* Ankunft am Ende der Treppe A (gemessen: Leon (20364,25368), Elza (20432,25368)) */
    pl->x = ank_x; pl->y = RE15_R39Q_Y_1061; pl->z = 25368; pl->rot_y = 3072;
    pl->floor = RE15_R39Q_ETAGE_1061; re15_collision_set_band(RE15_R39Q_ETAGE_1061);
    frame();
    int32_t px = 0, pz = 0, dmin = -1;
    const int nf = re15_elzazombies_r39_freigabe(&px, &pz, &dmin);
    PRUEF(nf == 4 && dmin >= 4591 && !re15_elzazombies_r39_1061_gehalten(),
          "%s auf Etage 4 bei (%d,25368): %d Kopien frei, kleinster Abstand %d >= 4591 (= gemessen von unten, Abnahme 2 M5)", wer, ank_x,
          nf, dmin);
    int32_t d_vorher = dmin, d_nach = 1 << 30;
    int hpmin = pl->hp;
    for (int f = 0; f < 90; f++) { frame(); if (pl->hp < hpmin) hpmin = pl->hp; }
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->em_flag_id < 124 || a->em_flag_id > 127) continue;
        int32_t d = (int32_t)lround(sqrt((double)(a->x - pl->x) * (a->x - pl->x) + (double)(a->z - pl->z) * (a->z - pl->z)));
        if (d < d_nach) d_nach = d;
    }
    PRUEF(d_nach < d_vorher - KOERPERRADIUS && hpmin == 100,
          "%s: nach der Freigabe kommen sie naeher (%d -> %d in 90 Bildern), kein Griff (HP min %d)", wer, d_vorher, d_nach, hpmin);
}
static void teil_oben(void)
{
    printf("== oben ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    /* Elza: Zustand nach der Montage ((9,95), 10E1-Quellen still, alle vier 1061-Kopien scharf) */
    elza_flags();
    re15_game_flag_set(9, 95, 1);
    for (int i = 0; i < RE15_R39Q_N_10E1; i++) re15_game_flag_set(7, (uint8_t)(RE15_R39Q_BIT_10E1_0 + i), 1);
    re15_elzazombies_r39_reset();
    oben_lauf(0x1061, 20432, "Elza");
    /* Elza durch die 10C1-Tuer (ROOM10C1 @0x00E42 -> (26000,-7200,25300) Cut 2): Westseite, sofort frei */
    re15_elzazombies_r39_reset();
    if (room_boot(0x1061, 26000, -7200, 25300, 2048, 2) != 0) return;
    {
        const re15_r39q_lage_t *lagen = NULL; re15_elzazombies_r39_lagen(1, &lagen);
        int an_lage = 0;
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            const re15_actor_t *a = &g_actors[i];
            if (!a->active || a->em_flag_id < 124 || a->em_flag_id > 127) continue;
            const int k = a->em_flag_id - 124;
            if (a->x == lagen[k].x && a->z == lagen[k].z) an_lage++;
        }
        int32_t px = 0, pz = 0, dmin = -1;
        const int nf = re15_elzazombies_r39_freigabe(&px, &pz, &dmin);
        PRUEF(an_lage == 4 && re15_elzazombies_r39_nach_oben() == 0 && nf == 4 && dmin >= 4591 && !re15_elzazombies_r39_1061_gehalten(),
              "Elza durch die 10C1-Tuer (Etage 4): Westseiten-Lagen (%d/4), sofort frei, kleinster Abstand %d >= 4591", an_lage, dmin);
    }
    /* Leon: Zustand nach seiner Montage ((9,73), (9,96), 10E0-Quellen still, alle vier 1060-Kopien scharf) */
    re15_game_state_init(); g_gameflow.character = 0;
    re15_game_flag_set(9, 71, 1); re15_game_flag_set(3, 94, 1); re15_game_flag_set(9, 73, 1);
    re15_game_flag_set(RE15_R39Q_BANK, RE15_R39Q_BIT_LEON_1060, 1);
    for (int i = 0; i < 4; i++) re15_game_flag_set(7, (uint8_t)(0xe0 + i), 1);
    re15_elzazombies_r39_reset();
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    oben_lauf(0x1060, 20364, "Leon");
    g_gameflow.character = 4;
}

/* ================================================================================================================== */
/* auswerten_ton <laut.raw> <stumm.raw>: integration_r39_elzazombies_knall — zwei RE15_AUDIO_CAP_SYNC-Mitschnitte desselben  */
/* Laufs, einmal mit RE15_R39Q_KNALL_STUMM (Signal verbraucht, nicht gespielt). Je Spielbild 1470 Stereo-Abtastungen s16  */
/* (audio_pc.c, Offset = Bild * 1470). Die Differenz = die Knaelle der Kette (Satz 0 Auftakt 1041, Satz 1 Ende 1061).    */
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
    PRUEF(gruppen == 2 && lang_ok == 2, "zwei hoerbare Knaelle (Knall-Bank Satz 0 Auftakt 1041; Ende 1061 Satz 0 vor der 1071-Tuer bzw. "
          "Satz 1 wie ROOM1030 ohne 1071-Ueberlebende) in der Kette: %d Gruppen, "
          "%d >= 20 Bilder und RMS >= 1000",
          gruppen, lang_ok);
    free(a); free(b);
    return g_fail;
}

/* ================================================================================================================== */
/* VERTRAG Runde 39 Abschnitt 2 "keine Umgebungsvariablen des Aufrufers erben" (Nachbesserung 1). Die Engine liest an     */
/* vielen Stellen RE15_*-Schalter; ctest reicht die Umgebung der Shell durch. Gemessen (Bau 02:20, ohne diese Funktion):  */
/* geerbtes RE15_RE2_DMG_MODEL=0 verschob in kette_4 die Spur-Lagen (ROOM1061 Aktor 1 Bit 124 6245 -> 6194, ROOM1031    */
/* Aktor 1 Bit 0 8997 -> 9836), RE15_SCD_TRACE=1 verlaengerte kette_4 um 112177 Zeilen. Darum loescht main() vor dem     */
/* ersten Engine-Aufruf jede RE15_*-Variable, ohne Ruecksicht auf die Schreibweise: Windows liest Namen ohne sie         */
/* (gemessen: "re15_scd_trace=1" schaltete den Trace ein, nach1061 15 -> 325 Zeilen).                                    */
/* ================================================================================================================== */
#ifdef _WIN32
#  define R39Q_UMGEBUNG _environ
#else
extern char **environ;
#  define R39Q_UMGEBUNG environ
#endif
static int ist_re15_name(const char *s)
{
    return (s[0] == 'R' || s[0] == 'r') && (s[1] == 'E' || s[1] == 'e') && s[2] == '1' && s[3] == '5' && s[4] == '_';
}
/* -> Zahl der RE15_*-Namen, die danach noch in der Umgebung stehen (0 = sauber, -1 = kein Speicher) */
static int umgebung_leeren(void)
{
    size_t n = 0, k = 0;
    for (char **e = R39Q_UMGEBUNG; e && *e; e++) n++;
    char **namen = (char **)calloc(n + 1, sizeof *namen);
    if (!namen) return -1;
    for (char **e = R39Q_UMGEBUNG; e && *e && k < n; e++) {
        size_t l = strcspn(*e, "=");
        if (l < 5 || !ist_re15_name(*e)) continue;
        namen[k] = (char *)malloc(l + 2);
        if (!namen[k]) break;
        memcpy(namen[k], *e, l);
        namen[k][l] = '=';
        namen[k][l + 1] = '\0';
        k++;
    }
    for (size_t i = 0; i < k; i++) {
        size_t l = strlen(namen[i]) - 1;
        printf("umgebung: geerbt %.*s -> geloescht\n", (int)l, namen[i]);
#ifdef _WIN32
        _putenv(namen[i]);                 /* "NAME=" entfernt die Variable (CRT-Tabelle und Prozess-Umgebung) */
#else
        namen[i][l] = '\0';
        unsetenv(namen[i]);
#endif
        free(namen[i]);
    }
    free(namen);
    int rest = 0;
    for (char **e = R39Q_UMGEBUNG; e && *e; e++)
        if (strcspn(*e, "=") >= 5 && ist_re15_name(*e)) rest++;
    return rest;
}

int main(int argc, char **argv)
{
    int rest = umgebung_leeren();
    if (rest != 0) {
        printf("  FEHLER: geerbte RE15_*-Umgebung laesst sich nicht loeschen (%d)\n", rest);
        return 1;
    }
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "auswerten_ton")) {
        if (argc < 4) { printf("auswerten_ton <laut.raw> <stumm.raw>\n"); return 2; }
        int f = auswerten_ton(argv[2], argv[3]);
        printf("%s: %d Fehler\n", teil, f);
        return f ? 1 : 0;
    }
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "zensus"))    teil_zensus();
    if (alle || !strcmp(teil, "kette_4"))   { teil_kette(4, 5, 1); teil_kette(4, 5, 0); }
    if (alle || !strcmp(teil, "kette_0"))   teil_kette(0, 5, 1);    /* 10E1 leer, 1071 voll: Auftakt -> 1031 Cut 7 (Nachtrag 12) */
    if (alle || !strcmp(teil, "kette_2"))   teil_kette(2, 3, 1);
    if (alle || !strcmp(teil, "kette_00"))  teil_kette(0, 0, 1);    /* beide leer: Auftakt -> 1031 Cut 12 (Rollo) */
    if (alle || !strcmp(teil, "leon_4"))    { teil_leon(4, 5, 1); teil_leon(4, 5, 0); leon_vergleich(); }
    if (alle || !strcmp(teil, "leon_2"))    teil_leon(2, 0, 1);     /* 1140 leer: Szene -> 1060 -> 1040 */
    if (alle || !strcmp(teil, "leon_0"))    teil_leon(0, 5, 1);     /* 10E0 leer: Kette wie bisher */
    if (alle || !strcmp(teil, "leon_nach1060")) teil_leon_nach1060();
    if (alle || !strcmp(teil, "altstand"))  teil_altstand();     /* Nachbesserung 3, Mangel 1 */
    if (alle || !strcmp(teil, "oben"))      teil_oben();         /* Nachbesserung 3, Mangel 2 / E1 */
    if (alle || !strcmp(teil, "nach1061"))  teil_nach1061();
    if (alle || !strcmp(teil, "nach1031"))  teil_nach1031();
    if (alle || !strcmp(teil, "durch1041")) teil_durch1041();
    if (alle || !strcmp(teil, "ohne"))      teil_ohne();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
