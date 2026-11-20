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

/* ===== Strang ton Stufe 6 (2026-11-20): der SENDER eines Geraeuschs mit Lage (Messbericht U6) ======================
 * FUN_80045a64 rechnet die Lage immer aus EINER Weltposition. Woher sie kommt, haengt am Ruf:
 *   (a) FUN_800453d0 (Raum-Geraeusch, snd1) und FUN_80045630 (Schritt): DAT_800ac784 + 0x34 = die Entity, deren Code
 *       GERADE laeuft (`lw v0,-0x387c(v0)=>DAT_800ac784` @0x800453d4, FUN_80045a64-Ruf @0x800454e0-ec bzw. @0x8004582c-30).
 *       DAT_800ac784 setzt die Entitaeten-Schleife FUN_8001a50c je Entity (Start `sw v0,0(v1)` @0x8001a540, je Runde
 *       `addiu v0,v0,500` / `sw v0,0(s2)` @0x8001a5a8-ac, Handler `jalr v0` @0x8001a588) und der Spieler-FSM
 *       FUN_80031c44 auf den Spieler (`sw s0=>DAT_800aca54` @0x80031c6c). Hauptlauf: Schleife `jal 0x8001a50c`
 *       @0x8001ce04, danach Spieler `jal 0x80031c44` @0x8001ce0c -> ausserhalb der Gegner-Schleife ist es der Spieler.
 *       GEMESSEN (Original-Savestate ROOM1140 Cut 0, scratchpad ton/s6/ss_lage_reg.py): DAT_800b2824/26 = 102/65 =
 *       FUN_80045a64 der Entity 4 (Typ 0x11, 200/0/-19600), SE-Stimme 20 (Raum-Satz, Tone 4) Register 10567/4259;
 *       die Lage des Spielers waere 93/103.
 *   (b) FUN_80045024 (Bank-Geraeusch, ARMS/CORE) mit Lage-Bit: param_2 = Zeiger auf die Weltposition (@0x800451c0-cc).
 *       Spieler-Rufe: &DAT_800aca88 (= Spieler+0x34, z. B. `addiu a1,..` @0x800123d4); der Schuss (ESP-Routine 9,
 *       FUN_80017654): die s16-Weltlage des Effekt-Platzes (`lh v0,40/42/44(v1)` @0x80017664-7c, `addiu a1,sp,16`
 *       @0x80017680, `jal 0x80045024` @0x80017684).
 * Port: (a) re15_svm_sender_entity(e) setzt die laufende Entity (NULL = Spieler), (b) re15_svm_sender_lage(pos) setzt
 * param_2 fuer den naechsten Bank-Ruf (NULL = Spieler-Ruf). `entity` ist ein const re15_actor_t * (re15_actor.h). */
void re15_svm_sender_entity(const void *entity);
void re15_svm_sender_lage(const int32_t *pos);
/* Lage fuer (a) bzw. (b); 0 ohne Raum/Kamera (dann Zweig ohne Lage). */
int  re15_svm_lage_entity(int lage[2]);
int  re15_svm_lage_param(int lage[2]);
/* Messhaken: Slot der gesetzten Entity (-1 = Spieler/keine), 1 wenn ein param_2 gesetzt ist. */
int  re15_svm_sender_entity_slot(void);
int  re15_svm_sender_lage_gesetzt(void);
/* Messhaken (kein Spielpfad, NULL im Spiel): wird bei jedem re15_svm_sender_entity mit dem Slot gerufen (-1 = Spieler). */
extern void (*re15_svm_sender_haken)(int slot);

/* ===== Strang ton Stufe 6 Runde 2 (2026-11-20): die uebrigen Geraeusch-Wege mit Lage-Bit ===========================
 * Zensus aller Rufe `jal 0x80045024` in PSX.EXE und STAGE1-6.BIN (scratchpad ton/s6r2/zensus_45024.py): 103 Rufe; das
 * Lage-Bit ist das niedrige Byte von param_1 (`(param_1 & 0xff) != 0` -> FUN_80045a64(param_2) @0x800451c0-cc).
 *
 * (c) CORE-Rufe der SPIELER-Zustaende und der Opfer-Maschinen: param_1 = 0x04ss0001, param_2 = der Spieler. Zwei Formen:
 *     - EXE-Spielerzustaende: a1 = 0x800aca5a + 46 = &DAT_800aca88 (z. B. `lui a2,0x800b / addiu a2,a2,-13734` @0x8003671c-20,
 *       `addiu a1,a2,46` @0x800367ac, a0 0x04030000|1 @0x80036744/64; ebenso @0x80035c00, @0x80035d38, @0x80035e80,
 *       @0x80036004, @0x80036184, @0x80036274, @0x80036340, @0x800364f0), STAGE5-Birkin-Opfer @0x8011b0cc/b254/b264/
 *       b4b8/b4e0/b788 dieselbe Form (a1 = 0x800aca5a + 46 bzw. 0x800acae3 - 91).
 *     - Opfer-Maschinen in den Overlays: a1 = DAT_800ac784 + 0x34 (`lw a1,-14460(a1)` / `addiu a1,a1,52`), und DAT_800ac784
 *       ist dort der SPIELER: sie laufen im Spieler-FSM FUN_80031c44 (`addiu s0,v0,720` / `sw s0,0(v0)` @0x80031c64-6c ->
 *       DAT_800ac784 = 0x800aca54), Befehl 5/6 ueber die Tafeln 0x80073f90[5] = 0x80036834 (jalr *(0x800ac758 + Typ*4)
 *       @0x80036884-a8) und [6] = 0x800368c0 (jalr *(0x800ac858 + Typ*4) @0x80036910-34). GEMESSEN im Original-Savestate
 *       (ton/s6r2/opfer_tab.py, h_schuss1140/pre.sav): Befehl 5 Zombie 0x8010a28c, Hund 0x80111944, Gorilla 0x8011c118;
 *       Befehl 6 Zombie 0x8010a6b8, Hund 0x80111cb0, Gorilla 0x8011c3d4; deren Unter-Tafeln (Index DAT_800aca59)
 *       0x801201a8 -> 0x8010a2cc (Ruf @0x8010a3e4), 0x801201d8 -> 0x8010a6f8 (Rufe @0x8010a7b8, @0x8010a864),
 *       0x801210f0 -> 0x80111cf0 (Ruf @0x80111df4), 0x80121580 -> 0x8011c414 (Ruf @0x8011c4b8). STAGE3 0x80115d70 (Rufe
 *       @0x80115de4, @0x80115f1c) haengt am gleichen Verteiler-Muster (`lbu v0,-13735(v0)` = DAT_800aca59, Tafel 0x8011ee44,
 *       `jalr v0` @0x80115d30-58).
 *     re15_svm_core_se_spieler(id) = FUN_80045024(0x04<<24 | id<<16 | 1, &DAT_800aca88). Die Menue-/Item-Toene 0x0404..0x0406
 *     tragen kein Lage-Bit (`lui a0,0x404` ohne `ori`, a1 = 0 @0x8004a47c-0x8004a664) und bleiben re15_audio_core_se.
 * (d) SCD Se_on (0x36, Handler @0x80041624): param_1 = (Bank<<24)|(Id<<16)|Flags (`sll v1,a3,24` / `srl a0,a0,8` /
 *     `or` @0x80041718-34, Flags = pc[3]), param_2 = sp+16 (`addiu a1,sp,16` @0x80041710) = Ursprung + Versatz, der
 *     Versatz mit `lhu` (NULL-ERWEITERT, @0x800416ec / @0x800416fc / @0x8004170c) und `addu` auf den Ursprung, als 32-Bit-
 *     Wort gespeichert (`sw` @0x800416f8 / @0x80041708 / @0x8004171c), und FUN_80045a64 liest `lw` (@0x80045aa4 ff.).
 *     Ursprung nach pc[4] (Tafel 0x80010d9c): 0 -> 0/0/0 (@0x80041678-84), 1 -> Spieler 0x800aca54 + 0x34 (@0x80041688-8c,
 *     @0x800416e0-e8), 2 -> Gegner-Pool 0x800acc2c + Index*500, 3 -> Objekt-Pool 0x800b3f98 + Index*0x94 (@0x80041698-dc).
 *     Zensus aller 284 Se_on der Disc (ton/s6r2/zensus_seon.py): Flags != 0 nur 5-mal, alle in ROOM3091, alle Ursprung 0.
 *     re15_svm_se_on_param liefert param_2 fuer Ursprung 0 und 1; 1 wenn das Lage-Bit gesetzt und der Ursprung gebaut ist. */
void re15_svm_core_se_spieler(int se_id);
int  re15_svm_se_on_param(int flags, int ursprung, int pool, int16_t vx, int16_t vy, int16_t vz, int32_t pos[3]);
/* (e) Raum-Geraeusch FUN_800453d0 eines Port-Ankers, der im Original im Code einer Entity laeuft (Sender = DAT_800ac784 + 0x34
 *     = diese Entity, @0x800453d4 / @0x800454e0-ec): Sender fuer den einen Ruf auf g_actors[slot] (gueltig und aktiv, sonst
 *     bleibt der laufende Sender), danach der vorige Sender. Nutzer: Roys Anker ROOM1021 (elza_r36.c, elzatoene_r38.c). */
void re15_svm_raum_se_von(int slot, int se_id);

#endif /* RE15_SVM_VOL_H */
