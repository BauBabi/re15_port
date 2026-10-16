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
/* Nachbesserung 2 (Freigabe des Orchestrators 2026-10-16, AUFTRAG Runde 38): die Erholungsleiter des Treffer-Handlers
 * 0x80105438 P3 (re2z_hit_main case 3) wie das Original:
 *   Bit 6  P3    Ein-Angreifer-Riegel 0x800CFDCB & 0x80 vor dem 0xB01-Wurf (@0x80105AA0-B0) und das Leiter-Ende
 *                `bne a0,v1,0x80105ba0` @0x80105B3C: GLEICHER Clip (+0x218 == +0x14C, der Normalfall) -> 0x10101 mit
 *                +0x16B/+0x158 = 30/+0x16A/+0x15A = rand + 300 (@0x80105B44-80) und `jal 0x800152c8` @0x80105B98;
 *                ANDERER Clip -> 0x101 (`sw` im Delay-Slot @0x80105B40). Ohne Bit: die bis Runde 37 umgekehrte Bedingung. */
#define RE15_R37L_P3           0x40u
#define RE15_R37L_ALLES        0x7Fu

void     re15_re2z_r37_produzenten(unsigned mask);   /* Gegenprobe: Maske der aktiven Produzenten */
unsigned re15_re2z_r37_produzenten_maske(void);
/* Zaehler (nur Sonden): Wechsel nach 0x201 je Produzent seit dem letzten Ruecksetzen. */
void     re15_re2z_r37_zaehler_reset(void);
unsigned re15_re2z_r37_zaehler(int produzent);       /* produzent 0..2 = (1)..(3) */
int      re15_re2z_r37_letzter_produzent(int slot);  /* 0..2 = Produzent des letzten Wechsels dieses Platzes, -1 = keiner */
/* Den (ahead+1)-ten naechsten Wurf des RE2-Zufallsstroms (@0x80015FE8) voraussehen, ohne ihn zu bewegen (nur Sonden). */
uint32_t re15_re2z_rng_peek(int ahead);
/* Ausgaenge der Erholungsleiter P3 seit dem letzten re15_re2z_r37_zaehler_reset (nur Sonden):
 * 0 = 0xB01 (@0x80105ACC), 1 = 0xC01 (@0x80105B08), 2 = 0x201 (@0x80105B24), 3 = 0x10101 (@0x80105B44-50), 4 = 0x101 (@0x80105B40). */
unsigned re15_re2z_r37_p3_ausgang(int k);

#endif
