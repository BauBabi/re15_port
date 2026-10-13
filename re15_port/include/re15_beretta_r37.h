/*
 * re15_beretta_r37.h — Runde 37 Spur G: der 3-Schuss-Feuerstoss der Beretta M93R (Waffe 5) und der
 * Glock 18 (Waffe 6) trifft JE KUGEL.
 *
 * Dossier: analysis/befunde_runde37/G_beretta.md (Messungen, Belege, Tests).
 *
 * NUTZER-VORGABE (AUFTRAG.md Nachtrag 2026-10-12, woertlich): "bei der Beretta M93R muss - wenn sie
 * schon mehrfach schießt, das auch gelten das die Gegner mehrfach getroffen werden. Werden sie nur
 * ein einziges mal getroffen bei 3 Schuss, ist es nur Schussverschwendung."
 *
 * RE1.5 (Auslieferungsstand) trifft je Feuerstoss EINMAL — statisch und am Original gemessen:
 *   Entlade-Handler 0x800338A8 (Tabelle @0x80074114/18): drei FX-Ketten Sub 2 (@0x800338ac/f4/28),
 *   EIN `jal 0x80011f50` @0x8003396c, DREI `jal 0x8004eae4` @0x80033974/7c/84; der Handler laeuft
 *   je Feuerstoss einmal (Latch 0x800aca5b @0x80033470-80, `jalr` @0x800334e0).
 *   DuckStation-GDB (equip_test.sav, Auslieferungs-EXE @0x80026e4c = 0800e003): Resolver 1x,
 *   Munition 15 -> 12, Kinder der FX-Kette in den Bildern B41/B44/B47 (CORE00.ESP Effekt 2/3/4
 *   Sub 2 = je drei Zeilen Routine 15 mit Warte +0x0E = 0/2/2 @Datei 0x1188/0x11B0/0x11D8 —
 *   Routine 15 @0x80017ac8: Warte != 0 -> -1 und zurueck @0x80017af4-b0c, sonst Kind @0x80017b38).
 *   => drei Blitze, drei Knaelle, drei Huelsen, EIN Treffer.
 *
 * FORM (Beta -> Retail, RE2 Custom Handgun = RE2-Item 4, eigene FSM 0x8004879C @0x800A6F48):
 *   Sub 2 0x800487D8 feuert eine KUGEL in den Feuerclip-Bildern 0/3/6 (`beq v1,zero` @0x80048910,
 *   `addiu v0,zero,3` / `beq` @0x80048914-18, `addiu v0,zero,6` / `bne` @0x8004891c-20; Bildzaehler +0x14D
 *   steigt je Bild um 1: `lbu v0,333(s2)` / `addiu v0,v0,1` / `sb v0,333(s2)` @0x80029b28-34); Kugel 2/3
 *   nur mit Patrone (`jal 0x80069f54` / `bne v0,zero` @0x80048938-40). Je Kugel: Treffer-Skript neu
 *   (`jal 0x80041098` @0x80048990), EINE Patrone (`jal 0x8006a0cc` @0x800489a0); der Resolver
 *   FUN_800410CC laeuft jedes Bild (@0x80048a04) und loest nach jedem Skript-Neustart genau EIN Bild
 *   auf (Muster @0x800A666C = `00 01 00 ff`). Schaden je Kugel: RE2-Zeile 4 == Zeile 3 (Handgun) in
 *   20 von 22 Gegnertabellen (Zombie @0x800A4168 w0 = @0x800A4154 w0 = 0x00E03C10); nur die
 *   Trefferpause ist kuerzer (Zombie w1 0x00810214 -> 1 Bild statt 0x02850A14 -> 5 Bilder), damit
 *   die naechste Kugel 3 Bilder spaeter wieder trifft.
 *
 * PORT-ABBILDUNG (diese Datei):
 *   * Kugel 1 = der bisherige Resolve im Eintrittsbild (@0x8003396c, unveraendert).
 *   * Kugel k (k = 2, 3) im Bild 3*(k-1) nach dem Eintritt — derselbe Abstand wie die RE1.5-FX-Kette
 *     (Kinder T, T+3, T+6) und die RE2-Kugelbilder 0/3/6. Je Kugel derselbe Resolver
 *     re15_player_weapon_fire(5|6) (Sichtlinie, Gegnerwahl, Schaden, Reaktion, Blut).
 *   * Kugel k nur, wenn das Eintrittsbild fuer sie eine Patrone abgebucht hat (RE1.5 bucht alle
 *     drei sofort ab, `jal 0x8004eae4` x3 ungeprueft; RE2 prueft je Kugel @0x80048938).
 *   * Der Feuerstoss endet mit dem Rueckstoss (Treffer/Griff/Raumwechsel loeschen ihn,
 *     re15_player_aim_interrupt / re15_player_aim_reset) oder einem Waffenwechsel.
 *   * RE1.5-Riegel +0x93 Bit 0 (`ori v0,v0,0x1` @0x800124F0, Freigabe erst am Ende der Trefferreaktion
 *     z.B. @0x80105f9c-fac, Kakerlake `sb zero,147(a0)` STAGE3.BIN @0x801149ac): fuer jeden Gegner, fuer den
 *     KEIN RE2-Pausenfilter laeuft, loest Kugel 2/3 den Riegel, den DIESER Feuerstoss gesetzt hat (Bits 0|1),
 *     solange der Gegner lebt (HP >= 0) — RE2-Gegenstueck: die 1-Bild-Pause der Zeile 4.
 *     Massgeblich ist der FILTER, nicht der KI-Modus (Nachbesserung 1, Abnahme 0 M1): ein RE2-Pausenfilter
 *     (+0x1D3, je Bild im Nachlauf von game_step) laeuft nur fuer die Zombie-Familie (re15_re2z_owns_type ->
 *     re15_re2z_hit_filter_apply) und 0x20/0x21/0x25/0x26 (re15_re2_pause_filter_apply), jeweils nur mit
 *     RE2-Gehirn (re15_ai_re2_for_type). Nur diese behalten ihre RE2-Regel (Zeile 4: Zombie 1 Bild, Hund/
 *     Spinne 0, Kraehe 15). Die RE1.5-Gehirne 0x23/0x27/0x29/0x2b/0x30 (und 0x36, 0x1a) werden in JEDEM
 *     KI-Modus wie unter RE1.5-KI behandelt — der Port stempelt ihnen keine Pause (re15_re2_stun_frames
 *     `return 0`) und hat fuer sie keinen Filter.
 *   * Schaden je Kugel (re15_damage.c): Spalte 3 der RE1.5-Zeile (Einzelschuss-Pistole) — die
 *     RE1.5-Spalte 5/6 ist der Schaden des GANZEN Feuerstosses (Zombie 0x10..0x14: 5 / 15 = 3 x 5,
 *     @0x8006e0d0 + Typ*0x58 + 3*4 / + 5*4). In den RE2-Zeilen ist Spalte 5 == Spalte 3.
 */
#ifndef RE15_BERETTA_R37_H
#define RE15_BERETTA_R37_H

#include <stdint.h>

/* 1 fuer die Feuerstoss-Waffen 5 (Beretta M93R) und 6 (Glock 18): beide zeigen auf den Entlade-
 * Handler 0x800338A8 (@0x80074114 / @0x80074118). */
int re15_beretta_r37_ist_feuerstoss(int waffe);

/* Eintrittsbild des Feuerstosses (game_step, direkt nach Resolve + Munitionsabbuchung):
 * patronen = Zahl der wirklich abgebuchten Patronen (0..3), treffer1 = Rueckgabe des Resolvers fuer
 * Kugel 1 (Platz+1 oder 0). Ohne Wirkung fuer andere Waffen. */
void re15_beretta_r37_feuerstoss(int waffe, int patronen, int treffer1);

/* Je Spielbild (game_step, vor dem Feuerpfad): zaehlt das Bild und loest im Bild 3 / 6 die Kugel 2 / 3
 * auf. Gibt die Rueckgabe des Resolvers zurueck (Platz+1), 0 wenn in diesem Bild nichts aufgeloest
 * wurde. re15_beretta_r37_tick_ex nimmt die Bedingung "Feuerstoss laeuft noch" als Argument (Sonden). */
int re15_beretta_r37_tick(void);
int re15_beretta_r37_tick_ex(int noch_im_feuer);

/* 1, wenn fuer diesen Gegnertyp im aktuellen KI-Modus ein RE2-Pausenfilter den Riegel +0x93 freigibt
 * (Zombie-Familie bzw. 0x20/0x21/0x25/0x26 mit RE2-Gehirn) — dann loest der Feuerstoss den Riegel NICHT.
 * Fuer Sonden/Tests (Nachbesserung 1, Abnahme 0 M1/M3). */
int re15_beretta_r37_re2_pause_regelt(unsigned type);

/* Offene Kugeln des laufenden Feuerstosses (0 = keiner aktiv). */
int re15_beretta_r37_offen(void);

/* Mess-Zaehler (Sonden/Tests): Feuerstoesse, aufgeloeste Kugeln, Treffer (Resolver != 0),
 * abgebrochene Feuerstoesse, geloeste RE1.5-Riegel. */
typedef struct {
    uint32_t feuerstoesse, kugeln, treffer, abgebrochen, riegel_geloest;
} re15_beretta_r37_mess_t;
void re15_beretta_r37_mess(re15_beretta_r37_mess_t *out);
void re15_beretta_r37_reset(void);

#endif /* RE15_BERETTA_R37_H */
