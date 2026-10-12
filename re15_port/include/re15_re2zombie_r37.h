/**
 * @file re15_re2zombie_r37.h
 * @brief Runde 37 Spur L "re2zombie" — Mess-/Gegenprobe-Schalter der RE2-Zombie-KI
 *        (enemy_ai_re2_zombie.c). Dossier analysis/befunde_runde37/L_re2zombie.md.
 *
 * Die drei Produzenten des Arme-hoch-Gangs `+0x4 = 0x201` (EXEC[2] @0x80102260) im Gang-Executor
 * EXEC[1] @0x80101A40 (RE2 Retail EMZ0.BIN, selbst disassembliert):
 *   Bit 0  (1) TIMER       @0x80101D68-DBC  +0x15A abgelaufen, (rand & 3) == 0, dist < 0x1388
 *   Bit 1  (2) NAHBEREICH  @0x80101EC4-F10  dist < 0x7D0, Spieler im +-512-Kegel, nicht beansprucht
 *   Bit 2  (3) RENNEN      @0x80101F14-F60  0x800CFBF6 & 4, dist < 0x9C4, Spieler im +-1324-Kegel
 * Standard = alle drei an (das Original). Die Maske ist NUR fuer Sonden/Gegenproben da.
 */
#ifndef RE15_RE2ZOMBIE_R37_H
#define RE15_RE2ZOMBIE_R37_H

#define RE15_R37L_PROD_TIMER   0x1u
#define RE15_R37L_PROD_NAH     0x2u
#define RE15_R37L_PROD_RENNEN  0x4u
#define RE15_R37L_PROD_ALLE    0x7u

void     re15_re2z_r37_produzenten(unsigned mask);   /* Gegenprobe: Maske der aktiven Produzenten */
unsigned re15_re2z_r37_produzenten_maske(void);
/* Zaehler (nur Sonden): Wechsel nach 0x201 je Produzent seit dem letzten Ruecksetzen. */
void     re15_re2z_r37_zaehler_reset(void);
unsigned re15_re2z_r37_zaehler(int produzent);       /* produzent 0..2 = (1)..(3) */

#endif
