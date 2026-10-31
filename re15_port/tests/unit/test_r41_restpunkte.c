/*
 * Runde 41 Spur Q "restpunkte" - Mechanik der Punkte 4 und 5 (Dossier analysis/befunde_runde41/Q_restpunkte.md).
 *
 * gator    Punkt 4: re15_gatorszene_r41_gator_kommt = Tor vor dem Setzen des Alligators (Form RE2 ROOM40A0 @0x0054 `21 04 33 00` vor Sce_em_set @0x00A0),
 *          Sperrbit = (9,111) "Alligator tot" (Zeitpunkt = RE2-Tot-Bit (5,0x1E) @0x80102660). Bit 110 allein (Kampf begonnen) sperrt NICHT: der Alligator traegt
 *          den Water Key. Der Spinnen-Filter gilt ohne Schalter weiter.
 * original Punkt 5, Prozess mit RE15_GATOR_ORIGINAL=1 (der Schalter wird einmal je Prozess gelesen): der Filter verwirft nichts, das Tor sperrt nicht.
 */
#include <stdio.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_boss_gator.h"
#include "re15_engine.h"
#include "re15_gatorszene_r41.h"
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

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if (!strcmp(t, "gator")) teil_gator();
    else if (!strcmp(t, "original")) teil_original();
    else { printf("Teil?\n"); return 2; }
    printf("%s\n", g_fail ? "FAIL" : "OK");
    return g_fail;
}
