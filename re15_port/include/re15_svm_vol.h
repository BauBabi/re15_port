/*
 * RE1.5 Port — Lautstaerke-Gesetz der Sony-Tonbibliothek libsnd (Strang "ton", Stufe 1, 2026-11-19).
 *
 * Nutzer 19.11.2026: "Bei der PC Version klingen die Sounds irgendwie dumpfer/anders als in der PSX Version."
 * Messbericht U1: der Port rechnete die Stimmen-Lautstaerke linear und zog pauschal ab (>>1, SS_BASE_MVOL
 * 0x1a00); das Original rechnet sie in SpuVmKeyOnNow als Produkt und setzt es ins QUADRAT.
 *
 * ===== SpuVmKeyOnNow @0x80055d10 (info/Re1.5/PSX.EXE, selbst disassembliert) =====================================
 *   80055d1c  lbu v1,24(v0)            ; VabHdr+0x18 = Bank-Lautstaerke (v0 = *(0x800b2b34))
 *   80055d24  lbu a0,0x800b5318        ; Tastenstaerke (Anschlag)
 *   80055d28  sll v0,v1,14 / subu      ; Bank * 0x3fff
 *   80055d30  mult a0,v0               ; Anschlag * Bank * 0x3fff
 *   80055d38  lui 0x8206 / ori 0x1029 / mult / mfhi / sra 13     ; / 0x3f01 (= 127 * 127)
 *   80055d48  lbu a0,0x800b531e        ; Programm-Lautstaerke (ProgAtr+1)
 *   80055d60  mult a2,a0
 *   80055d6c  lbu v0,0x800b5321        ; Ton-Lautstaerke (VagAtr+2)
 *   80055d74  mult v1,v0
 *   80055d7c  lui 0x40c / ori 0x2051 / multu ... srl 13 @0x80055dac   ; / 0x3f01
 *   80055db4  lhu v1,0x800b532a        ; Sequenz-Kennung; 80055dfc ori v0,zero,0x21 / 80055e00 beq a1,v0 -> 0x80055e68
 *   80055e08  lhu v0,116(v1) / mult a3,v0 / (0x02040811) srl 6        ; links  * Sequenz+0x74 / 127
 *   80055e28  lhu v0,118(v1) / mult a3,v0 / (0x02040811) srl 6        ; rechts * Sequenz+0x76 / 127
 *   80055e6c  lbu v1,0x800b5322 (Ton-Pan)      80055e74 sltiu v0,v1,0x40
 *             < 0x40: rechts = rechts * Pan / 63          (mult a3,v1 @0x80055e7c, 0x04104105 srl 5 @0x80055ea4)
 *             sonst : links  = links * (0x7f - Pan) / 63  (@0x80055ea8-d8)
 *   80055ee0  lbu v1,0x800b531f (Programm-Pan) — dieselbe Zweiteilung @0x80055ee8-0x80055f48
 *   80055f50  lbu v1,0x800b5319 (Tasten-Pan)   — dieselbe Zweiteilung @0x80055f58-0x80055fb8
 *   80055fc0  lh v1,0x800b2678 / 80055fc8 bne v1,1  ; Mono-Schalter: beide = der groessere (@0x80055fd0-e4)
 *   80055fe8  mult a2,a2               ; QUADRAT links
 *   80055ff0  lui v1,0x4 / ori v1,v1,0x11 / multu ... srl a2,a0,13 @0x80056020   ; / 0x3fff
 *   80056008  mult a1,a1               ; QUADRAT rechts ... srl a1,v0,13 @0x8005606c  ; / 0x3fff
 *   80056058  sh a2,0(v0)              ; -> 0x8008f6ac + Stimme*16 = SPU-Register Lautstaerke links
 *   80056070  sh a1,0(v1)              ; -> 0x8008f6ae + Stimme*16 = SPU-Register Lautstaerke rechts
 *
 * GEMESSEN am laufenden Original (30 DuckStation-Savestates des Messberichts, echte SPU-Register gegen diese Rechnung
 * aus dem Spiel-RAM, scratchpad ton/s1/formel_zensus.py): 138 von 138 klingenden Stimmen bytegenau - 131 Musik-Stimmen
 * (129 ueber diese Key-On-Rechnung, 2 ueber SpuVmSetVol @0x8005926c nach einem Controller 7; 66 der 131 mit Register 0,
 * weil die Bank fuer die Messung stummgeschaltet war) und 7 Geraeusch-Stimmen (voll/volr aus dem Vormerk-Satz), u. a.
 *   Rolltor-Rumpeln ROOM1130 snd0 Ton 7 (vol 127)                      -> 16383/16383
 *   Rotor SUB_15 Programm 0 (mvol 80) Ton 0/1 (vol 127), Anschlag 127   ->  6500/ 6500
 *   MAIN17 Programm 0 Ton 0 (Bank 100, Programm 110, Ton 50), Anschlag 115 * Kanal 90 / 127 -> 480/480
 *   MAIN17 Programm 3 Ton 1 (Ton 80), Anschlag 127, Kanal 127            ->  3023/ 3023
 *
 * ===== Tastenstaerke =============================================================================================
 *   Geraeusch ohne Lage: FUN_80045024 `(param_1 & 0xff) == 0` -> DAT_800b2824 = DAT_800b2826 = Ton-Lautstaerke
 *     (`lbu 0x2(s1)` @0x800451dc ff.), d. h. die Ton-Lautstaerke geht ZWEIMAL ein.
 *   Geraeusch mit Lage:  DAT_800b2824/26 aus FUN_80045a64 (Entfernung/Richtung, Port: re15_wassersprung_r41_lage).
 *   SsUtKeyOnV @0x80059d3c macht aus (voll, volr) Tastenstaerke + Tasten-Pan:
 *     80059e08 bne a0,v1 ... gleich:  Staerke voll, Pan 0x40          (@0x80059e10-20)
 *     80059e2c volr < voll:           Staerke voll, Pan (volr<<6)/voll (@0x80059e30-6c)
 *     80059e78 sonst:                 Staerke volr, Pan 0x7f - (voll<<6)/volr (@0x80059e78-bc)
 *   Musik: SpuVmKeyOn @0x800584f0-0x80058534: Staerke = Anschlag * Kanal-Lautstaerke (Sequenz+0x4e+2*Kanal) / 127
 *     (0x81020409 sra 6), Tasten-Pan = Kanal-Pan (`sb s7,0x800b5319` @0x80058550, Sequenz+0x17+Kanal).
 *
 * ===== RE2 (info/re2leon/SLUS_007.48) — fuer die RE2-Tonbaenke des Ports ==========================================
 *   _SsVmKeyOnNow @0x80083760: dasselbe Produkt (@0x8008376c-0x80083804) und dieselben drei Pan-Stufen
 *   (@0x800838b0 / @0x80083928 / @0x80083998), ABER vor dem Quadrat:
 *     80083a30  lh v1,0x800dcc46 (Sequenz-Kennung) / 80083a34 addiu v0,zero,33 / 80083a38 beq v1,v0,0x80083a98
 *   -> eine per SsUtKeyOnV gespielte Stimme (Kennung 0x21 = jedes Geraeusch) ueberspringt das Quadrat
 *   (`mult a2,a2` @0x80083a40 / `mult a1,a1` @0x80083a50 laufen nur fuer Sequenz-Stimmen). RE2-Geraeusche sind also
 *   LINEAR, RE2-Musik im Quadrat. Hauptlautstaerke wie RE1.5: FUN_80059654 `_DAT_800dcb9c = 0x7f007f`,
 *   SsSetMVol @0x8007d298 (* 0x81).
 *
 * ===== Hauptlautstaerke und Hardware =============================================================================
 *   FUN_80043a34 `sh v1,DAT_800b5274` @0x80043a7c (= 0x7f, ebenso DAT_800b5276), FUN_80043b80 `jal SsSetMVol`
 *   @0x80043ba8, SsSetMVol @0x8005c9b0: Wert * 129 (`sll v0,a0,7 / addu` @0x8005c9c4-c8) = 0x3fff. Gemessen im
 *   laufenden Original: Hauptlautstaerke-Register L/R = 0x3fff.
 *   SPU (psx-spx soundprocessingunitspu.md Zeilen 357-364): Register Bits 0-14 = "Voice volume/2
 *   (-4000h..+3FFFh = Volume -8000h..+7FFEh)" -> wirksame Lautstaerke = Register * 2, Vollausschlag 0x8000.
 */
#ifndef RE15_SVM_VOL_H
#define RE15_SVM_VOL_H

#include <stdint.h>

#define RE15_SVM_VOLL        0x7f     /* groesster 7-Bit-Wert jeder Stufe                                      */
#define RE15_SVM_PAN_MITTE   0x40     /* `sltiu v0,v1,0x40` @0x80055e74 / SsUtKeyOnV `ori v0,zero,0x40` @0x80059e10 */
#define RE15_SVM_REG_VOLL    0x3fff   /* `lui v1,0x4 / ori v1,v1,0x11` @0x80055ff0-f4 = Teiler 0x3fff            */
#define RE15_SPU_MVOL_REG    0x3fff   /* SsSetMVol(127,127) @0x80043ba8 -> 127 * 129 @0x8005c9c4-c8; gemessen    */
/* SPU-Register -> Q15-Faktor des Port-Mischers (psx-spx Zeile 364: Register = Lautstaerke / 2). Die Rechnung liefert
 * mit den Daten der Disc nur 0..0x3fff; ein Register darueber (Bit 14 = negative Lautstaerke, Bit 15 = Gleit-Modus,
 * psx-spx Z. 363-374) bildet der Port-Mischer nicht nach -> PORT-WAHL: Anschlag bei 0x3fff. */
#define RE15_SPU_REG_Q15(reg) ((((int)(reg) & 0xffff) > 0x3fff ? 0x3fff : ((int)(reg) & 0xffff)) << 1)
#define RE15_SPU_MVOL_Q15    (RE15_SPU_MVOL_REG << 1)              /* 0x7ffe */

typedef struct {
    int key;          /* DAT_800b5318  Tastenstaerke 0..127                                  */
    int key_pan;      /* DAT_800b5319  Tasten-Pan 0..127 (0x40 = Mitte)                      */
    int vab_mvol;     /* VabHdr+0x18   Bank-Lautstaerke                                      */
    int prog_mvol;    /* DAT_800b531e  ProgAtr+1                                             */
    int prog_pan;     /* DAT_800b531f  ProgAtr+4                                             */
    int tone_vol;     /* DAT_800b5321  VagAtr+2                                              */
    int tone_pan;     /* DAT_800b5322  VagAtr+3                                              */
    int ist_seq;      /* DAT_800b532a != 0x21 (Musik-Sequenz); 0 = Geraeusch (SsUtKeyOnV)    */
    int seq_l, seq_r; /* Sequenz+0x74 / +0x76 (SsSeqSetVol), nur mit ist_seq                 */
    int mono;         /* DAT_800b2678 == 1 (OPTIONS SOUND Mono)                              */
} re15_svm_vol_t;

/* RE1.5 SpuVmKeyOnNow @0x80055d10-0x80056070: SPU-Lautstaerke-Register links/rechts (0..0x3fff). */
void re15_svm_keyon_reg(const re15_svm_vol_t *in, int *reg_l, int *reg_r);

/* RE2 _SsVmKeyOnNow @0x80083760-0x80083ac8: wie oben, aber ohne Quadrat fuer Geraeusche (ist_seq == 0,
 * `beq v1,v0,0x80083a98` @0x80083a38). */
void re15_svm_keyon_reg_re2(const re15_svm_vol_t *in, int *reg_l, int *reg_r);

/* SsUtKeyOnV @0x80059e08-bc: (voll, volr) -> Tastenstaerke + Tasten-Pan. */
void re15_svm_utkeyon_split(int voll, int volr, int *key, int *key_pan);

/* SpuVmKeyOn @0x800584f0-0x80058534: Tastenstaerke einer Sequenz-Note = Anschlag * Kanal-Lautstaerke / 127. */
int re15_svm_seq_key(int anschlag, int kanal_vol);

/* Lage des SPIELERS fuer ein Geraeusch mit Lage (FUN_80045a64 mit Spieler+0x34 = &DAT_800aca88): lage[0] = voll
 * (DAT_800b2824), lage[1] = volr (DAT_800b2826). 0 ohne Raum/Kamera (dann gibt es keine Lage). */
int re15_svm_lage_spieler(int lage[2]);

#endif /* RE15_SVM_VOL_H */
