/*
 * Runde 37 Spur H "zombieaktiv" — Zombies "teilweise aktiv im Gang" wie RE2 (Dossier
 * analysis/befunde_runde37/H_zombieaktiv.md).
 *
 * NUTZER (AUFTRAG.md, Nachtrag 2 + Ergaenzung, woertlich): "die Zombies sind immer noch zu passiv. Beispielsweise, am
 * Ende von STAGE1 Leon, wenn wir die ganzen Zombies spawnen nach Irons tod - die Zombies bleiben einfach nur Stumpf alle
 * im Gang stehen. So sind sie keine Bedrohung .... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im
 * Gang." / "das soll nicht nur da gelten, das darf generell für die Zombies gelten" / "schaue einfach nach wie Resident
 * Evil 2 das macht".
 *
 * WIE RE2 ES MACHT (RE2 Retail, EMZ0.BIN + PSX.EXE, selbst disassembliert; im Original gemessen):
 *   Der Spawn-Satz Sce_em_set (RE2-Opcode 0x44, Handler 0x8005714C) legt seinen Deskriptor nach +0x10E
 *   (`lhu v0,4(v1)` @0x8005734c / `sh v0,270(s0)` @0x80057354). Der Zombie-INIT 0x8010065C schreibt als Grundwort
 *   0x0001 = Zustand 1 / Unterzustand 0 = STEHEN (`addiu v0,zero,1` / `sw v0,4(s2)` @0x801006ac-b8) und fuer
 *   Deskriptor 6 (`+0x10E & 0x3F`) 0x0101 = Zustand 1 / Unterzustand 1 = GANG (`addiu v0,zero,257` / `sw v0,4(s2)`
 *   @0x80100ab8-abc). Ein GEHENDER Zombie folgt dem Navigator FUN_8004A808 auf die Spielerlage (Wurzel @0x801002d0-358)
 *   aus jeder Entfernung; die Gang-Leiter DECISION[1] 0x80101714 hat keinen Rueckweg ins Stehen. Ein STEHENDER wird nur
 *   wach bei dist < 0x1388 im Vorderkegel 1024 mit Sicht (@0x801012f0-31c) oder nach +0x158 = rand+150 Bildern bei
 *   dist < 0x1D4C mit Sicht (@0x80101478-88 / @0x80101544-7c), sonst Zufalls-Wander (@0x8010151c-40).
 *   ZENSUS aller 495 RE2-RDTs: 433 Zombie-Saetze, Deskriptor 6 = 140 (32 %), 0 = 214; fast jeder Raum mischt beide.
 *   GEMESSEN im Original (DuckStation-GDB, ROOM20A0, Spielstand SLUS-00748_6): sechs Zombies mit Deskriptor 6 stehen ab
 *   Takt 2 auf 0x00010101 und gehen aus 13660..18992 Abstand mit 15..32 Einheiten je Takt auf Leon zu; dieselben sechs
 *   mit Deskriptor 0 bleiben 600 Takte stehen (Abstand unveraendert). => "aktiv im Gang" IST Deskriptor 6.
 *
 * WAS DER PORT BIS v0.8.24 TAT: der RE2-INIT (enemy_ai_re2_zombie.c re2z_init) bildet die RE1.5-Deskriptoren ab;
 * nur 0x0d (RE1.5 ENGAGE ab INIT, `ori v0,zero,0x201` / `sw v0,4(v1)` @0x80100f40-44 STAGE1.BIN) wurde Gang, alle
 * anderen aufrechten Formen (0x00 UND 0x02) wurden Stehen.
 *
 * ABBILDUNG (PORT-MAPPING Beta -> Retail, Form RE2-Deskriptor 6): RE1.5 kennt fuer aufrechte Zombies drei Klassen:
 *   0x0d  ENGAGE ab INIT                                           -> RE2 6 (unveraendert, re2z_init sel 0x0d)
 *   0x00  Kampf-Untermodus 0, DECIDE FUN_80101b64 (STAGE1.BIN @0x8011f840[0]): ENGAGE 0x201, sobald Leon im Weitkegel
 *         +-0x5f4 sichtbar ist — OHNE Abstandsgrenze (`jal arc_test(0x5f4)` @0x80101b8c, `sll/bne` @0x80101bf4-f8,
 *         Sicht-Bit `lhu 472 / andi 0x10 / beq` @0x80101c0c-18, `ori 0x201 / sw` @0x80101c1c-20; der einzige
 *         Abstandstest des Engage-Zweigs: bei dist >= 0x2711 (`sltiu` @0x80101c38) ersetzt ein 1/8-Wurf (`jal rng` /
 *         `andi 0x7` @0x80101c44-4c) das 0x201 durch 0x801 (@0x80101c54/64); davor Zaehler +0x9C -> 0x101
 *         (@0x80101ba0-c4) und 0x701 bei dist < 0x7D0 ausserhalb 0x2c8 (@0x80101bd4-f0); der Sichtsensor FUN_8001bc08
 *         -> FUN_8001b84c(0x5e8) prueft nur Kegel + Nibble-3-Strahl @0x8001b87c-8e8) = der AKTIVE Zombie der RE1.5-Daten
 *                                                                  -> RE2 6 (NEU, diese Datei)
 *         Dieselbe Klasse in Stage 4 (ROOM4050: Zombie-Girl Typ 0x13 und Zombies), Binaerdatei STAGE4.BIN (Runde 37
 *         Nachbesserung 1 — hier standen vorher die Adressen der STAGE1.BIN-Kopie des Girl-Codes @0x80120230/0x80120264,
 *         die ROOM4050 nicht laedt): Raumlader @0x800397a4-ec, Stage-Byte 3 / Raum 5 -> @0x8007438c[3] -> Datei 912 =
 *         ROOM4050.RDT (221128 Byte); Overlay-Wechsel @0x80039a30 -> @0x800299a4(2, 3+1) -> Ladesatz @0x80073c0c =
 *         Datei 3 (Dateitabelle @0x8006f454: 0x1A620 Byte = STAGE4.BIN) nach 0x80100000, Installation 0x80118380.
 *         STAGE4.BIN: 0x80072bac[0x13] = 0x8010a87c (`addiu v1,v1,-22404` @0x80118390, `sw v1,0x2bf8(at)` @0x80118478);
 *         Zustand 1 = 0x8010b228 (@0x80119734) -> Modus +0x9 & 0xf ueber @0x80119758 (`lbu v0,9(v0)` @0x8010b648,
 *         `addiu at,at,-26792` @0x8010b65c, `jalr` @0x8010b66c); Modus 0 = 0x8010b688 -> DECIDE @0x8011978c[+0x5]
 *         (`lbu v0,5(v0)` @0x8010b6cc, `addiu at,at,-26740` @0x8010b6dc, `jalr v0` @0x8010b6ec); Eintrag [0] =
 *         0x80101b18 = STAGE1 FUN_80101b64 Wort fuer Wort (70/70 bis `jr ra` @0x80101c28). Die Stage-4-Zombies
 *         (0x80072bac[0x10] = 0x801003d8) erreichen dieselbe 0x80101b18 ueber @0x80118d34[0] = 0x80101640 ->
 *         DECIDE @0x80118d68 (`addiu at,at,-29336` @0x80101660).
 *   0x02  schlafender Steher, DECIDE FUN_80101c7c (STAGE1.BIN @0x8011f960[0]): ENGAGE nur bei dist < 0xFA0
 *                                                                  -> RE2 0 (unveraendert)
 * Die RE2-KI hat fuer aufrechte Zombies genau diese zwei Klassen (0 / 6); die Abbildung legt die RE1.5-Daten-Klasse
 * "aktiv" auf die RE2-Klasse "aktiv". Wirkung: Stage 1 (Leon/Elza) 1010/1020/1070/1100/1180/11E0/11F0/1250 (+ Elza-
 * Zwillinge), Stage 4 4050 — ihre grid-0x00-Zombies gehen ab dem Spawn auf den Spieler zu; Schlaefer (0x02:
 * 1090/10B0/10D0/10E0/1140-Wiederkehr/1220 ...), Liegende, Fresser, Kriecher bleiben wie sie sind.
 *
 * WAEHREND EINES MONTAGE-SCHRITTS (re15_irons_tod.h, Zustand != RE15_IT_AUS) bleibt es beim Stehen: der Spieler steht
 * dann unsichtbar auf einem Parkplatz hinter der Kamera, ein gehender Zombie wuerde ihn angreifen (Riegel
 * integration_r35_cut1150 "Spieler-HP 100 -> min 100"). Beim spaeteren Betreten (Nachspawn) gilt die Abbildung.
 *
 * Die ABBILDUNG wirkt nur unter der RE2-KI (re2z_init laeuft nur fuer re15_ai_re2_for_type + re15_re2z_owns_type);
 * unter RE1.5-KI und MIXED (Zombies dort RE1.5-Gehirn) bleiben grid-0x00-Zombies in ihrer RE1.5-Klasse. Der Nachspawn
 * 1130 in Durchbruch-Form (irons_tod_1150.c EM_1130_NACH, grid 0x0d) gilt dagegen fuer JEDE KI-Wahl: unter der
 * RE1.5-KI ist 0x0d der RE1.5-Angriff ab INIT (@0x80100f40-44) — gemessen RE15_AI_FLAVOR=re15, Tuer 1150 -> 1130:
 * alle fuenf ab Bild 6 in 0x201 und in 400 Bildern 2511..8610 naeher; v0.8.24 (grid 0x02) Stehen, Weg 0..18.
 */
#ifndef RE15_ZOMBIEAKTIV_R37_H
#define RE15_ZOMBIEAKTIV_R37_H

#include <stdint.h>
#include "re15_actor.h"

/* RE2-Zustandswoerter des INIT (EMZ0.BIN) */
#define RE15_R37_RE2_WORT_STEHEN  0x0001u   /* `addiu v0,zero,1` / `sw v0,4(s2)` @0x801006ac-b8 */
#define RE15_R37_RE2_WORT_GANG    0x0101u   /* `addiu v0,zero,257` / `sw v0,4(s2)` @0x80100ab8-abc (Deskriptor 6) */

/* RE1.5-Deskriptor (Sce_em_set pc[3] -> +0x9) der aktiven Klasse: Kampf-Untermodus 0 (@0x8011f80c[0] = FUN_8010168c,
 * DECIDE @0x8011f840[0] = FUN_80101b64). */
#define RE15_R37_RE15_AKTIV       0x00u

/* HAKEN scd_vm.c op_sce_em_set (nach dem Ablegen des Aktors): merkt sich fuer diesen Aktor-Platz, ob der Spawn-Satz
 * die RE1.5-Form eines aktiven Zombies traegt. Jeder Sce_em_set in den Platz ueberschreibt die Marke. */
void re15_r37_em_set(int actor_slot, const re15_actor_t *a, uint8_t behavior);

/* HAKEN enemy_ai_re2_zombie.c re2z_init (letzter Zweig, statt 0x0001): 1 = dieser Zombie bekommt RE2-Deskriptor 6
 * (Zustandswort RE15_R37_RE2_WORT_GANG). Nur fuer genau den Aktor, der mit dieser Marke gespawnt wurde (Typ und
 * Spawn-Lage gleich — der INIT laeuft im ersten KI-Takt, vor jeder Bewegung); die Marke wird verbraucht. */
int  re15_r37_re2_gang(int slot, const re15_actor_t *e);

/* HAKEN enemy_ai_re2_zombie.c re15_re2z_rng_reset (Raum-Ladekette, VOR dem Init-Lauf der Sce_em_set; auch aus
 * re15_actor_init): alle Marken verwerfen — ihre Lebensdauer ist die des Aktor-Feldes. */
void re15_r37_marken_loeschen(void);

/* Nur fuer Sonden/Riegel: Abbildung und den Nachspawn 1130 in aktiver Form abschalten (Gegenprobe = Stand v0.8.24). */
void re15_r37_zombieaktiv_enable(int an);
int  re15_r37_zombieaktiv_enabled(void);

#endif /* RE15_ZOMBIEAKTIV_R37_H */
