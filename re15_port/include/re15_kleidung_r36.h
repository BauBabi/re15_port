/* re15_kleidung_r36.h — Runde 36 Spur B "kleidung": Leons zerrissene/blutige Kleidung unter JEDEM
 * Gegner-Gehirn (RE1.5 und RE2). Dossier analysis/befunde_runde36/B_kleidung.md.
 *
 * Das Blut-Decal-Wundsystem (FUN_80037edc @0x80037edc, Tabelle @0x800b10ec, analysis/blood_decals.md)
 * ist ein RE1.5-Merkmal, das RE2 Retail nicht hat. Im Original haengt der Griff-Stempel NICHT am
 * Gegner, sondern an der SPIELER-Opfer-FSM (cmd 5, 0x8010a2cc, Phase 4 @0x8010a4e8): der Greifer
 * schreibt beim ueberlebten Griff aca5a := 4, der Phase-4-Handler stempelt ueber
 * `lbu aca59 / jalr 0x801201b8[aca59]` (@0x8010a580-5b0). Der Port rief den Helper bisher nur im
 * RE1.5-ZOMBIE-Code (THROW-OFF [4]); unter dem RE2-Standardgehirn (seit e11962ec, 2026-08-22) lief
 * deshalb nie ein Stempel. Dieses Modul traegt die Phase-4-Reaktion und die beiden RE1.5-Stempel-
 * Formen fuer Hund und Kraehe, damit die RE2-Gehirne dieselben Wunden erzeugen (PORT-MAPPING:
 * RE2-Ereignis -> RE1.5-Form, jede Zahl mit ihrer RE1.5-Adresse). */
#ifndef RE15_KLEIDUNG_R36_H
#define RE15_KLEIDUNG_R36_H

#include <stdint.h>
#include "re15_actor.h"

/* Generische Hook-A-Typen (STAGE1 FUN_8011e864 @0x8011e9c0-0x8011eac8, player_hit_chain.md §1):
 * 0x10,0x11,0x12,0x16,0x18,0x1c..0x1f -> LAB_8010a28c; 0x13 Zombie-Girl -> LAB_8010c16c
 * (Trampolin in dieselbe FSM 0x8010a2cc). 0x1A (RE2-Gitterarm, Opfer-Spender 0x10) = PORT-WAHL
 * (im RE1.5-Overlay ohne Griff). Hund 0x20 / Kraehe 0x21 / Gorilla 0x27 haben EIGENE Hooks. */
int  re15_kleidung_generischer_griff(uint8_t typ);

/* Phase 4 der Opfer-FSM @0x8010a4e8 (laeuft genau EINMAL je ueberlebtem Griff, aca5a:=4 -> 5):
 *   Release-Blut `jal 0x80019700` @0x8010a574, a0=0x1500: dir<2 Spieler-Part 8 (+0x5A0
 *   @0x8010a570), dir>=2 Part 3 (+0x244 @0x8010a54c); danach Wund-Helper 0x801201b8[dir]:
 *   0 = 0x8010a1cc (0,+10)(5,+50)(7,+50) | 1 = 0x8010a208 (0,+10)(4,+50)(7,+50) |
 *   2 = 0x8010a244 (1,+50) | 3 = 0x8010a268 (2,+50).
 * dir = Griff-Richtung (aca59): 0 vorn / 1 hinten / 2,3 Kriecher. Nur fuer generische Typen. */
void re15_kleidung_abwurf(uint8_t greifer_typ, int dir);

/* RE1.5-Hundebiss-Stempel (Biss-Connect FUN_8010f15c @0x8010f33c-60): panel =
 * byte[0x80121010 + aca59*2 + (rng&1)] mit aca59 = a780(Spieler)+2, Bytes 05 07 04 06
 * @0x80121014 -> Spieler gleichgerichtet (Ruecken zum Hund) 4/6, sonst 5/7; Betrag 0x32
 * (@0x8010f35c), rng = FUN_8001af20 @0x8010f334. Nur beim lebenden, nicht gegriffenen Spieler
 * (@0x8010f2dc/e4). PORT-MAPPING auf den RE2-Boden-Biss (@0x80104F7C-88 -> 0x801051BC(0,dir)). */
void re15_kleidung_hundebiss(const re15_actor_t *hund, const re15_actor_t *spieler);

/* RE1.5-Kraehen-Stempel 0x801161e8 (vert = Spieler.y - Kraehe.y, +0x1ec @0x801124cc/@0x80114f40):
 *   <1500 (1,+0xa)(2,+0xa) | 1500..2999 (3,+0xa)(4,+0x28)(6,+0x28)(5,+0x28) |
 *   3000..3599 (0,+0x8)(7,+0x28) | >=3600 nichts (@0x8011620c-70, beq @0x80116258).
 * RE1.5-Aufrufe: Dive-Treffer @0x80113b7c, Griff-Eintritt @0x80113ef4, Strike @0x801144bc.
 * PORT-MAPPING auf den RE2-Kraehen-Griff (P0 geclaimt @0x80102550-58 -> Opfer-FSM). */
void re15_kleidung_kraehe(const re15_actor_t *kraehe, const re15_actor_t *spieler);

/* Panel-Ziel auf der Spieler-TIM (8bpp-Texel-Koordinaten, Zeile 0..255) aus der LUT @0x80074208
 * (8 Zeilen, Index = Charakter-Byte DAT_800aca5c: Leon 0, Elza 4). 0 = ungueltig. */
int  re15_kleidung_panel_ziel(int charakter, int panel, int *tim_x, int *tim_y);

#endif /* RE15_KLEIDUNG_R36_H */
