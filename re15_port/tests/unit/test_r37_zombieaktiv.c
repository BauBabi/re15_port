/*
 * Runde 37 Spur H "zombieaktiv" — RIEGEL. Dossier analysis/befunde_runde37/H_zombieaktiv.md, Mechanik und Belege
 * include/re15_zombieaktiv_r37.h.
 *
 * NUTZER: "die Zombies sind immer noch zu passiv ... nach Irons tod - die Zombies bleiben einfach nur Stumpf alle im Gang
 * stehen ... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im Gang." / "das darf generell für die
 * Zombies gelten" / "schaue einfach nach wie Resident Evil 2 das macht".
 *
 * RE2 (Original, gemessen: DuckStation-GDB ROOM20A0, H_belege/re2_original_room20a0_takte.txt): ein Zombie mit
 * Spawn-Deskriptor 6 steht ab Takt 0 auf Zustandswort 0x0101 (INIT `addiu v0,zero,257` / `sw v0,4(s2)` @0x80100ab8-abc)
 * und ab TAKT 2 im Gang (0x00010101); er verkuerzt den Abstand zu Leon aus 13660..18992 mit 15,4..32,6 Einheiten je Takt
 * (kleinste 100-Takt-Rate 1536 / 100). Mit Deskriptor 0 bleibt derselbe Zombie stehen (Zustandswort 0x0001).
 *
 * Teile (argv[1]):
 *   abbildung    die Spawn-Abbildung am echten Sce_em_set (VM-Programm `44 ..` + Evt_end): fuer JEDE RE1.5-Zombie-Form
 *                (grid 0x00 0x02 0x06 0x0d 0x0e 0x81 0x83 0x84 0x85 0x86 0x87 0x88 0xa1 0xae, Zensus der 240 RDTs)
 *                Zustand nach dem INIT mit und ohne Abbildung: NUR grid 0x00 aendert sich (Stehen 1/0 -> Gang 1/1,
 *                spaetestens Bild 2 wie RE2 Takt 2); ein Zombie OHNE Sce_em_set-Spawn (grid 0) bleibt Steher; waehrend
 *                eines Montage-Schritts (Spieler geparkt) bleibt grid 0x00 Steher.
 *   nachspawn1130  das Nutzer-Beispiel: ROOM1130 nach der Montage (Bits (9,73)/(9,74) .. wie nach der Kette), Leon an der
 *                Tuer aus 1150 (Spawn ROOM1150 Slot 0 @0x00D5E (-5900,16350)), keine Eingabe: alle fuenf Montage-
 *                Zombies (Nachspawn Programm 10, grid 0x0d) spaetestens Bild 2 im Gang (1/1) und nach 300 Bildern
 *                jeder mindestens 300 x 15 = 4500 naeher an Leon (RE2-Original: kleinste Rate 15,4 je Takt).
 *                Gegenprobe Stand v0.8.24 (re15_r37_zombieaktiv_enable(0): Nachspawn grid 0x02 wie bisher): 600 Bilder,
 *                KEINER erreicht den Gang (Unterzustand 1) — der RE2-Steher wacht nur < 0x1D4C (@0x80101544-7c), Leon ist
 *                >= 20000 entfernt = der Nutzer-Befund.
 *   raum1180     ein Raum mit grid-0x00-Zombies im Normalspiel (ROOM1180 main00, drei Records): Leon am Eintritt aus
 *                1160, jeder grid-0x00-Zombie spaetestens Bild 2 im Gang, nach 300 Bildern naeher; Gegenprobe: keiner.
 * "mess <teil>" druckt die Zeitlinien (je Zombie Abstand in Bild 0/2/100/300/600, Zustandsfolge) statt zu pruefen.
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
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_skeleton.h"
#include "re2_ems.h"
#include "re15_irons_tod.h"
#include "re15_zombieaktiv_r37.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

static int fails = 0, checks = 0, s_mess = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

/* RE2-Original (R4 im Dossier): Gang ab Takt 2, kleinste 100-Takt-Rate 1536/100 -> 15 Einheiten je Takt (abgerundet). */
#define R37_BILD_GANG     2
#define R37_RATE_MIN      15
#define R37_BILDER_RATE   300

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static void load_bank_re2(uint8_t type)
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

static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

static int zombie_typ(unsigned t) { return t == 0x10 || t == 0x11 || t == 0x12 || t == 0x13 || t == 0x16 || t == 0x18; }

static uint8_t *s_raw; static size_t s_n;
static int raum_laden(uint16_t raum)
{
    if (s_raw) { free(s_raw); s_raw = NULL; }
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE%X/ROOM%04X.RDT", raum >> 12, raum);
    s_raw = slurp(p, &s_n);
    return s_raw != NULL;
}

/* Grundzustand eines Raums (Muster test_r36_zombieki_eintritt.c eintreten_an), Flags bleiben wie gesetzt. */
static void grundzustand(uint16_t raum, int cut, int32_t px, int32_t pz, int16_t rot)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_player_victim_reset();
    re15_pauseflags_clear();
    re15_damage_seed_rng(0x2545f491u);
    re15_inv_load_briefing();
    g_current_room_id = raum; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = rot;
    pl->floor = (uint8_t)re15_collision_band_from_y(0);
    re15_collision_set_band(pl->floor);
    s_shown = cut;
    load_bank_re2(0x10); load_bank_re2(0x11); load_bank_re2(0x12); load_bank_re2(0x16); load_bank_re2(0x18);
}

static int eintreten(uint16_t raum, int cut, int32_t px, int32_t pz, int16_t rot)
{
    if (!raum_laden(raum)) return 0;
    if (re15_rdt_parse(s_raw, s_n, &g_room_rdt) != 0) return 0;
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    grundzustand(raum, cut, px, pz, rot);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, px, pz, cut);
    g_scd.cut_auto_enabled = 1;
    return 1;
}

static int32_t isqrt64(long long v) { if (v <= 0) return 0; long long r = 0; long long b = 1LL << 31;
    while (b) { long long t = r + b; if (t * t <= v) r = t; b >>= 1; } return (int32_t)r; }
static int32_t abst(const re15_actor_t *a, const re15_actor_t *b)
{ long long dx = a->x - b->x, dz = a->z - b->z; return isqrt64(dx * dx + dz * dz); }

/* ------------------------------------------------------------------------------------------------------------------
 * Teil "abbildung": ein einzelner Sce_em_set ueber die VM (Opcode-Bytes wie im RDT: `44 ss tt gg 00 00 p6 pp xx xx yy yy
 * zz zz 00 00 rr rr 00 00`, ROOM1140 sub00 @0x00BAA) + Evt_end, dann zwei Bilder. Rueckgabe: Zustand/Unterzustand. */
static void spawn_einzeln(uint8_t typ, uint8_t grid, int an, int *st, int *ss1, int *bild_gang)
{
    re15_r37_zombieaktiv_enable(an);
    static uint8_t prog[24];
    const int16_t x = -1000, z = -1000, rot = 0;
    memset(prog, 0, sizeof prog);
    prog[0] = 0x44; prog[1] = 0x00; prog[2] = typ; prog[3] = grid; prog[6] = 0x00; prog[7] = 0xff;
    prog[8] = (uint8_t)(x & 0xff); prog[9] = (uint8_t)((x >> 8) & 0xff);
    prog[12] = (uint8_t)(z & 0xff); prog[13] = (uint8_t)((z >> 8) & 0xff);
    prog[16] = (uint8_t)(rot & 0xff); prog[17] = (uint8_t)((rot >> 8) & 0xff);
    prog[20] = 0x01; prog[21] = 0x00;                      /* Evt_end */
    g_scd.work_vars[0x11] = 0; g_scd.work_vars[0x12] = 20; /* Gleichzeitig-Limit (@0x80042214-3c) nicht im Weg */
    scd_thread_start(5, prog);
    *bild_gang = 0;
    for (int fr = 1; fr <= 3; fr++) {
        frame();
        const re15_actor_t *e = &g_actors[1];
        if (!*bild_gang && e->active && e->state == 1 && e->sub_state_1 == 1) *bild_gang = fr;
    }
    *st = g_actors[1].state; *ss1 = g_actors[1].sub_state_1;
}

static int teil_abbildung(void)
{
    /* Raum mit freiem Boden bei (-1000,-1000) nicht noetig: der INIT ist lageunabhaengig; ROOM1140 als Kulisse. */
    static const uint8_t grids[] = { 0x00, 0x02, 0x06, 0x0d, 0x0e, 0x81, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0xa1, 0xae };
    int nur00 = 1, gang00 = 0, bild00 = 99, anders = 0;
    for (unsigned gi = 0; gi < sizeof grids; gi++) {
        int st[2], ss[2], bg[2];
        for (int an = 0; an < 2; an++) {
            if (!raum_laden(0x1140) || re15_rdt_parse(s_raw, s_n, &g_room_rdt) != 0) return 0;
            g_room_rdt_ok = 1; s_rdt = g_room_rdt;
            grundzustand(0x1140, 0, -20000, 20000, 0);
            spawn_einzeln(0x10, grids[gi], an, &st[an], &ss[an], &bg[an]);
        }
        printf("  grid 0x%02x: ohne Abbildung Zustand %d/%d, mit Abbildung %d/%d (Gang ab Bild %d)\n", grids[gi],
               st[0], ss[0], st[1], ss[1], bg[1]);
        if (grids[gi] == 0x00) {
            gang00 = (st[0] == 1 && ss[0] == 0 && st[1] == 1 && ss[1] == 1);
            bild00 = bg[1];
        } else if (st[0] != st[1] || ss[0] != ss[1]) { nur00 = 0; anders++; }
    }
    re15_r37_zombieaktiv_enable(1);
    if (s_mess) return 1;
    CHECK(gang00, "grid 0x00 (RE1.5 FUN_80101b64, ENGAGE ohne Abstandsgrenze @0x80101bf4-c20): v0.8.24 Stehen 1/0, jetzt RE2-Gang 1/1 "
          "(Deskriptor 6 @0x80100ab8-abc)");
    CHECK(bild00 >= 1 && bild00 <= R37_BILD_GANG, "grid 0x00: Gang spaetestens Bild %d (RE2-Original Takt 2): Bild %d", R37_BILD_GANG, bild00);
    CHECK(nur00 && anders == 0, "alle anderen 13 RE1.5-Formen (0x02 Schlaefer, 0x0d, Fresser, Liegende, Kriecher, Sitzer) unveraendert (%d abweichend)", anders);

    /* ohne Sce_em_set (Sonden-/Werkzeug-Spawn, grid 0): bleibt Steher — die Marke gibt es nur fuer echte Spawn-Saetze.
     * (a) ohne vorherige Marke; (b) mit liegengebliebener Marke eines Satzes, dessen INIT nie lief (Aktor danach neu
     * belegt, andere Lage): die Pruefung Typ + Spawn-Lage im INIT verwirft sie. */
    for (int fall = 0; fall < 2; fall++) {
        if (!raum_laden(0x1140) || re15_rdt_parse(s_raw, s_n, &g_room_rdt) != 0) return 0;
        g_room_rdt_ok = 1; s_rdt = g_room_rdt;
        grundzustand(0x1140, 0, -20000, 20000, 0);
        if (fall == 1) {                                  /* Marke setzen, ohne den INIT laufen zu lassen */
            static const uint8_t p2[] = { 0x44, 0x00, 0x10, 0x00, 0, 0, 0, 0xff, 0x18, 0xfc, 0, 0, 0x18, 0xfc, 0, 0, 0, 0, 0, 0, 0x01, 0x00 };
            g_scd.work_vars[0x11] = 0; g_scd.work_vars[0x12] = 20;
            scd_thread_start(5, p2);
            scd_vm_tick();                                /* nur die VM: Spawn bei (-1000,-1000), KEIN KI-Takt */
            re15_actor_init();
        }
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = -20000; pl->z = 20000;
        re15_actor_t *e = &g_actors[1];
        memset(e, 0, sizeof *e);
        e->active = 1; e->type = 0x10; e->grid_id = 0; e->state = 0; e->x = -1500; e->z = -1000; e->flags = 1;
        re15_enemy_apply_hitbox(e, 0x10);
        frame(); frame();
        CHECK(e->state == 1 && e->sub_state_1 == 0, "Zombie ohne Sce_em_set (grid 0)%s: Steher 1/0 wie bisher (%d/%d)",
              fall ? ", liegengebliebene Marke an anderer Lage" : "", e->state, e->sub_state_1);
    }
    /* Montage-Schritt (Spieler geparkt, re15_irons_tod.h): grid 0x00 bleibt Steher */
    {
        int st, ss, bg;
        if (!raum_laden(0x1140) || re15_rdt_parse(s_raw, s_n, &g_room_rdt) != 0) return 0;
        g_room_rdt_ok = 1; s_rdt = g_room_rdt;
        grundzustand(0x1140, 0, -20000, 20000, 0);
        re15_irons_tod_zustand_setzen(RE15_IT_S1030);
        spawn_einzeln(0x10, 0x00, 1, &st, &ss, &bg);
        re15_irons_tod_zustand_setzen(RE15_IT_AUS);
        CHECK(st == 1 && ss == 0, "grid 0x00 waehrend eines Montage-Schritts (Zustand S1030, Spieler geparkt): Steher 1/0 (%d/%d)", st, ss);
    }
    return 1;
}

/* ------------------------------------------------------------------------------------------------------------------
 * Gemeinsamer Lauf: Raum betreten, Leon steht, Zeitlinie je Zombie. */
typedef struct {
    int slot; unsigned typ; uint8_t grid;
    int32_t d0, d2, d100, d300, d600;
    int bild_gang;       /* erstes Bild im Gang (Zustand 1 / Unterzustand 1) */
    int nie_gang;        /* 1 = in keinem Bild Unterzustand 1 */
} zl_t;

static int lauf(uint16_t raum, int cut, int32_t px, int32_t pz, int16_t rot, int bilder, zl_t *zs, int *nz)
{
    if (!eintreten(raum, cut, px, pz, rot)) return 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    *nz = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || !zombie_typ(e->type)) continue;
        zl_t *z = &zs[(*nz)++]; memset(z, 0, sizeof *z);
        z->slot = s; z->typ = e->type; z->grid = e->grid_id; z->d0 = abst(pl, e); z->nie_gang = 1;
    }
    for (int fr = 1; fr <= bilder; fr++) {
        pl->hp = 100;                                   /* Leon steht (keine Eingabe); stirbt nicht, damit der Lauf zaehlt */
        frame();
        for (int k = 0; k < *nz; k++) {
            zl_t *z = &zs[k]; re15_actor_t *e = &g_actors[z->slot];
            if (!e->active) continue;
            int32_t d = abst(pl, e);
            if (e->state == 1 && e->sub_state_1 == 1) { if (!z->bild_gang) z->bild_gang = fr; z->nie_gang = 0; }
            if (fr == 2) z->d2 = d;
            if (fr == 100) z->d100 = d;
            if (fr == 300) z->d300 = d;
            if (fr == 600) z->d600 = d;
        }
    }
    if (s_mess)
        for (int k = 0; k < *nz; k++)
            printf("  ROOM%04X slot %2d typ %02x grid %02x: Gang ab Bild %d%s | Abstand Bild 0 %6d, 2 %6d, 100 %6d, 300 %6d, 600 %6d\n",
                   raum, zs[k].slot, zs[k].typ, zs[k].grid, zs[k].bild_gang, zs[k].nie_gang ? " (nie)" : "",
                   (int)zs[k].d0, (int)zs[k].d2, (int)zs[k].d100, (int)zs[k].d300, (int)zs[k].d600);
    return 1;
}

/* Flags wie nach der Montage-Kette (re15_irons_tod.h): (9,71) 10F0-Szene, (3,94) erste Irons-Szene, (9,73) Todesszene
 * gesehen, (9,74)/(9,75)/(9,76) Spawns 1130/1040/1030 verbraucht, Rolltor 1040 offen (4,4)/(4,5), Tor 1030 (4,15),
 * Zone-7-Tot-Bits der umgezogenen 1140-/1070-Records (umzug_vorbereiten). */
static void flags_nach_montage(void)
{
    re15_game_state_init();
    re15_game_flag_set(9, 71, 1); re15_game_flag_set(3, 94, 1);
    for (int b = 73; b <= 76; b++) re15_game_flag_set(9, (uint8_t)b, 1);
    re15_game_flag_set(4, 4, 1); re15_game_flag_set(4, 5, 1); re15_game_flag_set(4, 15, 1);
    for (int i = 0; i < 5; i++) { re15_game_flag_set(7, (uint8_t)(0xd3 + i), 1); re15_game_flag_set(7, (uint8_t)(0xc6 + i), 1); }
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

static int teil_nachspawn1130(void)
{
    static zl_t zs[RE15_ACTOR_MAX]; int nz = 0;
    /* Leon = Spawn der Tuer ROOM1150 Slot 0 @0x00D5E -> ROOM1130 (-5900,0,16350) Gierung 0, Ziel-Cut 0 */
    for (int an = 1; an >= 0; an--) {
        flags_nach_montage();
        re15_r37_zombieaktiv_enable(an);
        printf("%s\n", an ? "MIT Runde 37:" : "GEGENPROBE Stand v0.8.24 (Nachspawn grid 0x02, keine Abbildung):");
        if (!lauf(0x1130, 0, -5900, 16350, 0, 600, zs, &nz)) { CHECK(0, "ROOM1130 nicht ladbar"); return 0; }
        if (s_mess) continue;
        int n_gang = 0, n_nah = 0, n_nie = 0; int32_t min_weg = 1 << 30; int bild_max = 0;
        for (int k = 0; k < nz; k++) {
            if (zs[k].bild_gang >= 1 && zs[k].bild_gang <= R37_BILD_GANG) n_gang++;
            if (zs[k].bild_gang > bild_max) bild_max = zs[k].bild_gang;
            int32_t weg = zs[k].d0 - zs[k].d300;
            if (weg < min_weg) min_weg = weg;
            if (weg >= R37_BILDER_RATE * R37_RATE_MIN) n_nah++;
            if (zs[k].nie_gang) n_nie++;
        }
        if (an) {
            CHECK(nz == 5, "Nachspawn 1130 (Programm 10): 5 Montage-Zombies im Raum (%d)", nz);
            CHECK(n_gang == nz, "alle %d spaetestens Bild %d im Gang (RE2-Original Takt 2): %d (spaetestes Bild %d)", nz, R37_BILD_GANG, n_gang, bild_max);
            CHECK(n_nah == nz, "alle %d nach %d Bildern mindestens %d naeher an Leon (RE2-Original >= 15,4 je Takt): %d (kleinster Weg %d)",
                  nz, R37_BILDER_RATE, R37_BILDER_RATE * R37_RATE_MIN, n_nah, (int)min_weg);
        } else {
            CHECK(nz == 5 && n_nie == nz, "Gegenprobe v0.8.24: von %d Montage-Zombies erreicht in 600 Bildern KEINER den Gang (%d nie) = Befund "
                  "\"bleiben stumpf im Gang stehen\"", nz, n_nie);
        }
    }
    re15_r37_zombieaktiv_enable(1);
    return 1;
}

static int teil_raum1180(void)
{
    static zl_t zs[RE15_ACTOR_MAX]; int nz = 0;
    /* Leon = Spawn der Tuer ROOM1160 -> ROOM1180 (tools/engine_tueren.txt `1160 1180 -300 -16500 -4729 -16018 0`),
     * Normalspiel-Flags (Spielbeginn). ROOM1180 main00 spawnt drei Zombies mit grid 0x00 (RE1.5-Zensus, H_belege). */
    for (int an = 1; an >= 0; an--) {
        re15_game_state_init();
        re15_irons_tod_zustand_setzen(RE15_IT_AUS);
        re15_r37_zombieaktiv_enable(an);
        printf("%s ROOM1180, Leon am Eintritt aus 1160 (-4729,-16018)
", an ? "MIT Runde 37:" : "GEGENPROBE v0.8.24:");
        if (!lauf(0x1180, 0, -4729, -16018, 0, 600, zs, &nz)) { CHECK(0, "ROOM1180 nicht ladbar"); return 0; }
        if (s_mess) continue;
        int n00 = 0, gang00 = 0, nah00 = 0, nie00 = 0;
        for (int k = 0; k < nz; k++) {
            if (zs[k].grid != 0x00) continue;
            n00++;
            if (zs[k].bild_gang >= 1 && zs[k].bild_gang <= R37_BILD_GANG) gang00++;
            if (zs[k].d0 - zs[k].d300 >= R37_BILDER_RATE * R37_RATE_MIN || zs[k].d300 < 1500) nah00++;
            if (zs[k].nie_gang) nie00++;
        }
        if (an) {
            CHECK(n00 == 3, "ROOM1180: %d Zombies mit grid 0x00 (main00, Normalspiel)", n00);
            CHECK(gang00 == n00, "ROOM1180: alle %d grid-0x00-Zombies spaetestens Bild %d im Gang: %d", n00, R37_BILD_GANG, gang00);
            CHECK(nah00 == n00, "ROOM1180: alle %d nach %d Bildern >= %d naeher an Leon (oder schon < 1500): %d", n00, R37_BILDER_RATE,
                  R37_BILDER_RATE * R37_RATE_MIN, nah00);
        } else {
            CHECK(n00 == 3 && gang00 == 0, "Gegenprobe v0.8.24 ROOM1180: keiner der %d grid-0x00-Zombies ist ab Bild <= %d im Gang (%d)", n00,
                  R37_BILD_GANG, gang00);
        }
    }
    re15_r37_zombieaktiv_enable(1);
    return 1;
}

int main(int argc, char **argv)
{
    int a = 1;
    if (argc > 1 && strcmp(argv[1], "mess") == 0) { s_mess = 1; a = 2; }
    const char *teil = (argc > a) ? argv[a] : "alle";
    int ok = 1;
    if (!strcmp(teil, "abbildung") || !strcmp(teil, "alle"))     ok &= teil_abbildung();
    if (!strcmp(teil, "nachspawn1130") || !strcmp(teil, "alle")) ok &= teil_nachspawn1130();
    if (!strcmp(teil, "raum1180") || !strcmp(teil, "alle"))      ok &= teil_raum1180();
    if (s_mess) return 0;
    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return (fails || !ok) ? 1 : 0;
}
