/* re15_granate_r36.h — Runde 36 Spur D: die Handgranate — (1) KEIN Feuer an Zombies bei der
 * Explosiv-Granate (NUTZER-VORGABE), (2) Wurf-Aussetzer ("Granate wird manchmal nicht geworfen").
 *
 * Dossier: analysis/befunde_runde36/D_granate.md. Adressen = RE2 Leon (info/re2leon/PSX.EXE bzw.
 * COMMON/BIN/EMOVL10_S0.BIN = Zombie-Overlay @0x80100000) und RE1.5 PSX.EXE, selbst disassembliert.
 *
 * (1) FEUER NACH DER HE-EXPLOSION. Der Zerreiss-Tod der RE2-Zombie-Familie (FUN_80108BEC, DEATH[9][1]
 *     @0x8010CD68+4) ruft fuer die Zeilen 9 (HE/GL-Explosiv) und 17 (Rakete) den Spreng-Russ FUN_8010640C
 *     (`beq v1,9` @0x80108cc8, `bne v1,17` @0x80108cd0, `lhu 270 / andi 0x80 / bne` @0x80108cd8-e4,
 *     `jal 0x8010640c` @0x80108cec; dieselbe Leiter @0x801051E8-208 und @0x801084A0-B8). FUN_8010640C
 *     spawnt ZWEI Flammen der RE2-Klasse 5 (= RE2-CORE00-Id 5 FEUER, Sub 3 = Anim 0..9 mit Status 0x800
 *     "folgt dem Anker"): 0x05032710 am Part 0 mit a3 = (0,200,0) (`lui a0,0x503 / ori a0,a0,0x2710`
 *     @0x80106418-1c, `addiu v0,zero,200` @0x80106424, `jal 0x8001bf10` @0x80106440, Anker +72 @0x80106444)
 *     und 0x050313E8 am Part 3 (`lw v0,516(a2) / andi 0x1 / beq` @0x80106450-5c, `lui a0,0x503 / ori
 *     a0,a0,0x13e8` @0x80106460-64, `addiu a2,a2,588` @0x8010646c, `jal 0x8001bf10` @0x80106470); danach
 *     die Russ-Tinten (@0x80106478-f8). Der Port-Dekoder bildet Klasse 5 auf RE1.5-CORE00-Id 8 (FEUER)
 *     ab — gemessen (Dossier §2.1): je zerrissenem Zombie 2 Flammen, 211+ Bilder, erloeschen nicht.
 *     RE2 Retail laesst die HE-Leiche also brennen. NUTZER-VORGABE (AUFTRAG.md Punkt 1): "Bei normalen
 *     Granaten keinen Feuer-Effekt an den Zombies, nur das Zerplatzen der Koerperteile; Feuer nur bei
 *     den incendiary Granaten." FORM der Abweichung: die beiden Flammen-Spawns @0x80106418-44 und
 *     @0x80106450-74 entfallen (fuer alle Aufrufer von FUN_8010640C = Zeilen 9/17, beide explosiv);
 *     die Russ-Tinten @0x80106478-f8, das Zerreissen (0x80108BEC) und das Wegschleudern (0x80109610)
 *     bleiben; die Brand-Verkohlung FUN_80106128 (Zeile 10 = Brandgranate 0x0B / Art 4, Zeile 16
 *     Flammenwerfer) und die Saeure FUN_80106310 (Zeile 11) bleiben unveraendert.
 */
#ifndef RE15_GRANATE_R36_H
#define RE15_GRANATE_R36_H

#include <stdint.h>

/** Punkt 1 — Gate der beiden Flammen-Spawns des Spreng-Russ FUN_8010640C (@0x80106418-44 Part 0,
 *  @0x80106450-74 Part 3). Rueckgabe 1 = Flammen spawnen (RE2 Retail), 0 = KEINE Flammen (NUTZER-
 *  VORGABE Runde 36: Feuer nur bei der Brandgranate). `zeile` = RE2-Treffer-Zeile +0x5 des Zombies
 *  (9 = HE/GL-Explosiv, 17 = Rakete — die einzigen Aufrufer der Russ-Funktion). Kein Umgebungs-
 *  schalter: die Vorgabe gilt immer; die Funktion existiert, damit die Sonde die Zuordnung (welche
 *  Zeile = welche Granatenart) pruefen kann und der Haken in enemy_ai_re2_zombie.c eine Zeile bleibt. */
int re15_granate_r36_russ_flammen(unsigned zeile);

/** Zaehlung fuer Sonden: wie oft der Spreng-Russ seine Flammen UNTERDRUECKT hat (seit Reset). */
unsigned re15_granate_r36_russ_unterdrueckt(void);
void     re15_granate_r36_zaehler_reset(void);

#endif /* RE15_GRANATE_R36_H */
