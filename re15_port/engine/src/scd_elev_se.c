/* ============================================================================
 * scd_elev_se.c — FAHRSTUHL-FAHRTON
 * ⛔ DAS IST EINE RE2-ERGAENZUNG. RE1.5 IST HIER STUMM. NICHT byte-true RE1.5.
 * ============================================================================
 *
 * 1. WAS RE1.5 TUT (gemessen, nicht modelliert)
 * --------------------------------------------
 * Die Fahrt in ROOM1080/ROOM1081 (STAGE1 "ELEVATOR") und ROOM4020/ROOM4021
 * (STAGE4 "A-2 ELEVATOR") ist dieses Skriptstueck (ROOM1080.RDT @0x746, roh
 * gelesen — dieselben Bytes nochmal @0x7D8 und @0x86A, je eine Etage):
 *
 *   22 01 1c 01   Set   bank1 bit28 = 1
 *   09 0a 08 00   Sleep 8
 *   22 01 1c 00   Set   bank1 bit28 = 0
 *   09 0a 5a 00   Sleep 90            = DIE FAHRT (3 s @30 Hz)
 *   22 01 1c 01   Set   bank1 bit28 = 1
 *   09 0a 08 00   Sleep 8
 *   22 01 1c 00   Set   bank1 bit28 = 0
 *   09 0a 14 00   Sleep 20            = ANKUNFT
 *
 * In diesen 32 Byte steht kein Se_on (0x36), und ROOM1080/1081/4020/4021 haben
 * ueber das GANZE Skript hinweg NULL Se_on (Opcode-Zensus mit
 * tools/scd_dump_room.py, selbst gefahren).
 *
 * 2. DIE LUECKE, DIE DER PRUEFER AUFGEMACHT HAT — SELBST GESCHLOSSEN
 * ------------------------------------------------------------------
 * RE1.5 loest SE nicht nur per Skript aus, sondern auch DIREKT aus dem
 * Stage-Overlay heraus, ueber den SE-Emitter FUN_80045024 (@0x80045024
 * `srl v1,a0,24` = Bank im obersten Byte von a0 — selbst disassembliert).
 * Alle diese Rufe habe ich gezaehlt und ihre Aufrufer aufgeloest:
 *
 *   STAGE1.BIN: 11 x `jal 0x80045024` (Wort 0x0C011409) @0x8010A3E4, 0x8010A7B8,
 *     0x8010A804, 0x8010A864, 0x80111A50, 0x80111DF4, 0x80115C04, 0x80115C3C,
 *     0x80115CA4, 0x8011C1E4, 0x8011C4B8
 *   STAGE4.BIN: 12 x @0x8010A398, 0x8010A76C, 0x8010A7B8, 0x8010A818,
 *     0x80111484, 0x801115BC, 0x801117E8, 0x80115DC8, 0x80116060, 0x8011632C,
 *     0x80116434, 0x80116704
 *
 * Diese 23 Rufe sitzen in 7 bzw. 8 Funktionen, und JEDE dieser Funktionen hat
 * NULL direkte jal-Xrefs: sie haengen ausschliesslich in Zustands-Sprungtabellen
 * (z.B. STAGE1 @0x801201A8ff, gelesen vom Verteiler @0x8010A28C:
 *   8010a290: lbu v0,0x800aca59      ; Zustandsbyte
 *   8010a29c: sll v0,v0,2
 *   8010a2a4: addiu at,at,424        ; = 0x801201a8
 *   8010a2ac: lw  v0,0(at)
 *   8010a2b4: jalr v0
 * ) und dieser Verteiler wird beim Overlay-Start @0x8011E9A0-A4 in die
 * Gegner-Routinentabelle 0x800AC798/79C/7A0/7B0/7B8/7C8/7CC eingetragen —
 * also in die Typ-Slots der GEGNER. Die Spieler-Slots 0x800AC790/794 werden vom
 * Overlay gar nicht beschrieben (die EXE liest sie direkt, z.B. @0x8003E86C).
 *
 * Und: die vier Fahrstuhlraeume spawnen keinen einzigen Gegner (Opcode-Zensus
 * ROOM1080: Set/Ifel_ck/Endif/Sleep/Ck/Nop/Evt_end/Work_set/Member_set/
 * Sce_key_ck/Evt_exec/Evt_next/Cut_chg/Aot_on/Aot_set/Speed_set/Member_cmp/
 * Add_speed/Plc_dest/Edwhile/Door_aot_set/Do/Obj_model_set/B — kein Sce_em_set,
 * kein Plc_motion). Keiner der 23 Overlay-Emitter kann dort also feuern.
 *
 * Gegenprobe aus RE2: RE2 fuehrt in ROOM21B0/ROOMB1B0 DIESELBEN 32 Bytes aus
 * und setzt TROTZDEM zwei eigene Se_on davor. Der bit28-Puls selbst ist somit
 * nachweislich kein Ton-Ausloeser — sonst braeuchte RE2 die Se_on nicht.
 *
 * 3. WAS IMPORTIERT WIRD (RE2, ROOM21B0.RDT — alle Offsets selbst gelesen)
 * ------------------------------------------------------------------------
 *   @0x2756  36 02 11 01 01 00 00 00 00 00 00 00   Se_on bank2 id 0x11  (vor Puls 1)
 *   @0x2784  36 02 12 01 01 00 00 00 00 00 00 00   Se_on bank2 id 0x12  (vor Puls 2)
 * Operanden-Layout aus dem Handler selbst (LAB_80041624): @0x80041644
 * `lbu a3,1(s0)` = Bank, @0x80041648 `lh a0,2(s0)` = Id; Bank 2 = SND0
 * (re15_audio.h:157). Wellen/EDT/Tone -> tools/re2_elevator_cut.py.
 *
 * 4. DER AUSLOESER — AUS DEN DATEN, KEINE RAUMNUMMER IM CODE
 * ----------------------------------------------------------
 * Anker ist die 32-Byte-Signatur aus (1), gesucht im GELADENEN RDT-Puffer.
 * Gemessen ueber alle 240 RE1.5-RDTs: 4 Dateien, je 3 Treffer, 0 Fehltreffer
 * (gen/re15_elev_se.inc traegt die Fundstellen fuer den Riegel).
 * Die halbe Signatur waere NICHT eindeutig — `22 01 1c 01 09 0a 08 00
 * 22 01 1c 00 09 0a 14 00` allein steht auch in ROOM5090/5091/6030/6031.
 *
 * EINHAENGEPUNKT: NICHT op_set. op_set (scd_vm.c) ist der einzige Schreibpfad
 * fuer ALLE Spiel-Flags in allen 240 Raeumen (allein 716 statische Set-Stellen
 * in 40 STAGE1-Raeumen); ein Ton-Haken dort mischt sich in Tuer-, Event- und
 * Save-Latches. Stattdessen haengt der Ton an der EINEN Stelle im
 * SCD-Verteiler, an der der Programmzeiger ohnehin gelesen wird
 * (`uint8_t op = *t->pc;`), und zwar hinter dem Wachposten
 * `if (g_re15_elev_anchor_n)`. Der ist in 236 von 240 Raeumen 0 — ein
 * Global-Load und ein Sprung, und die Flag-Logik bleibt unberuehrt.
 *
 * 5. DER ZWEITE FAHRSTUHLTYP — WELLE 2 (2026-09-27)
 * --------------------------------------------------
 * Nutzer-Auftrag woertlich: "fahrstuhl sound muss ueberall bei fahrstuehlen rein.
 * Der fehlt weil REsident Evil 1.5 eine 40% Beta ist und unvollstaendig."
 * Damit ist die frueher hier offen gelassene Frage ("toent der Ton auch in
 * ROOM4020/4021?") ENTSCHIEDEN: ja, ueberall, ausdruecklich gewollt.
 *
 * Und es sind DREI Kabinen, nicht zwei. Gesucht wurde deshalb nicht die eine
 * Bytefolge, sondern die GESTALT einer Fahrt ueber alle 240 RDTs:
 *   1) ROOM1080/1081  STAGE1 "ELEVATOR"        3 Fahrten je Datei  SIG1
 *   2) ROOM3080/3081  STAGE3 "WAREHOUSE LIFT"  2 Fahrten je Datei  SIG2  (NEU)
 *   3) ROOM4020/4021  STAGE4 "A-2 ELEVATOR"    3 Fahrten je Datei  SIG1
 *
 * Die WAREHOUSE LIFT faellt durch SIG1, weil sie ein ANDERES Fahrskript hat.
 * Bytes selbst gelesen (ROOM3080.RDT @0x09FE):
 *   22 01 1c 01   Set   bank1 bit28 = 1    <- Puls 1 (Fahrt)
 *   09 0a 3c 00   Sleep 60
 *   22 01 1c 00   Set   bank1 bit28 = 0
 *   22 01 1d 01   Set   bank1 bit29 = 1    <- Puls 2 (Ankunft), ZWEITES Bit
 *   09 0a 3c 00   Sleep 60
 *   22 01 1d 00   Set   bank1 bit29 = 0
 *   22 01 1c 01   Set   bank1 bit28 = 1
 * Schlafzeiten 60/60 statt 8/90/8/20, zwei Bits statt einem.
 *
 * EINDEUTIGKEIT GEMESSEN (der Generator bricht ab, wenn eine Signatur ausserhalb
 * ihrer Dateien trifft): SIG1 12 Treffer / 4 Dateien, SIG2 4 Treffer / 2 Dateien,
 * 0 Fehltreffer ueber alle 240 RDTs.
 *
 * Auch die WAREHOUSE LIFT ist im ORIGINAL stumm: ROOM3080 hat zwar drei Se_on
 * (`36 02 0a ..` = Bank 2 Id 0x0A) @0x0095C/@0x00970/@0x00984, aber rund 0xA0
 * Byte VOR der Fahrt, in einem anderen Abschnitt. Wozu sie gehoeren, ist NICHT
 * bestimmt — nur, dass sie nicht in der Fahrt liegen.
 *
 * Nebenbefund zum Anker: bank1 bit0x1C/0x1D ist die Fahrstuhl-BELEUCHTUNG, nicht
 * "Fahrstuhl faehrt" — ROOM6030 verschraenkt dieselben Pulse mit Cut_chg 4/5
 * (@0x01214-0x01250) und hat @0x01004 eine Blink-Schleife mit Goto. Die beiden
 * Signaturen sind also der Flacker-Rhythmus einer Fahrt: gemessen eindeutig,
 * aber keine Semantik. Keine Fahrstuehle trotz bit0x1C/0x1D: ROOM2040/2041,
 * ROOM20B0/20B1, ROOM5090/5091, ROOM6030/6031.
 * ==========================================================================*/

#include <string.h>

#include "re15_elev_se.h"
#include "re15_audio.h"

#include "gen/re2_elev_bank.inc"   /* RE2_ELEV_EDT_SIZE / _VBD_OFF / _VBD_SIZE / SE-Ids */
#include "gen/re15_elev_se.inc"   /* s_re15_elev_sigs[], RE15_ELEV_SE_FIRST/SECOND,
                                   * s_re15_elev_hits[] */

/* 3 Etagen je Kabine * 2 Pulse = 6; 16 ist reichlich Luft. */
#define RE15_ELEV_ANCHOR_MAX 16

int g_re15_elev_anchor_n = 0;

static const unsigned char *s_anchor_pc[RE15_ELEV_ANCHOR_MAX];
static unsigned char        s_anchor_se[RE15_ELEV_ANCHOR_MAX];

/* Mess-Schiene fuer den Riegel — keine Spiellogik. */
static unsigned char s_fired[32];
static int           s_fired_n = 0;

void re15_elev_se_reset_log(void) { s_fired_n = 0; }

int re15_elev_se_fired(const unsigned char **out)
{
    if (out) *out = s_fired;
    return s_fired_n;
}

void re15_elev_se_room_scan(const unsigned char *raw, int raw_size)
{
    g_re15_elev_anchor_n = 0;
    if (raw == NULL || raw_size <= 0) return;

    for (int g = 0; g < RE15_ELEV_SIG_COUNT; g++) {
        const re15_elev_sig_t *sg = &s_re15_elev_sigs[g];
        const int len = (int)sg->len;
        for (int i = 0; i + len <= raw_size; i++) {
            if (raw[i] != sg->bytes[0]) continue;
            if (memcmp(raw + i, sg->bytes, (size_t)len) != 0) continue;
            if (g_re15_elev_anchor_n + 2 > RE15_ELEV_ANCHOR_MAX) break;
            /* Puls 1 @sig+0x00    -> id 0x11 (RE2 ROOM21B0.RDT @0x2756),
             * Puls 2 @sig+delta2  -> id 0x12 (RE2 ROOM21B0.RDT @0x2784). */
            s_anchor_pc[g_re15_elev_anchor_n] = raw + i;
            s_anchor_se[g_re15_elev_anchor_n] = (unsigned char)RE15_ELEV_SE_FIRST;
            g_re15_elev_anchor_n++;
            s_anchor_pc[g_re15_elev_anchor_n] = raw + i + sg->delta2;
            s_anchor_se[g_re15_elev_anchor_n] = (unsigned char)RE15_ELEV_SE_SECOND;
            g_re15_elev_anchor_n++;
            i += len - 1;             /* Treffer sind ueberlappungsfrei */
        }
    }
}

void re15_elev_se_pc(const unsigned char *pc)
{
    if (pc == NULL) return;
    for (int k = 0; k < g_re15_elev_anchor_n; k++) {
        if (s_anchor_pc[k] != pc) continue;
        if (s_fired_n < (int)(sizeof s_fired)) s_fired[s_fired_n++] = s_anchor_se[k];
        re15_audio_re2_elevator_se((int)s_anchor_se[k]);
        return;
    }
}

/* Gemessene Fundstellen (gen/re15_elev_se.inc) — nur fuer den Riegel. */
int re15_elev_se_hit_count(void) { return RE15_ELEV_HIT_COUNT; }

int re15_elev_se_hit(int i, unsigned *out_room, unsigned *out_off)
{
    if (i < 0 || i >= RE15_ELEV_HIT_COUNT) return -1;
    if (out_room) *out_room = s_re15_elev_hits[i].room;
    if (out_off)  *out_off  = s_re15_elev_hits[i].off;
    return 0;
}

int re15_elev_se_hit_sig(int i)
{
    if (i < 0 || i >= RE15_ELEV_HIT_COUNT) return -1;
    return (int)s_re15_elev_hits[i].sig;
}

/* Satz-TOC der Mini-Bank (gen/re2_elev_bank.inc) — Muster re2_enemse_toc_entry. */
void re15_elev_bank_rec(re15_elev_bank_rec_t *out)
{
    if (!out) return;
    out->edt_off    = RE2_ELEV_EDT_OFF;
    out->edt_size   = RE2_ELEV_EDT_SIZE;
    out->vbd_off    = RE2_ELEV_VBD_OFF;
    out->vbd_size   = RE2_ELEV_VBD_SIZE;
    out->vag1_size  = RE2_ELEV_VAG1_SIZE;
    out->vag2_size  = RE2_ELEV_VAG2_SIZE;
    out->se_ride    = RE2_ELEV_SE_RIDE;
    out->se_arrive  = RE2_ELEV_SE_ARRIVE;
}
