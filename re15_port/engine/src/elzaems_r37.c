/* =============================================================================================
 * elzaems_r37.c — Runde 37 Spur N "elzaems": Zwilling des Elza-Bits (DAT_800aca3c Bit 31 = SCD-Flag (1,0))
 * und das Archiv je Typ beim Sce_em_set. Belege im Kopf von include/re15_elzaems_r37.h und im Dossier
 * analysis/befunde_runde37/N_elzaems.md §2.
 * ============================================================================================= */
#include "re15_elzaems_r37.h"
#include "re15_roy_r37.h"       /* RE15_EMS_ARCHIV_LEON/_ELZA, re15_ems_toc */
#include "re15_gameflow.h"      /* g_gameflow.character, re15_char_variant */
#include "re15_room.h"          /* g_current_room_id */

/* ---- Zwilling von DAT_800aca3c Bit 31 ------------------------------------------------------------
 * Schreiber im Original: die Figurenwahl (TITLE.BIN @0x801024e4 `or`, @0x801024c8 `and`) und SCD-Set(1,0,v)
 * (LAB_8003fdd0, Bank-Tabelle @0x80074664[1] = 0x800aca3c). Der Port leitet den Figurenwert schon aus dem
 * Figurenbyte ab (re15_char_variant); hier kommt nur der Skriptwert hinzu. Er gilt, solange dieselbe Figur
 * gespielt wird (ein neues Spiel mit der anderen Figur spiegelt das Bit im Original neu). */
static int s_skript_wert  = -1;     /* -1 = seit der Figurenwahl kein Set(1,0,v) */
static int s_skript_figur = -1;     /* g_gameflow.character beim Schreiben */

void re15_elzabit_skript_set(int value)
{
    s_skript_wert  = value ? 1 : 0;
    s_skript_figur = (int)g_gameflow.character;
}

int re15_elzabit(void)
{
    if (s_skript_wert >= 0 && s_skript_figur == (int)g_gameflow.character)
        return s_skript_wert;
    return re15_char_variant();
}

/* ---- Archiv je Typ beim Sce_em_set ------------------------------------------------------------------
 * FUN_80022300 liest das Bit IM Sce_em_set (`jal 0x80022300` @0x80042328) — der Port laedt die Bank aber
 * erst beim ersten Zeichnen, wenn das Skript die Klammer schon geschlossen hat. Deshalb wird das Archiv hier
 * je Typ (Index Typ-0x10 wie @0x80022338, 64 Eintraege wie die TOCs @0x80072f38/@0x80073178) mit dem Raum
 * gemerkt. Alle Sce_em_set desselben Typs in einem Raum stehen im Original unter demselben Bitwert
 * (n1_bit31_zensus.txt: 3071 zweimal 0x30 unter 0, 50F1 zweimal 0x30 unter 0). */
static struct { unsigned raum; int8_t archiv; } s_merk[64];

void re15_ems_sce_em_set_merken(uint8_t type)
{
    if (type < 0x10u || type >= 0x50u) return;
    s_merk[type - 0x10u].raum   = g_current_room_id;
    s_merk[type - 0x10u].archiv = (int8_t)(re15_elzabit() ? RE15_EMS_ARCHIV_ELZA : RE15_EMS_ARCHIV_LEON);
}

int re15_ems_archiv_fuer_typ(uint8_t type)
{
    if (type >= 0x10u && type < 0x50u && s_merk[type - 0x10u].archiv >= 0 &&
        s_merk[type - 0x10u].raum == g_current_room_id && g_current_room_id != 0u)
        return s_merk[type - 0x10u].archiv;
    return re15_elzabit() ? RE15_EMS_ARCHIV_ELZA : RE15_EMS_ARCHIV_LEON;
}

void re15_elzaems_r37_reset(void)
{
    s_skript_wert = -1;
    s_skript_figur = -1;
    for (int i = 0; i < 64; i++) { s_merk[i].raum = 0u; s_merk[i].archiv = -1; }
}
