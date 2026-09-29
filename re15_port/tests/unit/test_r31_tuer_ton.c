/* test_r31_tuer_ton.c — Riegel "Ton ueberlebt den Raumwechsel" (Runde 31, analysis/befunde_runde31/
 * tueren_04_bau.md Abschnitt 3/8).
 *
 * RE2: der Raumwechsel schaltet nur Stimmen ab, deren SPU-Startadresse im Raumbereich liegt
 * (FUN_80059e54 @0x80059e90 jal 0x800597a4; @0x800597bc..0x80059810: 0x14441..0x3DC4F), die
 * Tuerbank liegt bei 0x3DC50 (@0x80014f08 lui a2,0x3 / @0x80014f18 ori a2,a2,0xdc50) - der
 * Schliesston (Door_exit, Tonkopf-Eintrag 1, Stimme 23 = SE-Stimme 7) klingt weiter.
 *
 * Geprueft mit dem ECHTEN audio_pc.c (RE15_AUDIO_CAP_SYNC, kein SDL-Geraet):
 *   1. Tonteil DOOR13.DO2[0..0x3DA8) laden (Groesse aus der Tabelle @0x8009a520), Ton 1 spielen,
 *      pumpen -> SE-Stimme 7 an.
 *   2. re15_audio_load_room_banks() (der Aufruf in re15_room_apply_pending) -> Stimme 7 IMMER NOCH an.
 *   3. Zwei Spielbilder weiter -> noch an (0,60 s Ton).
 *   4. Anderer Tonteil (DOOR25) wird geladen -> die alte Bank wird freigegeben, keine Stimme zeigt mehr
 *      auf freigegebenes PCM (Stimme 7 aus).
 */
#include "re15_audio.h"
#include "re15_engine.h"
#include "re15_door_seq.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

/* Der Test linkt das ECHTE audio_pc.c; diese Symbole kommen sonst aus main.c (wie test_rotor_bgm_pin). */
re15_engine_state_t g_engine;
void re15_debug_text(int x, int y, int z, const char *text)
{ (void)x; (void)y; (void)z; (void)text; }
unsigned char *re15_asset_read_file(const char *path, int *out_size)
{
    char buf[512];
    const char *pfx = "shared_assets/PSX/";
    if (strncmp(path, pfx, strlen(pfx)) == 0)
        snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path + strlen(pfx));
    else
        snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path);
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

static int g_fehler = 0;
#define PRUEF(c, ...) do { if (!(c)) { g_fehler++; printf("FEHLER: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *tonteil(int nr, int *n)
{
    int ton = 0, modell = 0, sektor = 0, groesse = 0;
    if (re15_door_seq_re2_archiv(nr, &ton, &modell, &sektor, &groesse) != 0) return NULL;
    char p[600];
    snprintf(p, sizeof p, "%s/DOOR/DOOR%02X.DO2", RE15_ASSET_RE2_DIR, nr);
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    uint8_t *b = (uint8_t *)malloc((size_t)ton);
    if (b && fread(b, 1, (size_t)ton, f) != (size_t)ton) { free(b); b = NULL; }
    fclose(f);
    *n = ton;
    return b;
}

int main(void)
{
    static char capenv[] = "RE15_AUDIO_CAP_SYNC=r31_tuer_ton.pcm";
    static char noint[] = "RE15_NO_INTRO=1";
    putenv(capenv);
    putenv(noint);
    re15_audio_init();

    int n13 = 0, n25 = 0;
    uint8_t *t13 = tonteil(0x13, &n13), *t25 = tonteil(0x25, &n25);
    PRUEF(t13 && n13 == 0x3DA8, "DOOR13-Tonteil (%d B, erwartet 0x3DA8 laut @0x8009a604)", n13);
    PRUEF(t25 && n25 == 0x44D8, "DOOR25-Tonteil (%d B, erwartet 0x44D8 laut @0x8009a6dc)", n25);
    if (!t13 || !t25) { printf("test_r31_tuer_ton: %d FEHLER\n", g_fehler); return 1; }

    PRUEF(re15_audio_re2_tuer_laden(t13, n13) == 1, "Tonteil DOOR13 nicht ladbar");
    re15_audio_re2_tuer_se(1);            /* Door_exit: Tonkopf-Eintrag 1 = Stimme 23 */
    re15_audio_se_pumpe();
    int vor = re15_audio_se_stimme_aktiv(7);
    re15_audio_tick();
    re15_audio_load_room_banks();         /* wie re15_room_apply_pending (room_common.c) */
    int nach = re15_audio_se_stimme_aktiv(7);
    re15_audio_tick(); re15_audio_tick();
    int spaeter = re15_audio_se_stimme_aktiv(7);
    printf("Schliesston DOOR13: Stimme 7 vor dem Raumwechsel %d, nach re15_audio_load_room_banks %d, "
           "zwei Bilder spaeter %d\n", vor, nach, spaeter);
    PRUEF(vor == 1, "Schliesston spielt nicht (SE-Stimme 7)");
    PRUEF(nach == 1 && spaeter == 1, "Schliesston ueberlebt den Raumwechsel nicht");

    /* derselbe Tonteil: keine Neuladung, Stimme bleibt */
    PRUEF(re15_audio_re2_tuer_laden(t13, n13) == 1 && re15_audio_se_stimme_aktiv(7) == 1,
          "gleicher Tonteil darf die laufende Stimme nicht beenden");
    /* anderer Tonteil: alte Bank frei, keine Stimme auf freigegebenem PCM */
    PRUEF(re15_audio_re2_tuer_laden(t25, n25) == 1, "Tonteil DOOR25 nicht ladbar");
    PRUEF(re15_audio_se_stimme_aktiv(7) == 0, "Stimme 7 zeigt nach dem Bankwechsel noch auf die alte Bank");
    free(t13); free(t25);
    printf(g_fehler ? "test_r31_tuer_ton: %d FEHLER\n" : "test_r31_tuer_ton: OK\n", g_fehler);
    return g_fehler ? 1 : 0;
}
