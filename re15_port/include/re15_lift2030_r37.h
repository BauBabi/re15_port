/* re15_lift2030_r37.h — Runde 37 Spur C Nachbesserung 1 (AUFTRAG.md Nachtrag 5b "in den Stage2 kleineres aufzügen
 * fehlt noch das sound. Den sollte man auch aus Resident Evil 2 extrahieren."). Dossier
 * analysis/befunde_runde37/C_fahrstuhl.md "Nachbesserung 1".
 *
 * RAEUME: die einzigen Aufzuege der STAGE2 sind die zwei Hubplattformen in ROOM2030/2031 "TREATMENT POOL"
 * (msg 1 @0x2038 "The elevator is not responding...", Strom aus ROOM2060): Lift A = obj 1 (6725,y,12050),
 * Fahrt sub02 + sub03; Lift B = obj 2 (-10675,y,4050), Fahrt sub04 + sub05. ROOM2031 = ROOM2030 (SCD bytegleich).
 *
 * ⛔ RE2-ERGAENZUNG (Beta -> Retail, Sound ist RE2): RE1.5 ist stumm (kein Se_on in ROOM2030/2031). Vorbild ist die
 * RE2-Hubplattform ROOM6030 ("It's a switch for the lift", msg 9) — dieselbe Art (Plattform + Spieler fahren per
 * Skript senkrecht, 5400 Einheiten): sub10 @0x02F8C Set(2,7)=1, @0x02F90 Se_on(2,0x11) KLICK, @0x02FBA Se_on(2,0x0B)
 * FAHRT unmittelbar vor der Fahrt-Schleife @0x02FDC For 180 (sub12 @0x030B6/@0x030BA/@0x030E4/@0x03106 gleich).
 * Die Wellen liegen als Mini-Bank shared_assets/RE2/LIFT6030.VBS vor (tools/fahrstuhl_r37/re2_lift6030_cut.py).
 *
 * ANKER = "Port-Zeile vor einem Original-Opcode" wie elza_r36.c / scd_elev_se.c: RE1.5 fuehrt dieselbe Folge
 * (Set(2,7)=1 am Ereignisbeginn, dann die Fahrt-Schleife) ohne Ton; der Port spielt den RE2-Satz, wenn der VM-Takt
 * den Anker-Opcode ausfuehrt. Anker = Datei-Offset + 8-Byte-Signatur im geladenen RDT-Puffer (ROOM2030 = ROOM2031).
 */
#ifndef RE15_LIFT2030_R37_H
#define RE15_LIFT2030_R37_H

#include <stdint.h>

/* Saetze der Mini-Bank LIFT6030.VBS (= gen/re2_lift6030_bank.inc RE2_LIFT_SE_*). */
#define RE15_LIFT_TON_KLICK       0   /* RE2 ROOM6030 Satz 0x11 (@0x02F90) */
#define RE15_LIFT_TON_FAHRT       1   /* RE2 ROOM6030 Satz 0x0B (@0x02FBA), 180-Bilder-Fahrt (Lift B)        */
#define RE15_LIFT_TON_FAHRT_KURZ  2   /* Satz 0x0B um 90 Bilder gekuerzt (Lift A faehrt 90 Bilder, PORT-WAHL)  */
#define RE15_LIFT_TON_N           3

/* Plattform-Haken (PC: platform/pc/src/lift2030_se_pc.c; PSX/Tests: NULL = stumm). */
extern void (*re15_lift2030_ton_hook)(int satz);

/* Anker-Zahl des aktuellen Raums (Wachposten im VM-Takt: ausserhalb ROOM2030/2031 0). */
extern int g_re15_lift2030_anker_n;

/* Je Raum einmal: Anker im rohen RDT-Puffer armieren (scd_register_current_rdt). */
void re15_lift2030_r37_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id);
/* Je ausgefuehrtem Opcode (scd_vm.c, Wachposten g_re15_lift2030_anker_n). */
void re15_lift2030_r37_pc(const unsigned char *pc);

/* Messschiene fuer Riegel: Zahl der gespielten Saetze, VM-Takt (g_scd.tick_count) des letzten Rufs, Datei-Offset
 * des letzten Ankers; Tabelle der Anker (Raum ohne Variante, Datei-Offset, Satz). */
int      re15_lift2030_r37_zaehler(int satz);
int      re15_lift2030_r37_letzter_takt(int satz);
unsigned re15_lift2030_r37_letzter_anker(void);
void     re15_lift2030_r37_reset(void);
int      re15_lift2030_r37_anzahl(void);
int      re15_lift2030_r37_anker(int i, uint16_t *raum, uint32_t *off, int *satz);

#endif
