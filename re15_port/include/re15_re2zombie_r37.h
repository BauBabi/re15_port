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
 * Standard = alles an (RE15_R37L_ALLES, das Original). Die Maske ist NUR fuer Sonden/Gegenproben da.
 */
#ifndef RE15_RE2ZOMBIE_R37_H
#define RE15_RE2ZOMBIE_R37_H

#include <stdint.h>

#define RE15_R37L_PROD_TIMER   0x1u
#define RE15_R37L_PROD_NAH     0x2u
#define RE15_R37L_PROD_RENNEN  0x4u
#define RE15_R37L_PROD_ALLE    0x7u
/* Die zwei Abweichungen des Steh-Zweigs und DECISION[12] (Runde 37 Spur L, ebenfalls nur fuer Gegenproben abschaltbar):
 *   Bit 3  (b)   DECISION[0] Block 3 committet bei (rand & 1) == 1 (`beq v0,zero,0x801013d4` @0x801013c8)
 *   Bit 4  (c)   EXEC[0] P1: Selbstwecker auch im Wander-Wurf-Takt, P2 erst im naechsten (@0x80101540-a0, @0x8010159c)
 *   Bit 5  D12   DECISION[12] @0x8010460C: Griff aus dem Ausfall-Stoss
 * Maske 0 = exakt der Stand bis Runde 37 (Basis e399ee3f). */
#define RE15_R37L_B            0x08u
#define RE15_R37L_C            0x10u
#define RE15_R37L_D12          0x20u
#define RE15_R37L_ALLES        0x3Fu

void     re15_re2z_r37_produzenten(unsigned mask);   /* Gegenprobe: Maske der aktiven Produzenten */
unsigned re15_re2z_r37_produzenten_maske(void);
/* Zaehler (nur Sonden): Wechsel nach 0x201 je Produzent seit dem letzten Ruecksetzen. */
void     re15_re2z_r37_zaehler_reset(void);
unsigned re15_re2z_r37_zaehler(int produzent);       /* produzent 0..2 = (1)..(3) */
int      re15_re2z_r37_letzter_produzent(int slot);  /* 0..2 = Produzent des letzten Wechsels dieses Platzes, -1 = keiner */
/* Den (ahead+1)-ten naechsten Wurf des RE2-Zufallsstroms (@0x80015FE8) voraussehen, ohne ihn zu bewegen (nur Sonden). */
uint32_t re15_re2z_rng_peek(int ahead);

#endif
