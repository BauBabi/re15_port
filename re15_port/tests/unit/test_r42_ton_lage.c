/* test_r42_ton_lage.c — Strang "ton", Stufe 6 (2026-11-20): LAGE DER GERAEUSCHE (Richtung und Entfernung).
 *
 * Nutzer 19.11.2026: "Bei der PC Version klingen die Sounds irgendwie dumpfer/anders als in der PSX Version."
 * Messbericht U6: im Original wandern die Schritte mit der Kamera zwischen links und rechts (Spitze L/R -11,9/-14,0 ->
 * -13,9/-12,2 dBFS), der Schuss hat 5731/4691; der Port spielte bis Stufe 5 links = rechts (PORT-WAHL (a) der Stufe 1:
 * Mittelwert) und nahm fuer jedes Geraeusch mit Lage den Spieler (PORT-WAHL (b)).
 *
 * Original-Rechnung FUN_80045a64 (re15_wassersprung_r41_lage, Bytes im Kopf dort; Tabellen-Riegel unit_r41_wasser20b0):
 * voll = DAT_800b2824 (`sh a0` @0x80045ce4), volr = DAT_800b2826 (`sh a1` @0x80045cd4), danach SsUtKeyOnV @0x80059e08-bc
 * und SpuVmKeyOnNow (links `sh a2` @0x80056058, rechts `sh a1` @0x80056070). Sender:
 *   FUN_800453d0 (Raum-Geraeusch) -> DAT_800ac784 + 0x34 (@0x800453d4, @0x800454e0-ec), die laufende Entity;
 *   FUN_80045024 (Bank-Geraeusch) -> param_2 (@0x800451c0-cc), beim Schuss die Weltlage des Effekt-Platzes
 *   (ESP-Routine 9, `lh v0,40/42/44(v1)` @0x80017664-7c, `jal 0x80045024` @0x80017684).
 *
 * Geprueft am echten audio_pc.c (RE15_AUDIO_CAP_SYNC, Regler 100):
 *   (1) Fresslaut ROOM1140 snd1 Satz 3 (Ton 4, Stimme 4 = SPU 20, Pitch 0x51c), Cut 0, Sender = Entity 4 (Typ 0x11,
 *       200/0/-19600). GEMESSEN im Original-Savestate (scratchpad ton_dumpf/orig/h_schuss1140/pre.sav, ton/s6/
 *       ss_lage_reg.py): DAT_800b2824/26 = 102/65, SPU-Stimme 20 Register 10567/4259. Der Spieler (93/103) waere falsch.
 *       Dazu die Spitzen am Mischer: links = Probe * 10567, rechts = Probe * 4259.
 *   (2) Schritt ROOM1130 snd0 Satz 29, Cut 9, Spieler -1614/0/17282: Lage 93/103 -> 3498/4275 (im Original gemessen),
 *       am Mischer links und rechts GETRENNT (bis Stufe 5: beide 3886).
 *   (3) Option Mono (DAT_800b2678 == 1, @0x80055fc0-e4): beide Seiten = die groessere -> Fresslaut 10567/10567.
 *   (4) Schuss ueber die ECHTE ESP-Routine 9 (CORE00.ESP Effekt 2 Strom 0 = Muendungsfeuer, Haken pc_bang): beim Knall
 *       ist param_2 gesetzt, die Lage ist FUN_80045a64 der Weltlage des Platzes (nicht die des Spielers), danach ist
 *       param_2 wieder frei; ein Spieler-Ruf (Messer Satz 5) nimmt danach wieder den Spieler.
 *   (5) Vorrang: ein gesetztes param_2 gilt auch fuer ein Raum-Geraeusch (FUN_80045024 Bank 3 aus den ESP-Routinen) vor
 *       der laufenden Entity.
 *
 * Runde 2 (Pruefbericht pruef_s6_r1, gelb: die uebrigen Wege mit Lage-Bit; Zensus re15_svm_vol.h (c)-(e)):
 *   (6) re15_enemy_ai_run_all setzt den Sender je laufender Entity und danach wieder den Spieler (FUN_8001a50c `sw v0,0(s2)`
 *       @0x8001a5ac je Entity, Spieler-FSM `sw s0` @0x80031c6c danach) - gemessen am Messhaken re15_svm_sender_haken.
 *   (7) Roys Anker ROOM1021 (elza_r36.c @0x020DE, elzatoene_r38.c @0x020B2): Sender = Roy (Work_set(2,1) davor, Slot 2),
 *       nicht der Spieler - FUN_80106c18 / FUN_801021f8 lesen DAT_800ac784.
 *   (8) SCD Se_on mit Lage-Bit ROOM3091 sub07 @0x01A62 `36 02 0d 01 00 00 46 a0 00 00 dc 05`: param_2 = 0 + Versatz NULL-
 *       ERWEITERT (`lhu` @0x800416ec ff.) = 41030/0/1500 (nicht -24506/0/1500); ein Se_on ohne Lage-Bit bleibt ohne Lage.
 *   (9) CORE-Ruf eines Spieler-Zustands (re15_player_stagger_cmd2 Clip 8 -> 0x04010001 @0x80035e80, a1 = 0x800aca5a + 46):
 *       Lage des Spielers, auch wenn gerade eine Entity laeuft; ein Menue-Ton 0x0405 (kein Lage-Bit) bleibt ohne Lage.
 *
 * GEGENPROBE (scratchpad ton/s6/gegen/gegen.sh): derselbe Quelltext mit dem audio_pc.c von bf6827892 (Stufe 5) faellt in
 * (1) Register und Mischer, (2) Mischer, (3), (4) Knall und (5) - 6 von 8 rot; gruen bleiben nur der Spieler-Ruf (4) und die
 * Register des Schritts (2), die Stufe 1 schon hatte. Auf r42/story 71caea668 / aaea5f8c2 gibt es re15_svm_vol.h nicht
 * (der Riegel baut dort nicht).
 */
#include "re15_audio.h"
#include "re15_engine.h"
#include "re15_vab.h"
#include "re15_spu_gauss.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_esp.h"
#include "re15_svm_vol.h"
#include "re15_wassersprung_r41.h"
#include "re15_elza_r36.h"
#include "re15_elzatoene_r38.h"
#include "re15_enemy_ai.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#define RATE      44100
#define PER_TICK  (RATE / 30)

/* wie test_r42_ton_mischer.c: das ECHTE audio_pc.c, diese Symbole kommen sonst aus main.c */
re15_engine_state_t g_engine;
void re15_debug_text(int x, int y, int z, const char *text) { (void)x; (void)y; (void)z; (void)text; }
unsigned char *re15_asset_read_file(const char *path, int *out_size)
{
    char buf[600];
    const char *pfx = "shared_assets/PSX/";
    if (strncmp(path, pfx, strlen(pfx)) == 0) snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path + strlen(pfx));
    else snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path);
    FILE *f = fopen(buf, "rb");
    if (!f) { snprintf(buf, sizeof buf, "%s", path); f = fopen(buf, "rb"); }
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    unsigned char *b = (unsigned char *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b && out_size) *out_size = (int)sz;
    return b;
}

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

/* Spitze der wie in der SPU geglaetteten Probe `vag0` bei der Tonhoehe des Tons `tone` (wie test_r42_ton_mischer.c). */
static int probe_spitze(const uint8_t *vh, size_t vh_n, const uint8_t *vb, size_t vb_n, int vag0, int tone, long n_aus)
{
    static re15_vab_t v;
    if (re15_vab_parse(vh, vh_n, &v) != 0 || vag0 >= v.vag_count) return -1;
    const uint32_t off = v.samples[vag0].offset, sz = v.samples[vag0].size;
    if (off + sz > vb_n) return -1;
    const re15_vab_tone_t *t = &v.tones[tone];
    const uint32_t pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift, t->center_note, t->pitch_shift);
    const int schleife = re15_vag_loop_start(vb + off, sz);
    const size_t cap = (sz / 16) * 28;
    int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
    const int n = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    int pk = 0;
    re15_spu_zaehler_t z;
    re15_spu_zaehler_start(&z);
    for (long k = 0; k < n_aus && n > 0; k++) {
        const int32_t g = re15_spu_gauss_wert(&z, pcm);
        const int a = g < 0 ? -g : g;
        if (a > pk) pk = a;
        if (!re15_spu_zaehler_weiter(&z, pcm, n, schleife, pitch)) break;
    }
    free(pcm);
    return pk;
}

/* Soll-Spitze: ((Probe * Register*2) >> 15) * 0x7ffe >> 15 (Mischer, Hauptlautstaerke 0x3fff). */
static int soll_spitze(int probe, int reg) { int v = (probe * (reg * 2)) >> 15; return (v * 0x7ffe) >> 15; }
static int nah(int ist, int soll) { const int d = ist > soll ? ist - soll : soll - ist; return d <= 2 + soll / 200; }

static uint8_t *raum_laden(const char *rel, int cut, int32_t x, int32_t y, int32_t z, int *n_out)
{
    int n = 0;
    uint8_t *d = re15_asset_read_file(rel, &n);
    if (!d || re15_rdt_parse(d, (size_t)n, &g_room_rdt) != 0) { printf("FAIL: %s nicht ladbar\n", rel); fails++; return NULL; }
    g_room_rdt_ok = 1;
    g_scd.work_vars[0x0A] = (int16_t)cut;               /* gezeigter Cut = DAT_800b0fe4 */
    memset(&g_actors[0], 0, sizeof g_actors[0]);
    g_actors[0].active = 1; g_actors[0].x = x; g_actors[0].y = y; g_actors[0].z = z;
    re15_audio_load_room_banks();
    if (n_out) *n_out = n;
    return d;
}

static const char *s_cap = "r42_ton_lage.pcm";
static long s_takt = 0;
static long laufen(int ticks) { const long t0 = s_takt; for (int i = 0; i < ticks; i++) re15_audio_tick(); s_takt += ticks; return t0; }
static int16_t *s_pcm = NULL; static long s_pcm_n = 0;
static void spitzen(long takt0, int ticks, int *pl, int *pr)
{
    *pl = *pr = 0;
    const long a = takt0 * PER_TICK, n = (long)ticks * PER_TICK;
    if (!s_pcm || a + n > s_pcm_n) { printf("FAIL: Mitschnitt zu kurz\n"); fails++; return; }
    for (long i = a; i < a + n; i++) {
        const int l = abs(s_pcm[i * 2]), r = abs(s_pcm[i * 2 + 1]);
        if (l > *pl) *pl = l;
        if (r > *pr) *pr = r;
    }
}

/* (4): der Knall-Haken des Ports, umwickelt - merkt param_2 und die Weltlage des Muendungsplatzes beim Knall. */
static void (*s_pc_bang)(void) = NULL;
static int s_bang_n = 0, s_bang_param = 0, s_bang_wpos[3], s_bang_lage[2] = { -1, -1 };
static void bang_messen(void)
{
    s_bang_n++;
    s_bang_param = re15_svm_sender_lage_gesetzt();
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->effect_id == 2 && f->row[0] == 9 && f->row[1] == 0) {   /* der Platz, dessen Routine A gerade 9 ist */
            s_bang_wpos[0] = f->wpos[0]; s_bang_wpos[1] = f->wpos[1]; s_bang_wpos[2] = f->wpos[2];
        }
    }
    if (s_pc_bang) s_pc_bang();
    int reg[2];
    (void)re15_audio_se_mess(reg, s_bang_lage);
}

/* (6): Folge der gesetzten Sender waehrend re15_enemy_ai_run_all. */
static int s_haken_folge[16], s_haken_n = 0;
static void haken_merken(int slot) { if (s_haken_n < 16) s_haken_folge[s_haken_n++] = slot; }

static int lage_von(int32_t x, int32_t y, int32_t z, int l[2])
{
    const int32_t p[3] = { x, y, z };
    return re15_wassersprung_r41_lage(&g_room_rdt, p, l, 0);
}

int main(void)
{
    static char capenv[1024];
    remove(s_cap);
    snprintf(capenv, sizeof capenv, "RE15_AUDIO_CAP_SYNC=%s", s_cap);
    putenv(capenv);
    static char noint[] = "RE15_NO_INTRO=1";
    putenv(noint);
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("== Strang ton Stufe 6: Lage der Geraeusche (FUN_80045a64, Sender wie im Original) ==\n");
    re15_audio_init();

    enum { N_FRESS = 20, N_SCHRITT = 40 };
    long t_fress = 0, t_schritt = 0;
    int p_fress = -1, p_schritt = -1;
    int n = 0;

    /* ---- (1) Fresslaut, Sender Entity 4 --------------------------------------------------------------------- */
    uint8_t *rdt2 = raum_laden("STAGE1/ROOM1140.RDT", 0, -5118, 0, -15817, &n);
    if (rdt2) {
        const uint8_t *vh = rdt2 + rd32(rdt2 + 0x18), *vb = rdt2 + rd32(rdt2 + 0x1c);
        const size_t vh_n = rd32(rdt2 + 0x1c) - rd32(rdt2 + 0x18);
        {   static re15_vab_t v; re15_vab_parse(vh, vh_n, &v);   /* Ton 4 -> VAG (vag_index 1-basiert) */
            p_fress = probe_spitze(vh, vh_n, vb, (size_t)n - rd32(rdt2 + 0x1c), (int)v.tones[4].vag_index - 1, 4, (long)N_FRESS * PER_TICK); }
        memset(&g_actors[4], 0, sizeof g_actors[4]);
        g_actors[4].active = 1; g_actors[4].type = 0x11; g_actors[4].x = 200; g_actors[4].y = 0; g_actors[4].z = -19600;
        int reg[2], lage[2], ls[2];
        (void)re15_svm_lage_spieler(ls);
        re15_svm_sender_entity(&g_actors[4]);                    /* wie re15_enemy_ai_run_all fuer Platz 4 */
        (void)laufen(2);
        re15_audio_room_se(3);
        re15_svm_sender_entity(NULL);
        t_fress = laufen(N_FRESS);
        int da = re15_audio_se_mess(reg, lage);
        CHECK(da && lage[0] == 102 && lage[1] == 65 && reg[0] == 10567 && reg[1] == 4259,
              "(1) Fresslaut ROOM1140 Satz 3, Sender Entity 4 (200/0/-19600): Lage %d/%d (Original DAT_800b2824/26 102/65), "
              "Register %d/%d (Original SPU-Stimme 20: 10567/4259); Lage des Spielers waere %d/%d",
              lage[0], lage[1], reg[0], reg[1], ls[0], ls[1]);

        /* ---- (3) Mono ------------------------------------------------------------------------------------- */
        re15_audio_set_mono(1);
        re15_svm_sender_entity(&g_actors[4]);
        re15_audio_room_se(3);
        re15_svm_sender_entity(NULL);
        (void)laufen(2);
        da = re15_audio_se_mess(reg, lage);
        CHECK(da && reg[0] == 10567 && reg[1] == 10567,
              "(3) Option Mono: Register %d/%d, Soll 10567/10567 (beide = die groessere, SpuVmKeyOnNow @0x80055fd0-e4)", reg[0], reg[1]);
        re15_audio_set_mono(0);

        /* ---- (5) param_2 vor der laufenden Entity ---------------------------------------------------------- */
        {   const int32_t p[3] = { -1800, 0, -21600 };           /* Lage der Entity 2 im Savestate (99/70), weder Spieler noch Entity 4 */
            int lp[2];
            (void)re15_wassersprung_r41_lage(&g_room_rdt, p, lp, 0);
            re15_svm_sender_entity(&g_actors[4]);
            re15_svm_sender_lage(p);
            re15_audio_room_se(3);
            re15_svm_sender_lage(NULL);
            re15_svm_sender_entity(NULL);
            (void)laufen(2);
            da = re15_audio_se_mess(reg, lage);
            CHECK(da && lage[0] == lp[0] && lage[1] == lp[1] && !(lage[0] == 102 && lage[1] == 65) && !(lage[0] == 93 && lage[1] == 103),
                  "(5) param_2 gesetzt + Entity 4 laufend: Lage %d/%d, Soll die von param_2 %d/%d (FUN_80045024 waehlt die Lage "
                  "aus param_2, @0x800451c0-cc)", lage[0], lage[1], lp[0], lp[1]);
        }

        /* ---- (4) Schuss ueber die echte ESP-Routine 9 -------------------------------------------------------- */
        int csz = 0;
        uint8_t *core = re15_asset_read_file("DATA/CORE00.ESP", &csz);
        static re15_esp_t bank;
        if (core && re15_esp_parse_global(core, (size_t)csz, &bank) == 0) {
            re15_esp_set_global_bank(&bank);
            re15_esp_fx_reset();
            re15_audio_prime_weapon(3);                          /* ARMS03 */
            s_pc_bang = re15_esp_bang_hook;                      /* pc_bang aus re15_audio_init */
            re15_esp_bang_hook = bang_messen;
            /* Muendungsfeuer wie die Pistole (FUN_80019700(0x02000800, ..) @0x800337bc: Effekt 2, Strom 0, Groesse 0x800),
             * hier an einer Stelle weit weg vom Spieler, damit Spieler- und Platz-Lage sicher verschieden sind. */
            const int ns = re15_esp_fx_spawn_rows(&bank, 2, 0, 0x0800, 200, -1500, -19600, 0, 0);
            for (int t = 0; t < 10 && s_bang_n == 0; t++) { re15_esp_fx_tick(&bank); (void)laufen(1); }
            re15_esp_bang_hook = s_pc_bang;
            int lw[2] = { -1, -1 }, ls[2] = { -1, -1 };
            const int32_t w[3] = { s_bang_wpos[0], s_bang_wpos[1], s_bang_wpos[2] };
            (void)re15_wassersprung_r41_lage(&g_room_rdt, w, lw, 0);
            (void)re15_svm_lage_spieler(ls);
            CHECK(ns >= 1 && s_bang_n == 1 && s_bang_param == 1 && s_bang_lage[0] == lw[0] && s_bang_lage[1] == lw[1]
                  && !(lw[0] == ls[0] && lw[1] == ls[1]) && !re15_svm_sender_lage_gesetzt(),
                  "(4) Schuss ueber ESP-Routine 9: %d Knall, param_2 beim Knall %d, Weltlage des Platzes %d/%d/%d -> Lage %d/%d, "
                  "gespielt %d/%d (Spieler waere %d/%d); danach param_2 frei %d",
                  s_bang_n, s_bang_param, w[0], w[1], w[2], lw[0], lw[1], s_bang_lage[0], s_bang_lage[1], ls[0], ls[1],
                  !re15_svm_sender_lage_gesetzt());
            (void)laufen(30);                                    /* Schuss ausklingen lassen (Prioritaets-Gate FUN_80045a18) */
            re15_audio_weapon_se(1);                             /* Leer-Klick 0x01010001 = Spieler-Ruf (@0x8003338c) */
            int reg5[2], lage5[2];
            da = re15_audio_se_mess(reg5, lage5);
            CHECK(da && lage5[0] == ls[0] && lage5[1] == ls[1],
                  "(4) danach ein Spieler-Ruf (ARMS03 Satz 1, Leer-Klick): Lage %d/%d = Spieler %d/%d", lage5[0], lage5[1], ls[0], ls[1]);
        } else { printf("FAIL: CORE00.ESP nicht lesbar\n"); fails++; }
        free(core);
    }

    /* ---- (2) Schritt ROOM1130 Cut 9 ----------------------------------------------------------------------------- */
    uint8_t *rdt = raum_laden("STAGE1/ROOM1130.RDT", 9, -1614, 0, 17282, &n);
    if (rdt) {
        const uint8_t *vh = rdt + rd32(rdt + 0x0c), *vb = rdt + rd32(rdt + 0x10);
        p_schritt = probe_spitze(vh, rd32(rdt + 0x10) - rd32(rdt + 0x0c), vb, (size_t)n - rd32(rdt + 0x10), 1, 1, (long)N_SCHRITT * PER_TICK);
        (void)laufen(4);
        re15_audio_footstep(7, 29);
        t_schritt = laufen(N_SCHRITT);
        int reg[2], lage[2];
        CHECK(re15_audio_se_mess(reg, lage) && lage[0] == 93 && lage[1] == 103 && reg[0] == 3498 && reg[1] == 4275,
              "(2) Schritt ROOM1130 Cut 9 Spieler -1614/0/17282: Lage %d/%d (93/103), Register %d/%d (3498/4275, Original)",
              lage[0], lage[1], reg[0], reg[1]);
    }

    /* ---- (6) re15_enemy_ai_run_all: Sender je Entity, danach der Spieler ------------------------------------------ */
    if (rdt) {
        for (int i = 1; i < RE15_ACTOR_MAX; i++) g_actors[i].active = 0;
        memset(&g_actors[2], 0, sizeof g_actors[2]); memset(&g_actors[5], 0, sizeof g_actors[5]);
        g_actors[2].active = 1; g_actors[2].type = 0x22; g_actors[2].x = -1000; g_actors[2].z = 16000;   /* 0x22 = Stub-Typ ohne */
        g_actors[5].active = 1; g_actors[5].type = 0x22; g_actors[5].x = -2000; g_actors[5].z = 18000;   /* Handler (run_all)    */
        re15_svm_sender_haken = haken_merken;
        s_haken_n = 0;
        re15_enemy_ai_run_all(0);
        re15_svm_sender_haken = NULL;
        CHECK(s_haken_n == 3 && s_haken_folge[0] == 2 && s_haken_folge[1] == 5
              && s_haken_folge[2] == -1 && re15_svm_sender_entity_slot() == -1,
              "(6) run_all mit Entities in Slot 2 und 5: Sender-Folge %d Eintraege [%d, %d, %d], Soll [2, 5, -1 = Spieler] "
              "(FUN_8001a50c @0x8001a5ac je Entity, danach Spieler-FSM @0x80031c6c)", s_haken_n,
              s_haken_n > 0 ? s_haken_folge[0] : 99, s_haken_n > 1 ? s_haken_folge[1] : 99, s_haken_n > 2 ? s_haken_folge[2] : 99);
        g_actors[2].active = 0; g_actors[5].active = 0;
    }

    /* ---- (7) Roys Anker ROOM1021: Sender = Roy (Arbeitsziel Slot 2) ------------------------------------------------- */
    int n3 = 0;
    uint8_t *rdt3 = raum_laden("STAGE1/ROOM1021.RDT", 0, -4274, 0, -6590, &n3);
    if (rdt3) {
        memset(&g_actors[2], 0, sizeof g_actors[2]);
        g_actors[2].active = 1; g_actors[2].type = 0x10; g_actors[2].x = -9000; g_actors[2].y = 0; g_actors[2].z = -2000;
        int lr[2] = { -1, -1 }, lsp[2] = { -1, -1 }, reg[2], lage[2];
        (void)lage_von(g_actors[2].x, g_actors[2].y, g_actors[2].z, lr);
        (void)re15_svm_lage_spieler(lsp);
        g_scd.work_slot = 2;                                       /* Work_set(2,1) `2e 02 01` @0x020DA bzw. @0x020AE */
        re15_elza_r36_raum_scan(rdt3, n3, RE15_ELZA_RAUM_1021);
        re15_elzatoene_r38_raum_scan(rdt3, n3, RE15_ELZATOENE_RAUM_1021);
        (void)laufen(30);
        re15_elza_r36_pc(rdt3 + 0x020DE);                          /* Roys Tod: snd1 SE 5, Verzug 1 (Anker-Tafel elza_r36.c) */
        g_scd.work_slot = -1;                                      /* ein anderer Faden aendert das Arbeitsziel vor dem Ausfuehren */
        re15_elza_r36_tick();                                      /* naechster VM-Takt: die verzoegerte Aktion */
        (void)laufen(1);
        int da = re15_audio_se_mess(reg, lage);
        CHECK(g_re15_elza_anker_n > 0 && da && lage[0] == lr[0] && lage[1] == lr[1] && !(lr[0] == lsp[0] && lr[1] == lsp[1])
              && re15_svm_sender_entity_slot() == -1,
              "(7) Roys Tod ROOM1021 @0x020DE: Lage %d/%d, Soll Roy (Slot 2, -9000/0/-2000) %d/%d, Spieler waere %d/%d; "
              "danach wieder Spieler-Sender %d", lage[0], lage[1], lr[0], lr[1], lsp[0], lsp[1], re15_svm_sender_entity_slot());
        (void)laufen(60);
        g_scd.work_slot = 2;                                       /* Work_set(2,1) @0x020AE direkt vor dem Anker */
        re15_elzatoene_r38_pc(rdt3 + 0x020B2);                     /* Roys Stoehnen: snd1 SE 4 */
        (void)laufen(1);
        da = re15_audio_se_mess(reg, lage);
        CHECK(g_re15_elzatoene_anker_n > 0 && da && lage[0] == lr[0] && lage[1] == lr[1],
              "(7) Roys Stoehnen ROOM1021 @0x020B2: Lage %d/%d, Soll Roy %d/%d (Spieler %d/%d)", lage[0], lage[1], lr[0], lr[1],
              lsp[0], lsp[1]);
        g_actors[2].active = 0;
        g_scd.work_slot = -1;
    }

    /* ---- (8) SCD Se_on mit Lage-Bit ROOM3091 -------------------------------------------------------------------- */
    int n4 = 0;
    uint8_t *rdt4 = raum_laden("STAGE3/ROOM3091.RDT", 0, -24000, 0, -6000, &n4);
    if (rdt4) {
        static const uint8_t op[12] = { 0x36, 0x02, 0x0d, 0x01, 0x00, 0x00, 0x46, 0xa0, 0x00, 0x00, 0xdc, 0x05 };
        CHECK(n4 > 0x01A62 + 12 && memcmp(rdt4 + 0x01A62, op, 12) == 0, "(8) ROOM3091 @0x01A62 traegt den Se_on mit Lage-Bit");
        int lu[2] = { -1, -1 }, ls16[2] = { -1, -1 }, lsp[2] = { -1, -1 }, reg[2], lage[2];
        (void)lage_von(41030, 0, 1500, lu);                        /* 0xa046 null-erweitert */
        (void)lage_von(-24506, 0, 1500, ls16);                     /* 0xa046 als s16 - so rechnet das Original NICHT */
        (void)re15_svm_lage_spieler(lsp);
        (void)laufen(30);
        scd_audio_event_t ev;
        memset(&ev, 0, sizeof ev);
        ev.kind = (uint8_t)SCD_AUDIO_SE_ON; ev.bank = op[1]; ev.sample_id = op[2];
        ev.raw_w0 = (uint16_t)((op[3] << 8) | op[4]); ev.raw_w1 = op[5];   /* wie op_se_on (scd_vm.c) */
        ev.pos_x = (int16_t)(op[6] | (op[7] << 8)); ev.pos_y = (int16_t)(op[8] | (op[9] << 8)); ev.pos_z = (int16_t)(op[10] | (op[11] << 8));
        scd_audio_queue_push(&ev);
        (void)laufen(1);
        int da = re15_audio_se_mess(reg, lage);
        CHECK(da && lage[0] == lu[0] && lage[1] == lu[1] && !re15_svm_sender_lage_gesetzt(),
              "(8) Se_on Flags 1 Ursprung 0 Versatz a046/0000/05dc: Lage %d/%d, Soll FUN_80045a64(41030/0/1500) = %d/%d "
              "(als s16 -24506 waere es %d/%d, Spieler %d/%d), Register %d/%d", lage[0], lage[1], lu[0], lu[1], ls16[0], ls16[1],
              lsp[0], lsp[1], reg[0], reg[1]);
        (void)laufen(60);
        ev.raw_w0 = 0;                                             /* derselbe Se_on OHNE Lage-Bit */
        scd_audio_queue_push(&ev);
        (void)laufen(1);
        da = re15_audio_se_mess(reg, lage);
        CHECK(da && lage[0] == -1 && lage[1] == -1,
              "(8) Se_on ohne Lage-Bit: ohne Lage (Messhaken %d/%d = -1/-1), Register %d/%d", lage[0], lage[1], reg[0], reg[1]);
    }

    /* ---- (9) CORE-Ruf eines Spieler-Zustands: Lage des Spielers --------------------------------------------------- */
    {   uint8_t *rdt5 = raum_laden("STAGE1/ROOM1140.RDT", 0, -5118, 0, -15817, NULL);
        if (rdt5) {
            memset(&g_actors[4], 0, sizeof g_actors[4]);
            g_actors[4].active = 1; g_actors[4].type = 0x11; g_actors[4].x = 200; g_actors[4].y = 0; g_actors[4].z = -19600;
            int lsp[2] = { -1, -1 }, le[2] = { -1, -1 }, reg[2], lage[2];
            (void)re15_svm_lage_spieler(lsp);
            (void)lage_von(200, 0, -19600, le);
            (void)laufen(60);
            re15_svm_sender_entity(&g_actors[4]);                 /* eine Entity laeuft gerade (Port-Kontext des Opfer-Einstiegs) */
            re15_player_stagger_cmd2(0x08);                        /* 0x04010001 @0x80035e80, a1 = &DAT_800aca88 */
            re15_svm_sender_entity(NULL);
            (void)laufen(1);
            int da = re15_audio_se_mess(reg, lage);
            CHECK(da && lage[0] == lsp[0] && lage[1] == lsp[1] && !(le[0] == lsp[0] && le[1] == lsp[1]) && !re15_svm_sender_lage_gesetzt(),
                  "(9) CORE 1 aus re15_player_stagger_cmd2 (0x04010001 @0x80035e80): Lage %d/%d = Spieler %d/%d (Entity 4 waere %d/%d), "
                  "Register %d/%d", lage[0], lage[1], lsp[0], lsp[1], le[0], le[1], reg[0], reg[1]);
            (void)laufen(60);
            re15_audio_core_se(5);                                 /* Menue 0x04050000, a1 = 0 @0x8004a664: kein Lage-Bit */
            (void)laufen(1);
            da = re15_audio_se_mess(reg, lage);
            CHECK(da && lage[0] == -1 && lage[1] == -1, "(9) Menue-Ton CORE 5 ohne Lage-Bit: Messhaken %d/%d = -1/-1", lage[0], lage[1]);
            g_actors[4].active = 0;
            free(rdt5);
        }
    }
    re15_audio_shutdown();

    {   FILE *f = fopen(s_cap, "rb");
        if (f) {
            fseek(f, 0, SEEK_END); const long sz = ftell(f); fseek(f, 0, SEEK_SET);
            s_pcm = (int16_t *)malloc((size_t)sz);
            if (s_pcm && fread(s_pcm, 1, (size_t)sz, f) == (size_t)sz) s_pcm_n = sz / 4;
            fclose(f);
        } }
    if (rdt2 && p_fress > 0) {
        int pl, pr; spitzen(t_fress, N_FRESS, &pl, &pr);
        const int sl = soll_spitze(p_fress, 10567), sr = soll_spitze(p_fress, 4259);
        CHECK(nah(pl, sl) && nah(pr, sr),
              "(1) Fresslaut am Mischer: Spitze L/R %d/%d, Soll %d/%d = Probe %d * Register 10567 / 4259", pl, pr, sl, sr, p_fress);
    }
    if (rdt && p_schritt > 0) {
        int pl, pr; spitzen(t_schritt, N_SCHRITT, &pl, &pr);
        const int sl = soll_spitze(p_schritt, 3498), sr = soll_spitze(p_schritt, 4275);
        CHECK(nah(pl, sl) && nah(pr, sr),
              "(2) Schritt am Mischer: Spitze L/R %d/%d, Soll %d/%d = Probe %d * Register 3498 / 4275 (bis Stufe 5: beide %d)",
              pl, pr, sl, sr, p_schritt, soll_spitze(p_schritt, (3498 + 4275) / 2));
    }
    free(rdt); free(rdt2); free(rdt3); free(rdt4); free(s_pcm);
    remove(s_cap);
    printf("%s: %d von %d Pruefungen\n", fails ? "ROT" : "GRUEN", checks - fails, checks);
    return fails ? 1 : 0;
}
