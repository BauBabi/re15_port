/*
 * Runde 41 Spur Q "restpunkte" - Mechanik der Punkte 4 und 5 (Dossier analysis/befunde_runde41/Q_restpunkte.md).
 *
 * gator    Punkt 4: re15_gatorszene_r41_gator_kommt = Tor vor dem Setzen des Alligators (Form RE2 ROOM40A0 @0x0054 `21 04 33 00` vor Sce_em_set @0x00A0),
 *          Sperrbit = (9,111) "Alligator tot" (Zeitpunkt = RE2-Tot-Bit (5,0x1E) @0x80102660). Bit 110 allein (Kampf begonnen) sperrt NICHT: der Alligator traegt
 *          den Water Key. Der Spinnen-Filter gilt ohne Schalter weiter.
 * original Punkt 5, Prozess mit RE15_GATOR_ORIGINAL=1 (der Schalter wird einmal je Prozess gelesen): der Filter verwirft nichts, das Tor sperrt nicht.
 * gpu      Punkt 3: Groessengrenze der PSX-GPU (re15_gpu_grenze_r41.h), Ursache der stillen Enden des Ketten-Riegels.
 */
#include <stdio.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_boss_gator.h"
#include "re15_engine.h"
#include "re15_gatorszene_r41.h"
#include "re15_gpu_grenze_r41.h"
#include "re15_scd.h"

static int g_fail = 0;
#define PRUEF(c, ...) do { printf("  %s: ", (c) ? "ok" : "FEHLER"); printf(__VA_ARGS__); printf("\n"); if (!(c)) g_fail = 1; } while (0)

static void teil_gator(void)
{
    printf("== gator ==\n");
    re15_game_state_init();
    PRUEF(!re15_gator_original(), "ohne Schalter");
    PRUEF(re15_gatorszene_r41_gator_kommt() == 1, "neues Spiel: der Alligator kommt");
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);
    PRUEF(re15_gatorszene_r41_gator_kommt() == 1, "Kampf begonnen (9,%d), lebt: er kommt wieder (Water Key)", RE15_GSZ_BIT_GESEHEN);
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT, 1);
    PRUEF(re15_gatorszene_r41_gator_kommt() == 0, "tot (9,%d): er kommt nicht mehr", RE15_GSZ_BIT_LIEGT);
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GENOMMEN, 1);
    PRUEF(re15_gatorszene_r41_gator_kommt() == 0, "Schluessel genommen (9,%d): (9,%d) bleibt, er kommt nicht mehr", RE15_GSZ_BIT_GENOMMEN, RE15_GSZ_BIT_LIEGT);
    PRUEF(re15_gatorszene_r41_spawn_weg(RE15_GSZ_RAUM, RE15_GSZ_SPINNE_TYP, RE15_GSZ_SPINNE0_X, RE15_GSZ_SPINNE0_Z) == 1, "Spinnen-Filter im Bosskampf-Design an");
}

static void teil_original(void)
{
    printf("== original ==\n");
    re15_game_state_init();
    PRUEF(re15_gator_original(), "RE15_GATOR_ORIGINAL gesetzt");
    PRUEF(re15_gatorszene_r41_spawn_weg(RE15_GSZ_RAUM, RE15_GSZ_SPINNE_TYP, RE15_GSZ_SPINNE0_X, RE15_GSZ_SPINNE0_Z) == 0, "Satz @0x00AB0 bleibt");
    PRUEF(re15_gatorszene_r41_spawn_weg(RE15_GSZ_RAUM | 1u, RE15_GSZ_SPINNE_TYP, RE15_GSZ_SPINNE1_X, RE15_GSZ_SPINNE1_Z) == 0, "Satz @0x00AC4 bleibt (Elza)");
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT, 1);
    PRUEF(re15_gatorszene_r41_gator_kommt() == 1, "das Tor gehoert zum Bosskampf-Design");
}

/* Punkt 3: Groessengrenze der PSX-GPU (psx-spx "Size Restriction": 1023 horizontal, 511 vertikal). Dazu die Rechnung des SDL-CPU-Rasterers (SDL_triangle.c:618
 * `s2_x_area.x = s2->x * area`, 32 Bit, FP_BITS 1): an der Grenze laeuft sie nicht ueber, die gemessene Flaeche aus ROOM20B0 F767 (1 130 976 900) schon. */
static void teil_gpu(void)
{
    printf("== gpu ==\n");
    PRUEF(!re15_gpu_dreieck_zu_gross(0, 0, 1023, 0, 0, 511), "1023 x 511 wird gezeichnet");
    PRUEF(re15_gpu_dreieck_zu_gross(0, 0, 1024, 0, 0, 10), "1024 breit: weg");
    PRUEF(re15_gpu_dreieck_zu_gross(0, 0, 10, 0, 0, 512), "512 hoch: weg");
    PRUEF(!re15_gpu_dreieck_zu_gross(-300, -100, 300, 100, 0, 400), "Abstand zaehlt, nicht die Lage (psx-spx: Y1 -100, Y2 +400 wird gezeichnet)");
    PRUEF(re15_gpu_dreieck_zu_gross(0, -300, 0, 300, 10, 0), "psx-spx: Y1 -300, Y2 +300 nicht");
    PRUEF(re15_gpu_dreieck_zu_gross(100, 100, 10250, 100, 100, 8000), "Quader F767 aus ROOM20B0 weg");
    {   /* GTE-Saettigung vor der Grenze (psx-spx: ScrX/ScrY FIFO -400h..+3FFh) */
        int x0 = 12288, y0 = 100, x1 = 200, y1 = 100, x2 = 200, y2 = 150;
        PRUEF(re15_gpu_dreieck_verwerfen(0, 0, &x0, &y0, &x1, &y1, &x2, &y2) == 0 && x0 == 0x3FF, "Ecke ausserhalb: saettigt auf 3FFh, Abstand 823, die Hardware zeichnet");
        x0 = -5000; y0 = -9000; x1 = 300; y1 = 9000; x2 = 300; y2 = 100;
        PRUEF(re15_gpu_dreieck_verwerfen(0, 0, &x0, &y0, &x1, &y1, &x2, &y2) == 1 && y0 == -0x400 && y1 == 0x3FF, "gesaettigt immer noch ueber 511: weg");
    }
    {
        const long long flaeche_max = 4LL * 2 * RE15_GPU_MAX_DX * RE15_GPU_MAX_DY / 2;   /* cross_product von Koordinaten << FP_BITS (1) */
        PRUEF(767 * flaeche_max < 2147483648LL, "groesste zugelassene Flaeche %lld x hoechste Texturzeile 767 bleibt unter 2^31", flaeche_max);
        PRUEF(255 * 1130976900LL >= 2147483648LL, "gemessene Flaeche laeuft ueber");
    }
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if (!strcmp(t, "gator")) teil_gator();
    else if (!strcmp(t, "original")) teil_original();
    else if (!strcmp(t, "gpu")) teil_gpu();
    else { printf("Teil?\n"); return 2; }
    printf("%s\n", g_fail ? "FAIL" : "OK");
    return g_fail;
}
