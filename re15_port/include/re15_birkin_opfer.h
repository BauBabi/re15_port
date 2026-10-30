/*
 * RE1.5 Rebuilt — Spieler-Seite der Birkin-Treffer (Typ 0x30): der Spieler-Haken cmd 5/6.
 *
 * Registrierung (STAGE5.BIN, RAW @0x80100000): `sw a0,0x800ac818` @0x8011de84 (cmd-5-Tabelle 0x800ac758 + 0x30*4)
 * und `sw a0,0x800ac918` @0x8011defc (cmd-6-Tabelle 0x800ac858 + 0x30*4), beide a0 = 0x8011afd8. Der Haken
 * verteilt nach aca59 ueber @0x80120468 = {0x8011b018, 0x8011b40c, 0x8011b6e0, 0x8011b9bc}:
 *   [0] Kralle von VORN  (aca58 = 0x005 @0x801177d0 STAGE3 / Wurf aca58 = 6 @0x80117ee4 STAGE3 in STAGE5)
 *   [1] Kralle von HINTEN (aca58 = 0x105, aca59 = FUN_8001a780 = 1)
 *   [2] Biss              (aca58 = 0x205 @0x80117c1c STAGE3, nur Bild < 0x26)
 * STAGE3 traegt denselben Haken @0x8011a7c4 (633 Wort instruktionsgleich, nur lui/addiu/j/jal verschoben),
 * registriert cmd 6 aber auf den Stub 0x8011b168 -> Wurf-Tod-Maschine 0x8011b1a8 (Opfer-Clip 1, Ende cmd 7) =
 * Port-Variante 3; ihr Blut-/Koerperteil-Block haengt am Trefferpunkt der Angriffsbox FUN_8001a5e0 (OFFEN).
 */
#ifndef RE15_BIRKIN_OPFER_H
#define RE15_BIRKIN_OPFER_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy.h"

/* Treffer-Schreibsatz der Birkin-KI: aca58 = (aca59 << 8) | cmd, acbfc = Birkin, acbcc/acbd0 = Birkin+0x178/+0x17c.
 * Liefert 0, wenn der Haken fuer cmd/Stage nicht gebaut ist (Aufrufer behaelt dann den alten Pin). */
int  re15_bk_opfer_start(const re15_actor_t *birkin, uint8_t cmd, uint8_t variante);
int  re15_bk_opfer_cmd(void);        /* aca58-Byte (5/6) solange der Haken laeuft, sonst 0 */
int  re15_bk_opfer_eigenbank(void);  /* 1 = die laufende Phase spielt Leons eigene Bank (acad8/acbc0) */
int  re15_bk_opfer_phase(void);      /* aca5a (Tests), 0xff = aus */
int  re15_bk_opfer_variante(void);   /* aca59 (Tests) */
void re15_bk_opfer_reset(void);
/* Ein Haken-Aufruf (Spieler-Dispatcher @0x80031cb4); vb = Opfer-Bank des Greifers (EM30 dir[5]/[6]). */
void re15_bk_opfer_tick(re15_actor_t *pl, const re15_enemy_bank_t *vb);

#endif
