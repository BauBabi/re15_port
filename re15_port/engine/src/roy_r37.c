/* =============================================================================================
 * roy_r37.c — Runde 37 Spur K "roy": Modellarchiv-Wahl nach dem Elza-Bit (FUN_80022300) und der
 * Umfang der Elliot-Sonderwege. Belege im Kopf von re15_roy_r37.h und im Dossier
 * analysis/befunde_runde37/K_roy.md §2.
 * ============================================================================================= */
#include "re15_roy_r37.h"
#include "re15_gameflow.h"
#include "re15_elzaems_r37.h"   /* Runde 37 Spur N: Elza-Bit = SCD-Flag (1,0), vom Skript umschaltbar */

/* Beide TOCs roh aus der EXE (tools/r37_k/gen_ems_toc_r37.py): {sektor, laenge} je Typ 0x10..0x4F,
 * CDEMD0 @0x80072f38 (Leon), CDEMD1 @0x80073178 (Elza). */
#include "gen/ems_toc_r37.inc"

int re15_ems_archiv_aktiv(void)
{
    /* DAT_800aca3c Bit 31 — aus dem Charakter-Byte abgeleitet wie im Original (re15_gameflow.h
     * re15_char_variant, TITLE.BIN @0x801024a8/@0x801024e4); Runde 37 Spur N: dasselbe Bit ist SCD-Flag (1,0)
     * (Bank-Tabelle @0x80074664[1] = 0x800aca3c) und wird von vier Raumskripten umgeschaltet -> re15_elzabit. */
    return re15_elzabit() ? RE15_EMS_ARCHIV_ELZA : RE15_EMS_ARCHIV_LEON;
}

int re15_ems_toc(int archiv, uint8_t type, uint32_t *sektor, uint32_t *laenge)
{
    if (type < 0x10u || type >= 0x50u) return 0;              /* 64 Eintraege, Index type-0x10 */
    const uint32_t *t = (archiv == RE15_EMS_ARCHIV_ELZA) ? s_ems_toc_elza : s_ems_toc_leon;
    unsigned i = (unsigned)(type - 0x10u) * 2u;               /* 8 B je Eintrag (`sll v1,a0,3`) */
    if (t[i + 1] == 0) return 0;
    if (sektor) *sektor = t[i];
    if (laenge) *laenge = t[i + 1];
    return 1;
}

const char *re15_ems_archiv_datei(int archiv)
{
    return (archiv == RE15_EMS_ARCHIV_ELZA) ? "EMD/CDEMD1.EMS" : "EMD/CDEMD0.EMS";
}

int re15_typ47_ist_elliot(uint8_t type)
{
    /* Runde 37 Spur N: das Archiv, aus dem die 0x47-Bank des Raums stammt (gemerkt beim Sce_em_set). */
    return type == RE15_ROY_TYP && re15_ems_archiv_fuer_typ(type) == RE15_EMS_ARCHIV_LEON;
}

int re15_roy_aus_elza_archiv(uint8_t type)
{
    return type == RE15_ROY_TYP && re15_ems_archiv_fuer_typ(type) == RE15_EMS_ARCHIV_ELZA;
}
