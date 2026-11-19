/* test_r42_ton_mischer.c — Strang "ton", Stufe 1 (2026-11-19): die Pegel am ECHTEN Mischer des PC-Ports.
 *
 * Nutzer 19.11.2026: "Bei der PC Version klingen die Sounds irgendwie dumpfer/anders als in der PSX Version",
 * "Zum Beispiel beim öffnen des Rolltors im Flur", "Der Helikopter Rotos sound klingt auch leicht anders."
 * Messbericht U1 (Original gegen Port, Regler 100): Rolltor-Rumpeln -11,4 dB, Rolltor-Stopp -8,1 dB, Rotor -8,7 dB,
 * Pistolenschuss -4 bis -6 dB, Schritte -3,7 dB — weil der Port linear rechnete und pauschal abzog.
 *
 * Der Riegel linkt das echte audio_pc.c, rendert deterministisch (RE15_AUDIO_CAP_SYNC: 1470 Stereo-Bilder je
 * re15_audio_tick, kein Tongeraet, Regler 100) und vergleicht die SPITZE jedes Geraeuschs mit
 *     Spitze der dekodierten Tonprobe * SPU-Register * 2 / 0x8000 * Hauptlautstaerke 0x7ffe / 0x8000,
 * das Register aus der belegten Rechnung SpuVmKeyOnNow @0x80055d10-0x80056070 (re15_svm_vol.h) bzw. so, wie es im
 * laufenden Original gemessen wurde:
 *   (1) Rolltor-Rumpeln ROOM1130 snd0 Satz 10: 16383/16383      (alt: Q15 8192 = Register 4096 -> -12,0 dB)
 *   (2) Rolltor-Stopp   ROOM1130 snd0 Satz 11: 10268/10268      (alt: Q15 7288 -> -9,0 dB)
 *   (3) Pistolenschuss  ARMS03 Satz 0, zwei Schichten (ROOM1140, Cut 0, Spieler -5541/0/-16024). Original: Lage 106/95
 *       -> 5731/4691 je Schicht; der Sender ist dort NICHT der Spieler, sondern die Lage im Angriffs-Arbeitssatz
 *       (`lw v1,0x800b52c4` / `lh v0,40(v1)` .. @0x80017654-88). Port bis Stufe 6 (PORT-WAHL: Lage des Spielers 92/102,
 *       Summe): 4343/5306 -> 4824/4824, das sind -0,67 dB gegen die Original-Summe 5211      (alt: Q15 5805 -> -5,1 dB)
 *   (4) Schritt ROOM1130 snd0 Satz 29 (Ton 1, vol 80), Lage 93/103 (Cut 9, Spieler -1614/0/17282):
 *       Original 3498/4275; Port bis Stufe 6: 3886/3886                                 (alt: Q15 5160 -> -3,6 dB)
 *   (5) Rotor SUB_15 (ROOM1170, nur der SUB-Slot aufgedreht): Stimmen-Register 6500/6500. Original-Aufnahme "nur
 *       Rotor": -22,3 dBFS (Messbericht Abschnitt 3); Port alt -31,0 dBFS. Der Port legt bis Stufe 3 noch Hall auf den
 *       Rotor (+1,0 dB) und zeichnet bis Stufe 5 ohne die Gauss-Glaettung des Tonchips -> Band -24,3 .. -19,8 dBFS.
 *
 *   (6) Runde 2 - MUSIK UNTER GESPROCHENEN SAETZEN (PORT-WAHL nach dem NUTZERWUNSCH 18.10.2026 "er Port kann die Stimme
 *       ruhig lauter anheben das es passt"; Herleitung im Kopf ueber STIMME_MUSIK_SEQ_VOL in audio_pc.c): solange ein
 *       gesprochener Satz auf dem Sprachkanal laeuft, wird die Musik-Summe gerechnet, als stuende die Sequenz-Lautstaerke
 *       auf 80 statt 127 -> (80/127)^2 = 13002/32768 = -8,03 dB (Sequenz-Lautstaerke vor dem Quadrat, SpuVmKeyOnNow
 *       @0x80055e08-64 / @0x80055fe8); abwaerts in einem Spielbild, aufwaerts 1/0x3c des vollen Pegels je Spielbild
 *       (FUN_800449f4(0x3c)).
 *       Geprueft: (6a) ein Geraeusch auf dem Sprachkanal laesst die Musik stehen; (6b) nach einem Spielbild steht der
 *       Faktor auf 13002 und bleibt dort; (6c) der Rotor liegt im Mitschnitt 8,03 dB (+-1,0) tiefer; (6d) ein Schuss
 *       waehrend des Satzes hat dieselbe Spitze wie in (3) - Geraeusche bleiben; (6e) nach dem Satz bleibt die Musik
 *       noch 0x3c Spielbilder unten (Pausen zwischen den Saetzen eines Gespraechs: gemessen 1 bis 40 Bilder) und kommt
 *       dann mit derselben Steigung zurueck (21193 nach 15 Bildern, 32768 nach 37).
 *
 * GEGENPROBE: derselbe Quelltext mit dem audio_pc.c von r42/story 71caea668 gebaut (ohne -DRE15_TON_S1_HAKEN, der
 * Messhaken re15_audio_se_mess existiert dort nicht) faellt in (1) bis (5). (6) haengt an den Messhaken der Runde 2;
 * Gegenprobe dafuer: audio_pc.c mit stillgelegter Absenkung (Ziel immer 1,0) -> (6b) bis (6e) fallen.
 */
#include "re15_audio.h"
#include "re15_engine.h"
#include "re15_vab.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_scd.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#define RATE      44100
#define PER_TICK  (RATE / 30)

/* wie test_rotor_bgm_pin.c: das ECHTE audio_pc.c, diese Symbole kommen sonst aus main.c */
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
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static re15_vab_t s_vab;

/* Spitze (Betrag) der dekodierten Tonprobe `vag0` (ab 0) einer Bank VH/VB. */
static int probe_spitze(const uint8_t *vh, size_t vh_n, const uint8_t *vb, size_t vb_n, int vag0)
{
    if (re15_vab_parse(vh, vh_n, &s_vab) != 0 || vag0 >= s_vab.vag_count) return -1;
    const uint32_t off = s_vab.samples[vag0].offset, sz = s_vab.samples[vag0].size;
    if (off + sz > vb_n) return -1;
    const size_t cap = (sz / 16) * 28;
    int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
    const int n = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    int pk = 0;
    for (int i = 0; i < n; i++) { const int a = pcm[i] < 0 ? -pcm[i] : pcm[i]; if (a > pk) pk = a; }
    free(pcm);
    return pk;
}

/* Soll-Spitze einer Stimme: ((Probe * Register*2) >> 15) * 0x7ffe >> 15 — die Rechnung des Mischers, mit dem
 * Register des Originals. `mal` Schichten derselben Probe liegen deckungsgleich uebereinander. */
static int soll_spitze(int probe, int reg, int mal)
{
    int v = (probe * (reg * 2)) >> 15;
    v = (v * 0x7ffe) >> 15;
    return v * mal;
}

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

typedef struct { int pk_l, pk_r; double rms; } mess_t;
static const char *s_cap = "r42_ton_mischer.pcm";
static long s_takt = 0;                                   /* bisher gerenderte Spieltakte */

/* `ticks` Spieltakte rendern; Rueckgabe = erster Takt des Stuecks (der Mitschnitt wird erst beim Schliessen
 * vollstaendig geschrieben -> vermessen wird nach re15_audio_shutdown mit messen()). */
static long laufen(int ticks)
{
    const long t0 = s_takt;
    for (int i = 0; i < ticks; i++) re15_audio_tick();
    s_takt += ticks;
    return t0;
}

static int16_t *s_pcm = NULL; static long s_pcm_n = 0;    /* ganzer Mitschnitt, Stereo-Bilder */
static mess_t messen(long takt0, int ticks)
{
    mess_t m = { 0, 0, 0.0 };
    const long a = takt0 * PER_TICK, n = (long)ticks * PER_TICK;
    if (!s_pcm || a + n > s_pcm_n) { printf("FAIL: Mitschnitt zu kurz (%ld Bilder, gebraucht %ld)\n", s_pcm_n, a + n); fails++; return m; }
    double acc = 0.0;
    for (long i = a; i < a + n; i++) {
        const int l = s_pcm[i * 2], r = s_pcm[i * 2 + 1];
        if (abs(l) > m.pk_l) m.pk_l = abs(l);
        if (abs(r) > m.pk_r) m.pk_r = abs(r);
        acc += ((double)l * l + (double)r * r) / 2.0;
    }
    m.rms = sqrt(acc / (double)n);
    return m;
}

static int nah(int ist, int soll) { const int d = ist > soll ? ist - soll : soll - ist; return d <= 2 + soll / 200; }   /* 0,5 % + Rundung */

int main(void)
{
    static char capenv[1024];
    remove(s_cap);
    snprintf(capenv, sizeof capenv, "RE15_AUDIO_CAP_SYNC=%s", s_cap);
    putenv(capenv);
    static char noint[] = "RE15_NO_INTRO=1";
    putenv(noint);
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("== Strang ton Stufe 1: Pegel am Mischer gegen die SPU-Register des Originals ==\n");
    re15_audio_init();

    int p_rumpeln = -1, p_stopp = -1, p_schritt = -1, p_schuss = -1;
    long t_rumpeln = 0, t_stopp = 0, t_schritt = 0, t_schuss = 0, t_rotor = 0;
    enum { N_RUMPELN = 75, N_STOPP = 60, N_SCHRITT = 40, N_SCHUSS = 30, N_ROTOR = 300 };
    long t_vor = 0, t_unter = 0, t_schuss2 = 0;                  /* (6) Musik unter gesprochenen Saetzen */
    enum { N_FENSTER = 40, N_SATZ = 200 };
    int16_t *stille = NULL;

    /* ---- ROOM1130: Rolltor (snd0 Satz 10 / 11) und Schritt (Satz 29) ------------------------------------------ */
    int n = 0;
    uint8_t *rdt = raum_laden("STAGE1/ROOM1130.RDT", 9, -1614, 0, 17282, &n);
    if (rdt) {
        const uint8_t *vh = rdt + rd32(rdt + 0x0c), *vb = rdt + rd32(rdt + 0x10);
        const size_t vh_n = rd32(rdt + 0x10) - rd32(rdt + 0x0c), vb_n = (size_t)n - rd32(rdt + 0x10);
        p_rumpeln = probe_spitze(vh, vh_n, vb, vb_n, 3);         /* Ton 7 -> VAG 4 */
        p_stopp   = probe_spitze(vh, vh_n, vb, vb_n, 4);         /* Ton 8 -> VAG 5 */
        p_schritt = probe_spitze(vh, vh_n, vb, vb_n, 1);         /* Ton 1 -> VAG 2 */
        (void)laufen(4);
        re15_audio_room_se_snd0(10);  t_rumpeln = laufen(N_RUMPELN);   /* 2,29 s Probe + Vormerk-Takt */
        re15_audio_room_se_snd0(11);  t_stopp   = laufen(N_STOPP);
        re15_audio_footstep(7, 29);   t_schritt = laufen(N_SCHRITT);
#ifdef RE15_TON_S1_HAKEN
        {   int reg[2], lage[2];
            CHECK(re15_audio_se_mess(reg, lage) && lage[0] == 93 && lage[1] == 103 && reg[0] == 3498 && reg[1] == 4275,
                  "(4) Schritt ROOM1130 Cut 9 Spieler -1614/0/17282: Lage %d/%d (im Original gemessen 93/103), Register %d/%d (3498/4275)",
                  lage[0], lage[1], reg[0], reg[1]); }
#endif
    }

    /* ---- ROOM1140: Pistolenschuss (ARMS03 Satz 0) ------------------------------------------------------------- */
    uint8_t *rdt2 = raum_laden("STAGE1/ROOM1140.RDT", 0, -5541, 0, -16024, &n);
    if (rdt2) {
        int en = 0, bn = 0;
        uint8_t *edh = re15_asset_read_file("SOUND/ARMS03.EDH", &en);
        uint8_t *vb  = re15_asset_read_file("SOUND/ARMS03.VB", &bn);
        if (edh && vb) {
            const uint32_t vho = rd32(edh + en - 8);
            p_schuss = probe_spitze(edh + vho, (size_t)en - 8 - vho, vb, (size_t)bn, 1);   /* Ton 1 und 2 -> VAG 2 */
            re15_audio_prime_weapon(3);                          /* ARMS03 */
            (void)laufen(2);
            re15_audio_weapon_se(0);  t_schuss = laufen(N_SCHUSS);
#ifdef RE15_TON_S1_HAKEN
            {   int reg[2], lage[2];
                CHECK(re15_audio_se_mess(reg, lage) && lage[0] == 92 && lage[1] == 102 && reg[0] == 4343 && reg[1] == 5306,
                      "(3) Schuss: Lage des Spielers %d/%d (92/102), Register %d/%d (4343/5306)", lage[0], lage[1], reg[0], reg[1]); }
#endif
        } else { printf("FAIL: ARMS03 nicht lesbar\n"); fails++; }
        free(edh); free(vb);
    }

    /* ---- ROOM1170: Rotor = SUB_15, nur der SUB-Slot aufgedreht (MAIN32 bleibt per Flag stumm) ------------------ */
    re15_audio_start_room_bgm(0, 0x17);
    (void)laufen(30);
    re15_audio_seq_ctl(1, 1);                                    /* Sce_bgm_control(1,1) @0x80044e00-48 */
    (void)laufen(60);                                            /* Einschwingen (Anstieg der Huellkurve) */
    t_rotor = laufen(N_ROTOR);
#ifdef RE15_TON_S1_HAKEN
    /* ---- (6) Musik unter gesprochenen Saetzen: der Rotor laeuft weiter, der "Satz" ist eine stille Probe ------- */
    stille = (int16_t *)calloc((size_t)N_SATZ * PER_TICK, sizeof(int16_t));
    if (stille) {
        const int voll = 0x8000, ziel = (80 * 80 * 0x8000) / (127 * 127);                 /* 13002 */
        CHECK(ziel == 13002 && re15_audio_musik_stimme_q15() == voll,
              "(6) Ausgangslage: Faktor %d (Soll 32768), Ziel %d (Soll 13002)", re15_audio_musik_stimme_q15(), ziel);
        re15_audio_stimme_mess(stille, 20 * PER_TICK, 0);        /* (6a) Geraeusch auf dem Sprachkanal */
        (void)laufen(10);
        CHECK(re15_audio_musik_stimme_q15() == voll,
              "(6a) Geraeusch auf dem Sprachkanal (kein gesprochener Satz): Faktor %d, Soll 32768 - die Musik bleibt stehen",
              re15_audio_musik_stimme_q15());
        (void)laufen(12);
        t_vor = laufen(N_FENSTER);                               /* Bezug: Rotor ohne Satz */
        re15_audio_stimme_mess(stille, N_SATZ * PER_TICK, 1);    /* (6b) gesprochener Satz, 200 Spielbilder */
        (void)laufen(1);
        const int g1 = re15_audio_musik_stimme_q15();
        (void)laufen(59);
        const int g60 = re15_audio_musik_stimme_q15();
        CHECK(g1 == ziel && g60 == ziel,
              "(6b) unter dem Satz: Faktor nach 1 / 60 Spielbildern %d / %d, Soll 13002 / 13002 (abwaerts in einem Spielbild "
              "auf (80/127)^2)", g1, g60);
        t_unter = laufen(N_FENSTER);                             /* (6c) Rotor unter dem Satz */
        re15_audio_seq_ctl(1, 2);                                /* Rotor aus: der Schuss soll allein im Fenster stehen */
        (void)laufen(45);                                        /* Hall-Rest des Rotors */
        CHECK(re15_audio_musik_stimme_q15() == ziel, "(6d) Faktor vor dem Schuss %d, Soll 13002", re15_audio_musik_stimme_q15());
        re15_audio_weapon_se(0);  t_schuss2 = laufen(N_SCHUSS);  /* (6d) Geraeusch waehrend des Satzes */
        (void)laufen(N_SATZ - (60 + N_FENSTER + 45 + N_SCHUSS)); /* bis zum Ende des Satzes */
        CHECK(re15_audio_musik_stimme_q15() == ziel, "(6e) Faktor am Satzende %d, Soll 13002", re15_audio_musik_stimme_q15());
        (void)laufen(58);                                        /* Wartezeit 0x3c Spielbilder: die Musik bleibt unten */
        CHECK(re15_audio_musik_stimme_q15() == ziel, "(6e) Faktor 58 Spielbilder nach dem Satzende %d, Soll 13002 (Wartezeit 0x3c)",
              re15_audio_musik_stimme_q15());
        (void)laufen(2);
        (void)laufen(15);
        const int r15 = re15_audio_musik_stimme_q15();
        (void)laufen(22);
        const int r37 = re15_audio_musik_stimme_q15();
        CHECK(abs(r15 - 21193) <= 2 && r37 == voll,
              "(6e) nach Satz und Wartezeit: Faktor nach 15 / 37 Spielbildern %d / %d, Soll 21193 / 32768 (dieselbe Steigung zurueck)", r15, r37);
    }
#endif
    re15_audio_shutdown();                                       /* schliesst den Mitschnitt */

    {   FILE *f = fopen(s_cap, "rb");
        if (f) {
            fseek(f, 0, SEEK_END); const long sz = ftell(f); fseek(f, 0, SEEK_SET);
            s_pcm = (int16_t *)malloc((size_t)sz);
            if (s_pcm && fread(s_pcm, 1, (size_t)sz, f) == (size_t)sz) s_pcm_n = sz / 4;
            fclose(f);
        }
        CHECK(s_pcm_n >= s_takt * PER_TICK, "Mitschnitt %s: %ld Stereo-Bilder, erwartet %ld", s_cap, s_pcm_n, s_takt * PER_TICK); }

    if (rdt) {
        mess_t m = messen(t_rumpeln, N_RUMPELN);
        const int s1 = soll_spitze(p_rumpeln, 16383, 1);
        printf("  (1) Rolltor-Rumpeln: Probe %d, Mitschnitt L/R %d/%d, Soll %d (Register 16383)\n", p_rumpeln, m.pk_l, m.pk_r, s1);
        CHECK(p_rumpeln > 1000 && nah(m.pk_l, s1) && nah(m.pk_r, s1),
              "(1) Rolltor-Rumpeln ROOM1130 snd0 Satz 10: Spitze %d/%d, Soll %d = Probe %d * Register 16383 (SpuVmKeyOnNow @0x80055d10; "
              "im Original gemessen 16383/16383). 25 %% des Solls = alte Rechnung (Ton-vol * 0x4000 / 127) >> 1", m.pk_l, m.pk_r, s1, p_rumpeln);
        m = messen(t_stopp, N_STOPP);
        const int s2 = soll_spitze(p_stopp, 10268, 1);
        printf("  (2) Rolltor-Stopp:   Probe %d, Mitschnitt L/R %d/%d, Soll %d (Register 10268)\n", p_stopp, m.pk_l, m.pk_r, s2);
        CHECK(p_stopp > 1000 && nah(m.pk_l, s2) && nah(m.pk_r, s2),
              "(2) Rolltor-Stopp ROOM1130 snd0 Satz 11 (Ton-vol 113): Spitze %d/%d, Soll %d = Probe %d * Register 10268", m.pk_l, m.pk_r, s2, p_stopp);
        m = messen(t_schritt, N_SCHRITT);
        const int s4 = soll_spitze(p_schritt, (3498 + 4275) / 2, 1);
        printf("  (4) Schritt:         Probe %d, Mitschnitt L/R %d/%d, Soll %d (Register 3886 = Mittel aus 3498/4275)\n", p_schritt, m.pk_l, m.pk_r, s4);
        CHECK(p_schritt > 1000 && nah(m.pk_l, s4) && nah(m.pk_r, s4),
              "(4) Schritt ROOM1130 Satz 29 (Ton-vol 80), Lage 93/103 (Cut 9, Spieler -1614/0/17282; im Original gemessen): Spitze %d/%d, "
              "Soll %d = Probe %d * (3498 + 4275) / 2", m.pk_l, m.pk_r, s4, p_schritt);
    }
    if (rdt2 && p_schuss > 0) {
        mess_t m = messen(t_schuss, N_SCHUSS);
        const int s3 = soll_spitze(p_schuss, (4343 + 5306) / 2, 2);
        const int s3o = soll_spitze(p_schuss, (5731 + 4691) / 2, 2);
        printf("  (3) Pistolenschuss:  Probe %d, Mitschnitt L/R %d/%d, Soll %d (2 Schichten, Register 4824 = Mittel aus 4343/5306; "
               "Original-Summe 5211 -> %d, Port %.2f dB dazu)\n", p_schuss, m.pk_l, m.pk_r, s3, s3o, 20.0 * log10((double)(m.pk_l > 0 ? m.pk_l : 1) / (double)s3o));
        CHECK(p_schuss > 1000 && nah(m.pk_l, s3) && nah(m.pk_r, s3),
              "(3) Pistolenschuss ARMS03 Satz 0 (ROOM1140 Cut 0, Spieler -5541/0/-16024, Lage des Spielers 92/102): Spitze %d/%d, Soll %d = "
              "2 Schichten * Probe %d * (4343 + 5306) / 2", m.pk_l, m.pk_r, s3, p_schuss);
        CHECK(m.pk_l * 100 >= s3o * 90 && m.pk_l * 100 <= s3o * 110,
              "(3) Pistolenschuss gegen das Original: Spitze %d, Summe des Originals %d (5731/4691 je Schicht, gemessen) - Soll innerhalb 10 %%",
              m.pk_l, s3o);
    }
    {   mess_t m = messen(t_rotor, N_ROTOR);
        const double db = 20.0 * log10((m.rms > 0.0 ? m.rms : 1e-9) / 32768.0);
        printf("  (5) Rotor SUB_15:    RMS %.1f = %.2f dBFS (Original-Aufnahme -22,3 dBFS; Port vor Stufe 1 -31,0 dBFS)\n", m.rms, db);
        CHECK(db > -24.3 && db < -19.8,
              "(5) Rotor SUB_15: %.2f dBFS, Soll -24,3 .. -19,8 (Original-Aufnahme -22,3; Stimmen-Register 6500/6500 nach SpuVmKeyOnNow, "
              "Hauptlautstaerke 0x3fff statt SS_BASE_MVOL 0x1a00)", db);
    }
#ifdef RE15_TON_S1_HAKEN
    if (stille) {
        const mess_t a = messen(t_vor, N_FENSTER), b = messen(t_unter, N_FENSTER);
        const double db = 20.0 * log10((b.rms > 0.0 ? b.rms : 1e-9) / (a.rms > 0.0 ? a.rms : 1e-9));
        printf("  (6c) Rotor unter einem gesprochenen Satz: RMS %.1f gegen %.1f ohne Satz = %.2f dB (Soll -8,03)\n", b.rms, a.rms, db);
        CHECK(a.rms > 500.0 && db > -9.03 && db < -7.03,
              "(6c) Musik unter einem gesprochenen Satz: %.2f dB gegen den Pegel davor (RMS %.1f / %.1f), Soll -8,03 +- 1,0 = "
              "(80/127)^2, Sequenz-Lautstaerke 80 vor dem Quadrat", db, b.rms, a.rms);
        if (rdt2 && p_schuss > 0) {
            const mess_t m = messen(t_schuss2, N_SCHUSS);
            const int s3 = soll_spitze(p_schuss, (4343 + 5306) / 2, 2);
            printf("  (6d) Schuss waehrend des Satzes: Mitschnitt L/R %d/%d, Soll %d wie in (3)\n", m.pk_l, m.pk_r, s3);
            CHECK(m.pk_l * 100 >= s3 * 97 && m.pk_l * 100 <= s3 * 103 && m.pk_r * 100 >= s3 * 97 && m.pk_r * 100 <= s3 * 103,
                  "(6d) Geraeusch waehrend eines gesprochenen Satzes: Spitze %d/%d, Soll %d +- 3 %% (wie ohne Satz - nur die Musik "
                  "tritt zurueck; der Rest ist der Hall-Auslauf des abgeschalteten Rotors)", m.pk_l, m.pk_r, s3);
        }
    }
#endif
    free(rdt); free(rdt2); free(s_pcm); free(stille);
    remove(s_cap);

    if (fails == 0) { printf("test_r42_ton_mischer: OK (%d Pruefungen)\n", checks); return 0; }
    printf("test_r42_ton_mischer: %d von %d FEHLGESCHLAGEN\n", fails, checks);
    return 1;
}
