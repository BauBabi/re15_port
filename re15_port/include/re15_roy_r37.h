/* =============================================================================================
 * re15_roy_r37.h — Runde 37 Spur K "roy": Elzas Roy (NPC-Typ 0x47) mit SEINEM Modell und seinen
 * Animationen (Nutzer 2026-10-12: "Bei Elzas Szenario stimmt weder das Modell von Roy noch all seine
 * Animationen. Ausserdem wiederholen sich seine Animationen wieder staendig").
 *
 * ORIGINAL (selbst disassembliert, Dossier analysis/befunde_runde37/K_roy.md §2):
 *   Der Modell-Lader FUN_80022300 (Sce_em_set -> `jal 0x80022300` @0x80042328) waehlt das Archiv
 *   NACH DEM ELZA-BIT, nicht nach dem Typ:
 *     8002232c  lw   v1,-13764(v1)   ; DAT_800aca3c
 *     80022334  bltz v1,0x800223ac   ; Bit 31 = Elza
 *     8002233c  ori  v0,zero,0x26    ; Leon: Datei 0x26 = EMD/CDEMD0.EMS (LBA 1316), TOC @0x80072f38
 *     800223ac  ori  v0,zero,0x27    ; Elza: Datei 0x27 = EMD/CDEMD1.EMS (LBA 3627), TOC @0x80073178
 *   Typ 0x47 ist bei Leon Elliot (CDEMD0 Sektor 1644, 127640 B = PLD/ELLIOT.*), bei Elza Roy
 *   (CDEMD1 Sektor 1639, 142272 B — RPD-Uniform, verwundet; eigene Baenke: Loco-Clip 7 = 113 Bilder
 *   Sturz, eigene Bank Clip 5 = 61 Bilder Gang). Gemessen in den Elza-Spielstaenden des Nutzers
 *   (DuckStation-RAM: Roys Baenke == CDEMD1, +0x180/+0x184 = ROOM1011 RBJ-Record 1).
 *
 * PORT: Die Elliot-Sonderwege des Ports (ELLIOT.*-Modell, RBJ-Record-0-Ueberlagerung, Ausnahme im
 * RBJ-Marker-Binder) gelten nur fuer LEONS Typ 0x47. Elzas Typ 0x47 laeuft den allgemeinen NPC-Weg
 * (Registry-Bank, Marker-Binder FUN_8001b3f8, Executor-Kanaele) mit dem CDEMD1-Blob.
 * ============================================================================================= */
#ifndef RE15_ROY_R37_H
#define RE15_ROY_R37_H

#include <stdint.h>

#define RE15_ROY_TYP 0x47u            /* Sce_em_set-Typ: ROOM1011 @0x009F6/@0x00A3E, ROOM1021 @0x01E0A */

#define RE15_EMS_ARCHIV_LEON 0        /* Datei 0x26 = EMD/CDEMD0.EMS (@0x8002233c) */
#define RE15_EMS_ARCHIV_ELZA 1        /* Datei 0x27 = EMD/CDEMD1.EMS (@0x800223ac) */

/* Archiv, aus dem das Original JETZT laedt: Elza-Bit DAT_800aca3c Bit 31 (bltz @0x80022334). */
int re15_ems_archiv_aktiv(void);

/* TOC-Eintrag des Archivs fuer `type` (Index type-0x10 @0x80022338): Sektor im Archiv + Laenge.
 * Rueckgabe 1 = Eintrag vorhanden (Laenge > 0), 0 = kein Modell fuer diesen Typ. */
int re15_ems_toc(int archiv, uint8_t type, uint32_t *sektor, uint32_t *laenge);

/* Relativer Pfad des Archivs unter der CD-Wurzel ("EMD/CDEMD0.EMS" / "EMD/CDEMD1.EMS"). */
const char *re15_ems_archiv_datei(int archiv);

/* 1 = `type` ist LEONS Typ 0x47 (Elliot) -> die Elliot-Sonderwege des Ports gelten.
 * 0 fuer jeden anderen Typ und fuer Elzas Typ 0x47 (Roy). */
int re15_typ47_ist_elliot(uint8_t type);

/* 1 = der Port laedt `type` aus dem Elza-Archiv CDEMD1 (statt des Port-Standards CDEMD0).
 * Umfang dieser Runde: nur Roy (Typ 0x47 bei gesetztem Elza-Bit). Die uebrigen Typen laedt der Port
 * weiter aus CDEMD0, obwohl das Original sie bei Elza ebenfalls aus CDEMD1 holt (Befund im Dossier
 * K_roy.md §6 OFFEN, ausserhalb Spur K). */
int re15_roy_aus_elza_archiv(uint8_t type);

#endif /* RE15_ROY_R37_H */
