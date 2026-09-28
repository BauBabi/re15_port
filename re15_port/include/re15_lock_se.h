#ifndef RE15_LOCK_SE_H
#define RE15_LOCK_SE_H

/* ============================================================================
 * "TUER VERSCHLOSSEN"-TON — ⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL
 * ============================================================================
 * Dossier: analysis/befunde_runde30/tuer-verschlossen.md (Runde 30, Befund A).
 *
 * WARUM RE1.5 HIER NICHT MASSGEBLICH IST: RE1.5 hat an verschlossenen Tueren KEIN
 * Ton-System (Beta, unfertig -> Memory reai-v2-beta-zu-retail: RE2 Retail ist das Ziel).
 *   Text-Handler PTR_8007469c[1] = 0x80043084: einziger Aufruf @0x800430a0
 *     `jal 0x80027e68` (Nachricht oeffnen) — kein SE.
 *   Tuer-Handler PTR_8007469c[2] = 0x800430bc: @0x800430c4 `sw a0,-13912(at)` Nutzlast,
 *     @0x800430d4 `sb v0,21337(at)` Uebergang = 1 — kein Schloss-Test, kein SE.
 *   Zwischen 0x80041730 (SCD-Se_on) und 0x8004a158 (Inventar) liegt keine der 41
 *     `jal 0x80045024`-Aufrufstellen der EXE: der ganze AOT-Scan ist stumm.
 *
 * RE2-VORBILD (RE2 PSX.EXE md5 09a9b642..., selbst disassembliert):
 *   Tuer-Handler PTR_800a73c4[1] = 0x80051514
 *     @0x800515a8 lbu a1,15(s2) key_id ; @0x800515b0 andi v0,a1,0x80 = verschlossen
 *     @0x8005160c lui a0,0x216 / @0x80051610 jal 0x8005ba28 / @0x80051614 addu a1,zero,zero
 *        (key_type 0xFF, von innen verriegelt) -> Text @0x800516b8 jal 0x8002fe38
 *     @0x800516a0 lui a0,0x216 / @0x800516a4 jal 0x8005ba28 / @0x800516a8 addu a1,zero,zero
 *        (Schluessel fehlt)                    -> Text @0x800516b8 jal 0x8002fe38
 *   0x8005ba28 = SE-Spieler: @0x8005ba30 srl t1,a0,24 (Bank 2), @0x8005ba7c/80 Satz 0x16;
 *     Bank 2 = Raumbank (FUN_80059e54 @0x80059f44 sw -> 0x800dbb80 = [RDT+0x08]).
 *   a1 = 0 -> NICHT positional; Ton und Text im SELBEN Bild.
 *   Raumskripte spielen denselben Satz, z.B. ROOM2110.RDT sub09: @Datei 0x01BB6
 *     Message_on 6 ("It's electronically locked. There's a card reader on the left."),
 *     @Datei 0x01BBC Se_on `36 02 16 00 00 00 44 a4 f8 f8 93 cc`.
 *
 * EINHAENGEPUNKT IM PORT: das Oeffnen der Schloss-Nachricht (RE1.5 kennt kein
 * key_id-Feld; das Schloss ist Datenwahl sce=1 Text statt sce=2 Tuer). Die Stellen
 * (Raum, Nachricht) -> Art liefert engine/src/gen/lock_se_sites.inc
 * (tools/gen_lock_se_sites.py, 52 Stellen: 17 x K, 35 x M; jede mit Datei-Offset).
 * RE1.5s EIGENER Schloss-Ton ROOM4000 sub02 @Datei 0x0142E Se_on(2,0x0f) bleibt RE1.5:
 * dieser Skript-Weg steht NICHT in der Tabelle (Generator-Bedingung C).
 * ==========================================================================*/

#include <stdint.h>

/* Weg, auf dem die Nachricht aufgeht (Bitmaske wie Spalte "wege" der Tabelle). */
#define RE15_LOCK_WEG_AOT    1   /* Text-Platz sce 1 -> re15_scd_show_message
                                  * (RE2-Gegenstueck: EXE @0x80051610 / @0x800516a4) */
#define RE15_LOCK_WEG_SKRIPT 2   /* SCD Message_on -> op_message_on
                                  * (RE2-Gegenstueck: ROOM2110 sub09 @Datei 0x01BBC) */

/* Art der Stelle — muss gleich RE15_LOCK_ART_K / _M aus gen/lock_se_sites.inc sein
 * (lock_se_common.c prueft das beim Uebersetzen). */
#define RE15_LOCK_SE_ART_K 0     /* elektronisch / Kartenleser / Ausweis / Pincode */
#define RE15_LOCK_SE_ART_M 1     /* mechanisch / von der anderen Seite             */

/* An der Stelle des tatsaechlichen Oeffnens gerufen. Steht (room_id, msg_id) in der
 * Tabelle und ist `weg` fuer diese Zeile gesetzt, faellt EIN Tuer-Ton (Satz der Art),
 * sonst passiert nichts. Rueckgabe: gespielter Satz der Mini-Bank oder -1. */
int  re15_lock_se_notice(unsigned room_id, uint8_t msg_id, int weg);

/* Satz der Mini-Bank TUERSE.VBS fuer eine Art (RE15_LOCK_SE_ART_*), -1 bei unbekannt. */
int  re15_lock_se_satz_fuer_art(int art);

/* Tabellen-Zugriff fuer Riegel und Sonden. */
int  re15_lock_se_site_count(void);
int  re15_lock_se_site(int i, unsigned *out_room, int *out_msg, int *out_art, int *out_wege);

/* Messhaken (keine Spiellogik): Zahl der ausgeloesten Toene und zuletzt gespielter Satz. */
extern unsigned g_re15_lock_se_zaehler;
extern int      g_re15_lock_se_letzter;

/* Satz-TOC der Mini-Bank shared_assets/RE2/TUERSE.VBS — Muster re15_elev_bank_rec:
 * der Satz steht in der Datei, seine Groessen im Code (gen/re2_door_bank.inc,
 * erzeugt von tools/re2_door_se_cut.py). */
typedef struct {
    unsigned edt_off, edt_size;   /* SE-Map @+0, VH @u32[edt_size-8] */
    unsigned vbd_off, vbd_size;   /* VB-Rumpf                        */
    int      se_count;            /* Saetze in der Mini-Bank         */
    int      se_zu_a, se_zu_b, se_zu_e;
} re15_door_bank_rec_t;
void re15_door_bank_rec(re15_door_bank_rec_t *out);

#endif /* RE15_LOCK_SE_H */
