/*
 * re15_werfer.h — Runde 35 Spur B: Werfer-Klasse (Granatwerfer 15/16/17, Raketenwerfer 18),
 * Flammenwerfer 14 und Colt Python 20.
 *
 * EINORDNUNG (Beta -> Retail, Memory reai-v2-beta-zu-retail; Belege Dossier
 * analysis/befunde_runde35/B_werfer.md §2): im RE1.5-Auslieferungsstand sind
 *   15..18  Entlade-Handler NULL (@0x8007413c-4b), Munitions-Zeiger NULL-Record 0x80074c88 (Record +4: @0x80074e60/6c/78/84),
 *           Waffen-Parametersatz 0 (@0x800740d6-e9) — nur Baenke, Magazin und Schaden fertig,
 *   20      Dispatch NULL (@0x80074080), Schaden 0 in jeder Gegnerzeile (@0x8006e0d0 Spalte 20),
 *   14      Handler nur als Debug-Patch in DEBUG.BIN (a1 = 3000 @0x800c466c, Rauch-jal entfernt),
 *           Effekt 3 sub 5 = aufsteigender Puff ohne Vorwaertsbewegung (CORE00.ESP @0x0574).
 * Deshalb ist RE2 Retail (info/re2leon/PSX.EXE) das Vorbild fuer Entladung, Projektil, Treffer,
 * Effekt und Ton; RE1.5 bleibt massgeblich fuer Animationsbaenke, Magazine, Schadensspalten,
 * Reichweiten und — wo fertig — die eigenen Toene.
 *
 * RE2-Effekt-Handler je Waffe (Tabelle @0x800A6FDC, gelesen `lhu v0,0x10e(s1)` / `jalr`
 * @0x800431ac-cc im Feuerzustand, jedes Bild mit In-Clip-Bild +0x14D):
 *   [9]  GL Explosiv  0x80044B44   [10] GL Brand 0x80044F44   [11] GL Saeure 0x80045090
 *   [16] Flammenwerfer 0x800454A0  [17] Rakete  0x80045588
 * Spawner FUN_8001bf10 (sofort lebendig, Status 0xA003): a0 = Bank<<24 | Sub<<16 | Skala,
 * a1 -> Platz+0x22 (Gier), a2 = Waffenknochen-Matrix (*(+0x198) + 0x7AC), a3 = Versatz.
 */
#ifndef RE15_WERFER_H
#define RE15_WERFER_H

#include <stdint.h>

/* 15..18 = Werfer-Klasse (RE1.5 Entlade-Tabelle @0x8007413c-4b NULL). */
int  re15_werfer_ist(int id);
/* Nachladbar: 15/16/17 (RE1.5 Magazin 6 @0x80074e5c/e68/e74 + Munitions-Records 0x80074cb4/b8/bc,
 * nur der Zeiger ist unverdrahtet) und 20 (Magazin 6 @0x80074e98; Munition MAGNUM 0x17 =
 * PORT-WAHL, s. Dossier §3.5). 18 = 4 Schuss ohne Munitionsitem (@0x80074e80, RE2 Id 17 ohne
 * Kombinations-Satz @0x800a9ea4). */
int  re15_werfer_nachladbar(int id);
/* Animationsbank/Mesh: 16 und 17 fuehren die Bank der 15 (RE1.5 PL00W10 = PL00W11 ist eine
 * Ingram-foermige Platzhalterbank ohne Granatwerfer-Clips; Elza PL04W0F = W10 = W11 und RE2
 * PL01W09 = 0A = 0B: EINE Bank fuer alle drei Munitionsarten). */
int  re15_werfer_bank_id(int id);
/* Clip-Umsetzung fuer die 11-Clip-Bank PL00W0F (Leon): [22,31,39,50, 4:Heben 15, 5:Feuer 34,
 * 6:Hold 1, 7:Feuer-hoch 36, 8:Hold-hoch 1, 9:Feuer-tief 36, 10:Hold-tief 1] = Standard-Folge
 * ohne die Clips 3/5/13 -> Basis -2; Nachladen (13) hat keinen Clip -> Hold (6).
 * Liefert den Clip unveraendert, wenn die Bank nicht 11 Clips hat oder id nicht 15..17 ist. */
int  re15_werfer_clip_remap(int id, int clip_n, int clip);
/* Rueckstoss-Abbruchschwelle (Standard-FSM `!R1 && acae9 > tab5[(id-1)*5+2]` @0x80033634-4c):
 * die Saetze der Ids 14..18/20 sind 0 (@0x800740d1-e9 und @0x800740ef-f3, unfertig); PORT-WAHL = Klassenwert 10
 * der fertigen schweren Waffen 5..13 (Saetze @0x800740a4..0x800740cc, je byte 2 = 0x0a). */
int  re15_werfer_recoil_break(int id);

/* Je Spielbild hinter dem Feuerpfad: RE2-Effekt-Handler der Ids 15..18 im Rueckstossbild 1
 * (`lbu v1,333 / addiu v0,zero,1 / bne` @0x80044b98-a0 GL, @0x8004559c-a4 Rakete). */
void re15_werfer_tick(void);

/* Flammenwerfer (RE2 [16] @0x800454a0), je Bild der Dauerfeuer-Schleife mit In-Clip-Bild f:
 * Strahl 0x031D1200 bei f % 3 == 1 (@0x800454b4-e0), a1 = Gier (`lh a1,-914(a1)` = Spieler
 * +0x76 @0x80045500), Versatz {150,1200,0} (@0x800454f0-508); SE 0x01000001 bei f == 1
 * (@0x80045534-44), SE 0x010B0001 bei f == 11 (@0x80045550-6c). */
void re15_werfer_flamme_bild(int f);
/* Fuel je Bild der Schleife = RE2 FUN_8006a0cc Id 16 (@0x8006a184-0x8006a21c): Zaehler
 * DAT_800d5c1c, alle 8 Bilder zwei Einheiten; Rueckgabe 0 = leer (Schleife beenden). */
int  re15_werfer_fuel_bild(void);
void re15_werfer_reset(void);
/* Leerschuss-Ton der Werfer-Klasse (Nachbesserung 1, M2): 1 = hier gespielt, 0 = der Aufrufer
 * spielt den RE1.5-Klick 0x01010001 der gefuehrten ARMS-Bank. Nur der Raketenwerfer 18: seine
 * RE1.5-Bank ARMS12 hat KEINEN Satz 1 (ARMS12.EDH Dateibytes 4..7 = ff ff ff ff), RE2 spielt im
 * Haltezustand der Standard-FSM 0x01010001 (@0x80043868/94-9c) = ARMS11 Satz 1 (EDH Bytes 4..7 =
 * 00 00 54 16). */
int  re15_werfer_leer_ton(int id);

/* Inventar-Kombination (Haken in menu_common.c exchange_match / exchange_exec): Aktion fuer das
 * Paar (Cursor-A-Id, Cursor-B-Id) der Werfer-Klasse und der Python — 2/3 = RE1.5-Nachladen in
 * A/B (Satzform `19 0f 02 00` @0x80074cb4 / `07 07 03 00` @0x80074cf8), 7/8 = RE2-Munitions-
 * wechsel des Granatwerfers (Zustaende 7/8 @0x8006bc18 / @0x8006bd98), 0 = kein Paar. Die
 * RE1.5-Zeilen 15..18/20 tragen pair_count 0 (@0x80074da8 + id*12 + 9) = unverdrahtet. */
int  re15_werfer_paar(uint8_t id_a, uint8_t id_b, uint8_t *result, uint8_t *pic);
/* RE2-Zustand 7: GL-Id := Runde - 0x0a, Runde := alte GL + 0x0a, Mengen getauscht, leere Runde
 * geloescht (sonst MIXITEM-Bild `pic` in die Zelle). */
void re15_werfer_gl_tausch(int gl_slot, int rd_slot, int pic);

/* Waffenrahmen des Leon-Granatwerfers PL00W0F (11-Clip-Bank): dreht die Knochenmatrix `rot`
 * (Q12, zeilenweise) um die 35,62 Grad, um die das Netz im Knochenrahmen gedreht modelliert ist
 * (MD1 @0x50A8, Punkte @0x518C-0x522C; sin 2386 / cos 3330), und liefert den Muendungsversatz
 * {-204,1717,3} im Netzrahmen. Rueckgabe 1 = angewendet, 0 = Netz liegt schon auf +y (alle
 * anderen Baenke). Belege: werfer_r35.c, Dossier §4.1 G1. */
int  re15_werfer_rahmen(int id, int clip_n, int32_t rot[9], int16_t ofs[4]);

/* Diagnose (Sonden): Zahl der RE2-Spawns seit reset, letzter Rueckstoss-Spawn-Bildzaehler. */
unsigned re15_werfer_spawns(void);

/* Nachbesserung 2 (Abnahme 1 N1, Dossier §9): Wandtest der Werfer-Geschosse.
 * re15_werfer_zelle_strecke: beruehrt die Strecke (x0,z0)-(x1,z1) die solide FLAECHE der Zelle (Typ 1..9 nach dem
 *   Verteiler 0x800b2858, @0x8003af04-84)? Punkt = Strecke der Laenge 0.
 * re15_werfer_band_strecke: dieselbe Frage gegen alle soliden Zellen des Bandes (u0 Bit 0, floor >> 4, u1 Bit 1 frei),
 *   je Quadrantenliste (FUN_8003b068); 1 = gesperrt. */
#include "re15_rdt.h"
int re15_werfer_zelle_strecke(const re15_sca_entry_t *e, int32_t x0, int32_t z0, int32_t x1, int32_t z1);
int re15_werfer_band_strecke(const re15_rdt_t *rdt, int32_t x0, int32_t z0, int32_t x1, int32_t z1, int band);

/* Plattform (audio_pc.c; Test-Stub in tests/test_support.c): Satz `satz` der RE2-ARMS-Bank
 * shared_assets/RE2/SOUND/ARMS<id>.EDH/.VB (RE2 Bank 1 = ARMS der Waffe, FUN_80059c74). */
void re15_audio_re2_arms_se(int arms_id, int satz);

#endif /* RE15_WERFER_H */
