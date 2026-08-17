/**
 * @file test_rotor_bgm_pin.c
 * @brief ROOM1170-ROTOR: der SUB-Layer muss vom SKRIPT gekeyt werden — und hoerbar sein.
 *
 * Nutzer-Report "Man hoert den Helikopter-Rotor nicht im Intro bei ROOM1170". Der Fix bestand
 * darin, eine PORT-ERFINDUNG zu entfernen (eine Distanz-/Pan-Skalierung des BGM-SUB-Layers mit
 * der SE-Mathematik FUN_80045a64). Dieser Pin haelt beide Haelften fest:
 *   (1) der Rotor ist im ON-Fenster HOERBAR und traegt die Hubschrauber-Signatur
 *       (Huellkurven-Peak bei der Blade-Pass-Frequenz ~10.9 Hz),
 *   (2) das SKRIPT allein steuert ihn: vor Sce_bgm_control(1,1) und nach (1,2) ist er STUMM.
 *
 * Warum (2) nicht trivial ist: die entfernte Skalierung schrieb s_ss_sub.mvol_l/r in JEDEM Frame
 * neu und hat damit das op2-Stop des Skripts jeden Frame wieder aufgehoben. Gemessen im echten
 * ROOM1170-Durchlauf (RE15_AUDIO_CAP_SYNC, Rotor-Layer isoliert gegen einen rotor-stummen
 * Referenzlauf): VORHER lief der Rotor im OFF-Fenster mit RMS 602 weiter, NACHHER RMS 0.1.
 *
 * BYTE-BELEGE (selbst disassembliert, info/Re1.5/PSX.EXE):
 *   Raum->BGM-Tabelle UNK_80074828 / Latch FUN_80044210: ROOM1170 (stage 0, room 0x17) -> 0x5572
 *     = MAIN 0x32 (Flag 1 = geladen-stumm) + SUB_15 (Flag A = 1 = stumm bis Skript).
 *   SUB-Loader FUN_80044774: SsSeqOpen + `lh a1,DAT_800b52b8` @0x80044960 + `jal 0x8005ab5c`
 *     (SsSeqSetVol) @0x80044970 + SsSeqPlay @0x80044980 fuer SEQ0.
 *   Auto-Play-Gate FUN_800444b0: `lbu DAT_800b52b5` @0x800444F0, `beq ==0xff -> Ende`
 *     @0x800444F8, `bne !=0 -> skip` @0x80044500 (SEQ0); SEQ#2 separat @0x80044528/@0x80044530
 *     mit SsSeqReplay @0x80044540.
 *   SCD-Steuerung Sce_bgm_control (0x54) -> FUN_80044da4, Jumptable @0x80010e58:
 *     op1 @0x80044e00-48 = SsSeqSetVol(slot-vol) + SsSeqPlay, op2 @0x80044e50-84 = SsSeqStop.
 *   Original-Ground-Truth der Slot-Lautstaerken (Savestate stage_saves/orig_1170_gp.sav):
 *     vol@0x800b52b0 = vol@0x800b52b8 = vol@0x800b52c0 = 127 — also UNSKALIERT.
 *   Gegenprobe zur entfernten Erfindung: `jal 0x80045a64` (4-Byte-alignter Instruktionswort-Scan
 *     0x0C011699 ueber die ganze PSX.EXE) hat EXAKT vier Aufrufer — @0x800451cc, @0x8004527c,
 *     @0x800454e8, @0x80045830 — alle im SE-Pfad; ueber alle acht Overlays NULL Treffer.
 *
 * MECHANIK DES PINS: ROOM1170s MAIN-Byte traegt Flag 1, MAIN32 ist also nach dem Laden STUMM,
 * solange das Skript es nicht aufdreht. Der Test dreht NUR den SUB-Slot auf — der Mitschnitt ist
 * damit von Haus aus rotor-rein, ohne Trickserei.
 *
 * Deterministisch: kein SDL-Geraet (RE15_AUDIO_CAP_SYNC rendert frame-gelockt 1470 Stereo-Frames
 * pro re15_audio_tick), feste Fensterlaengen, feste Assets.
 */
#include "re15_audio.h"
#include "re15_engine.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RATE      44100
#define PER_TICK  (RATE / 30)          /* 1470 Stereo-Frames pro Spielframe */

/* Der Test linkt das ECHTE audio_pc.c (nicht die re15_test_support-Stubs — die wuerden genau die
 * zu pruefenden Funktionen ersetzen). Diese beiden Symbole kommen sonst aus main.c: */
re15_engine_state_t g_engine;
void re15_debug_text(int x, int y, int z, const char *text)
{ (void)x; (void)y; (void)z; (void)text; }
/* wie re15_test_support.c: asset_pc.c selbst wuerde den ganzen PC-Renderer nachziehen */
unsigned char *re15_asset_read_file(const char *path, int *out_size)
{
    char buf[512];
    const char *pfx = "shared_assets/PSX/";
    FILE *f = NULL;
    if (strncmp(path, pfx, strlen(pfx)) == 0)
        snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path + strlen(pfx));
    else
        snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path);
    f = fopen(buf, "rb");
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

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

/* ---- Metrik ------------------------------------------------------------------------------- */

typedef struct { double rms; int peak; double env_amp; double env_hz; } metric_t;

/* Blade-Pass-Suche: Huellkurve (|x|, 5-ms-Gleitmittel, auf 2205 Hz dezimiert), dann Goertzel
 * ueber 8..14 Hz in 0.05-Hz-Schritten. Rueckgabe: staerkste Amplitude + ihre Frequenz. */
static void analyse(const int16_t *pcm, long frames, metric_t *out)
{
    memset(out, 0, sizeof *out);
    if (frames < RATE / 2) return;

    double acc = 0.0; int pk = 0;
    for (long i = 0; i < frames; i++) {
        int s = (pcm[i * 2 + 0] + pcm[i * 2 + 1]) / 2;
        acc += (double)s * (double)s;
        int a = s < 0 ? -s : s;
        if (a > pk) pk = a;
    }
    out->rms  = sqrt(acc / (double)frames);
    out->peak = pk;
    if (out->rms < 1.0) return;                       /* stumm -> keine Huellkurve */

    const int K = (int)(RATE * 0.005);                /* 5 ms Gleitmittel */
    const int DS = 20;                                /* -> 2205 Hz */
    long en = frames / DS;
    if (en < 128) return;
    double *e = (double *)malloc((size_t)en * sizeof(double));
    if (!e) return;
    double run = 0.0; long j = 0, w = 0;
    for (long i = 0; i < frames && j < en; i++) {
        int s = (pcm[i * 2 + 0] + pcm[i * 2 + 1]) / 2;
        run += (s < 0 ? -s : s);
        if (++w > K) { int o = (int)(i - K); run -= abs((pcm[o * 2 + 0] + pcm[o * 2 + 1]) / 2); w = K; }
        if ((i % DS) == 0) e[j++] = run / (double)w;
    }
    en = j;
    double mean = 0.0; for (long i = 0; i < en; i++) mean += e[i];
    mean /= (double)en;
    for (long i = 0; i < en; i++) e[i] -= mean;

    const double erate = (double)RATE / DS;
    double best = 0.0, bestf = 0.0;
    for (double f = 8.0; f <= 14.0001; f += 0.05) {
        double wq = 2.0 * M_PI * f / erate, cw = cos(wq), coeff = 2.0 * cw;
        double s0 = 0, s1 = 0, s2 = 0;
        for (long i = 0; i < en; i++) { s0 = e[i] + coeff * s1 - s2; s2 = s1; s1 = s0; }
        double re = s1 - s2 * cw, im = s2 * sin(wq);
        double amp = 2.0 * sqrt(re * re + im * im) / (double)en;
        if (amp > best) { best = amp; bestf = f; }
    }
    free(e);
    out->env_amp = best;
    out->env_hz  = bestf;
}

static int16_t *slurp16(const char *path, long *out_frames)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    int16_t *b = (int16_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_frames = sz / 4;
    return b;
}

/* ---- Lauf --------------------------------------------------------------------------------- */

#define W_PRE  90       /* Fenster VOR dem Skript-Key (SUB-Flag-A-Mute @0x800443A4)  */
#define W_ON  300       /* Fenster nach Sce_bgm_control(1,1)                         */
#define W_SET  30       /* Ausklingen der bereits klingenden Stimmen + Reverb nach op2 */
#define W_OFF 150       /* gemessenes Fenster danach                                  */

int main(void)
{
    static char capenv[1024];
    const char *cap = "rotor_pin.pcm";
    remove(cap);
    snprintf(capenv, sizeof capenv, "RE15_AUDIO_CAP_SYNC=%s", cap);
    putenv(capenv);
    static char noint[] = "RE15_NO_INTRO=1";
    putenv(noint);

    printf("== ROOM1170-Rotor: Skript-Key + Hoerbarkeit ==\n");
    re15_audio_init();
    /* ROOM1170 (stage 0, room 0x17): Tabelleneintrag 0x5572 -> MAIN32 Flag 1 (stumm) +
     * SUB_15 Flag A 1 (stumm). Es wird NUR der SUB-Slot aufgedreht -> der Mitschnitt ist
     * rotor-rein. */
    re15_audio_start_room_bgm(0, 0x17);

    /* Der gemeinsame Treiber re15_rotor_drive (engine/src/game_step_common.c) ruft in JEDEM
     * Gameplay-Frame re15_audio_rotor_update(cam_eye, cam_tgt, heli_pos). Der Pin fuehrt genau
     * diesen Aufruf mit ZWEI deutlich verschiedenen Geometrien, weil der BGM-Layer davon nicht
     * abhaengen darf: `jal 0x80045a64` (Distanz/Pan) hat in der ganzen PSX.EXE vier Aufrufer,
     * alle im SE-Pfad; kein SsSeq-Volumen wird jemals positionsabhaengig skaliert. Die Zahlen
     * unten sind BELIEBIGE Szenen-Geometrie (nah/fern) — sie sind absichtlich nicht als
     * Original-Daten ausgewiesen; gepinnt wird die UNABHAENGIGKEIT. */
    static const int32_t eye[3]  = {     0, -3000,  6000 };
    static const int32_t tgt[3]  = {     0,     0,     0 };
    static const int32_t near_[3] = {  -500,  -800,   700 };
    static const int32_t far_[3]  = {-40000,-20000, 40000 };

    for (int i = 0; i < W_PRE; i++) { re15_audio_rotor_update(eye, tgt, far_); re15_audio_tick(); }
    re15_audio_seq_ctl(1, 1);                       /* Sce_bgm_control(1,1) @0x80044e00-48 */
    for (int i = 0; i < W_ON; i++)  { re15_audio_rotor_update(eye, tgt, far_); re15_audio_tick(); }
    re15_audio_seq_ctl(1, 2);                       /* Sce_bgm_control(1,2) @0x80044e50-84 */
    for (int i = 0; i < W_SET + W_OFF; i++) { re15_audio_rotor_update(eye, tgt, near_);
                                              re15_audio_tick(); }
    /* zweites ON-Fenster mit NAHER Geometrie — muss gleich laut sein wie das ferne */
    re15_audio_seq_ctl(1, 1);
    for (int i = 0; i < W_ON; i++)  { re15_audio_rotor_update(eye, tgt, near_); re15_audio_tick(); }
    re15_audio_shutdown();

    long frames = 0;
    int16_t *pcm = slurp16(cap, &frames);
    CHECK(pcm != NULL, "Mitschnitt %s nicht lesbar (RE15_AUDIO_CAP_SYNC)", cap);
    if (!pcm) { printf("\n%d Pin(s) FEHLGESCHLAGEN\n", fails); return 1; }
    long want = (long)(W_PRE + W_ON + W_SET + W_OFF + W_ON) * PER_TICK;
    CHECK(frames >= want, "Mitschnitt zu kurz: %ld statt %ld Stereo-Frames", frames, want);
    if (frames < want) { printf("\n%d Pin(s) FEHLGESCHLAGEN\n", fails); return 1; }

    metric_t pre, on, off, on2;
    analyse(pcm,                                                     (long)W_PRE * PER_TICK, &pre);
    analyse(pcm + (long)W_PRE * PER_TICK * 2,                        (long)W_ON  * PER_TICK, &on);
    analyse(pcm + (long)(W_PRE + W_ON + W_SET) * PER_TICK * 2,       (long)W_OFF * PER_TICK, &off);
    analyse(pcm + (long)(W_PRE + W_ON + W_SET + W_OFF) * PER_TICK * 2, (long)W_ON * PER_TICK, &on2);

    printf("  PRE  (%3d Frames, vor op1):      rms=%8.1f peak=%5d\n", W_PRE, pre.rms, pre.peak);
    printf("  ON-fern  (%3d Frames, nach op1): rms=%8.1f peak=%5d  Blade-Pass %.2f Hz (Amp %.1f)\n",
           W_ON, on.rms, on.peak, on.env_hz, on.env_amp);
    printf("  OFF  (%3d Frames, ab op2+%d):    rms=%8.1f peak=%5d\n", W_OFF, W_SET, off.rms, off.peak);
    printf("  ON-nah   (%3d Frames, 2. op1):   rms=%8.1f peak=%5d  Blade-Pass %.2f Hz (Amp %.1f)\n",
           W_ON, on2.rms, on2.peak, on2.env_hz, on2.env_amp);

    /* (1) Vor dem Skript-Key STUMM — das ist das Flag-A-Mute des SUB-Loaders
     *     (`sb v0,DAT_800b52b5` @0x800443A4 mit (sub_b>>6)&1 = 1). */
    CHECK(pre.rms < 1.0,
          "PIN 1: vor Sce_bgm_control(1,1) muss der SUB-Layer stumm sein (SUB-Flag-A-Mute "
          "@0x800443A4, Auto-Play-Gate FUN_800444b0 @0x80044500); gemessen rms=%.1f", pre.rms);

    /* (2) Nach op1 HOERBAR. Der Schwellwert ist bewusst weit unter dem Messwert (gemessen
     *     rms ~ 900 im echten Durchlauf, ~1300 in diesem isolierten Fenster) — er faellt,
     *     sobald irgendeine Skalierung den Layer wieder herunterregelt. */
    CHECK(on.rms > 300.0,
          "PIN 2: nach Sce_bgm_control(1,1) MUSS der Rotor hoerbar sein (op1 = SsSeqSetVol + "
          "SsSeqPlay @0x80044e00-48, Original-Slot-vol 127); gemessen rms=%.1f", on.rms);

    /* (3) Die Hubschrauber-Signatur: Blade-Pass-Modulation ~10.9 Hz. Toleranz 9.5..12.5 Hz. */
    CHECK(on.env_hz >= 9.5 && on.env_hz <= 12.5 && on.env_amp > 1.0,
          "PIN 3: der ON-Layer muss die Blade-Pass-Huellkurve ~10.9 Hz tragen "
          "(SUB_15 SEQ0, 152 B, ppqn 0x01e0); gemessen %.2f Hz (Amp %.1f)",
          on.env_hz, on.env_amp);

    /* (4) Nach op2 wieder STUMM. GENAU HIER schlug die entfernte Port-Erfindung zu: sie
     *     ueberschrieb s_ss_sub.mvol_l/r jeden Frame und hob das Stop auf (echter Durchlauf:
     *     vorher rms 602 im OFF-Fenster, nachher 0.1). */
    CHECK(off.rms < 1.0,
          "PIN 4: nach Sce_bgm_control(1,2) MUSS der Rotor stumm sein (op2 = SsSeqStop "
          "@0x80044e50-84) — ein Per-Frame-Schreiber auf s_ss_sub.mvol hebt das Stop auf; "
          "gemessen rms=%.1f", off.rms);

    /* (5) KEINE Positionsabhaengigkeit: dasselbe Skript-op1 muss mit NAHER wie mit FERNER
     *     Heli-Geometrie gleich laut sein. Die entfernte Erfindung lag je nach Kamera-Cut bei
     *     44..96 von 127 (-2.4..-9.2 dB) — das faellt hier mit >20% Pegelunterschied auf. */
    {   double rel = (on.rms > 1.0) ? fabs(on2.rms - on.rms) / on.rms : 1.0;
        printf("  Pegelunterschied fern/nah: %.2f%%\n", 100.0 * rel);
        CHECK(rel < 0.10,
              "PIN 5: der BGM-SUB-Layer darf NICHT von der Heli-/Kamera-Geometrie abhaengen "
              "(FUN_80045a64 hat 4 Aufrufer, alle im SE-Pfad: @0x800451cc/@0x8004527c/"
              "@0x800454e8/@0x80045830); fern rms=%.1f vs nah rms=%.1f (%.1f%%)",
              on.rms, on2.rms, 100.0 * rel);
    }

    free(pcm);
    remove(cap);
    if (fails) { printf("\n%d Pin(s) FEHLGESCHLAGEN\n", fails); return 1; }
    printf("\nROTOR-PIN: alle Pins gruen\n");
    return 0;
}
