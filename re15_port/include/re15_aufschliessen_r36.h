#ifndef RE15_AUFSCHLIESSEN_R36_H
#define RE15_AUFSCHLIESSEN_R36_H

/* ============================================================================
 * Runde 36 Spur G (Punkt 3): RE2-TONFOLGE beim ERFOLGREICHEN Aufschliessen einer Tuer
 * ⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL — Dossier analysis/befunde_runde36/G_tueren.md (G3)
 * ============================================================================
 * Nutzer: "Türen die erfolgreich geöffnet wurden mit Karte oder sowas machen in Resident Evil 2 erst ein
 * "piep" Sound und dann ein unlock Geräusch. Bei uns nicht."
 *
 * RE1.5 ist beim Aufschliessen stumm: in keinem der 29 Aufschliess-Bloecke (gen/r36_aufschliessen.inc,
 * tools/gen_r36_aufschliessen.py) steht ein Se_on, z.B. ROOM10D0 sub19 @Datei 0x01988..0x0199E
 * (Aot_reset Platz 2 sce 2 + Message_on 5 "You've opened the lock.").
 *
 * RE2-VORBILD (selbst gelesen; Walker analysis/befunde_runde30/tools/r30_re2_scd.py):
 *   Kartenleser-Tuer ROOM2110.RDT sub08 (gerufen aus sub06 @0x01AEC nach Sce_Item_lost @0x01AEA):
 *     @0x01B4C Se_on(2,0x0C)   Karte ein
 *     @0x01B58 Sleep 15
 *     @0x01B5C Se_on(2,0x0D)   PIEP  (stehender Ton 1450 Hz)
 *     @0x01B8C Sleep 15
 *     @0x01B90 Se_on(2,0x0B)   AUFSCHLIESSEN
 *   Riegel von dieser Seite (Tuer-Handler 0x80051514, key_type 0xFE): @0x800515ec jal 0x8002fe38 (Text 10
 *     "You unlocked it.") und @0x800515f4/@0x800515f8 lui a0,0x226 / jal 0x8005ba28 = Raumbank-Satz 0x26
 *     im SELBEN Bild.
 *   Se_on 0x36 @0x80056428 -> jal 0x8005ba28 @0x80056530 (a0 = Bank<<24 | Satz<<16); Bank 2 = Raumbank
 *   (FUN_80059e54 @0x80059f44). Die vier Wellen liegen als Mini-Bank shared_assets/RE2/KARTSE.VBS
 *   (tools/re2_karten_se_cut.py, Satz-TOC gen/re2_karten_bank.inc).
 *
 * Einhaengepunkt: dieselben zwei Stellen wie der "Tuer verschlossen"-Ton (include/re15_lock_se.h) —
 * scd_vm.c re15_scd_show_message (Text-AOT) und op_message_on (Skript), unmittelbar vor dem Oeffnen der
 * Nachricht. Die Folge laeuft in Spielbildern (30 Hz wie RE2s Skript-Sleep) ueber re15_aufschliessen_r36_tick
 * (game_step_common.c, jedes Bild, auch bei offener Nachricht).
 * ==========================================================================*/

#include <stdint.h>

#define RE15_AUF_WEG_AOT    1
#define RE15_AUF_WEG_SKRIPT 2

/* Nachricht (room_id, msg_id) geht auf diesem Weg auf -> startet die Folge, wenn die Stelle in der
 * Tabelle steht. Rueckgabe: Art (RE15_AUF_ART_*) oder -1. */
int  re15_aufschliessen_r36_notice(unsigned room_id, uint8_t msg_id, int weg);
/* Ein Spielbild: faellige Toene der laufenden Folge spielen. */
void re15_aufschliessen_r36_tick(void);

/* Die Plattform meldet ihren Spieler fuer Saetze der Mini-Bank KARTSE.VBS an (PC: audio_pc.c
 * re15_audio_re2_karte_se via platform/pc/src/tuer_r36_pc.c). Ohne Anmeldung (PSX, Tests) wird nur gezaehlt. */
typedef void (*re15_auf_spieler_t)(int satz);
void re15_aufschliessen_r36_setze_spieler(re15_auf_spieler_t f);

/* Tabellen-/Messzugriff (Riegel, Sonden). */
int  re15_aufschliessen_r36_stellen(void);
int  re15_aufschliessen_r36_stelle(int i, unsigned *room, int *msg, int *art, int *wege);
/* Folge einer Art: n Eintraege (Satz der Mini-Bank, Bild-Versatz); Rueckgabe n (<= 3). */
int  re15_aufschliessen_r36_folge(int art, int *satz, int *bild, int max);
extern unsigned g_re15_auf_zaehler;      /* gespielte Toene seit Start */
extern int      g_re15_auf_letzter;      /* zuletzt gespielter Satz, -1 = keiner */

/* Satz-TOC der Mini-Bank (gen/re2_karten_bank.inc) fuer den PC-Lader. */
typedef struct {
    unsigned edt_off, edt_size, vbd_off, vbd_size;
    int      se_count;
} re15_karte_bank_rec_t;
void re15_karte_bank_rec(re15_karte_bank_rec_t *out);

#endif /* RE15_AUFSCHLIESSEN_R36_H */
