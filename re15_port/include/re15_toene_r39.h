/*
 * RE1.5 Rebuilt — Runde 39 Spur B "toene": Schubladen-Ton ROOM11F0/11F1, Lueftungsgitter- und Schrank-Ton ROOM1190/1191.
 *
 * Auftrag (woertlich: analysis/befunde_runde39/AUFTRAG.md, Belege und Messungen: analysis/befunde_runde39/B_toene.md):
 *   P1 "In Room 10F0 fehlt noch beim oeffnen der Schublade der Sound. Den koennte man zum Beispiel aus Resident Evil 2
 *       holen, wenn dort eine Schublade geoeffnet wird."
 *   P2 "In Room 1090, wenn die Hunde in der Cutscene rein gesprungen wird, fehlt noch der Sound vom oeffnenden Gitter,
 *       den koennte man von den oeffnenden Gittern der Spinnen aus STAGE2 holen."
 *   P3 "In Room 1090 fehlt noch Sound fuer den bewegenden Schrank nach dem loesen des Raetsels. Den koennte man aus
 *       Resident Evil 2 holen von den bewegenden Regalen in der 2. Etage."
 *
 * RAEUME (gemessen, Dossier §0): ROOM10F0 hat keine Schublade (main00/sub00/sub01 ohne Ereignis); die einzige
 * geoeffnete Schublade ist ROOM11F0 sub19 @0x017B6 `29 0b` = Schnitt auf Cut 11 (Bild: die offene Schublade mit den
 * Patronen). ROOM1090 hat weder Hunde noch Schrank; die Raetsel-Szene mit Schrank und Hunden ist ROOM1190 sub10 (dieselbe
 * Ziffernverschiebung wie Runde 36 M R0 "ROOM 1090" = ROOM1190). Elza-Raeume: ROOM11F1 ist bytegleich ROOM11F0, ROOM1191
 * traegt sub10/sub11 bytegleich (Unterschiede nur @0x2D22.. in sub14) -> dieselben Anker.
 *
 * RE1.5 spielt an keiner der drei Stellen einen Ton (kein Se_on in 11F0 sub19, 1190 sub10/sub11). ERGAENZUNG nach
 * NUTZER-VORGABE, die FORM (wann, wie oft) aus dem jeweiligen Vorbild:
 *   P1 RE2-Schubladen-Handler (AOT-Tabelle @0x800a73c4[13] -> 0x80052044): @0x8005218c `jal 0x8005ba28` = Se_on(2,0x28)
 *      (a0 = 0x0228xxxx aus @0x80052114 `lui a0,0x228`) im SELBEN Aufruf wie der Kamera-Schnitt @0x800521d8
 *      `jal 0x8002c7ac` -> Ton im Bild des Schnitts auf die Schublade. Welle: RE2 ROOM1130 Satz 0x28 (Wahl unter den
 *      vier RE2-Schubladen 10E0/1130/1150/2010: Nutzer-SAP "ROOM113.SAP - ... locked drawer sounds?").
 *   P2 RE1.5 ROOM2060 sub02 (Schachtgitter der Spinne): @0x0134A Se_on(2,0x0E) vor dem Aufklappen (@0x0135A For 20),
 *      @0x01376 Se_on(2,0x0D), wenn das Gitter in die Ruhelage zurueckschlaegt -> 1190 sub11 (Luke obj 4, je Hund):
 *      0x0E beim Beginn des Aufschwungs @0x0286A, 0x0D im Bild nach dem letzten Rueckschritt @0x0288C.
 *   P3 RE2 ROOM1120 (Bibliothek 2F) sub04: @0x02F2E Se_on(2,0x0C) direkt vor @0x02F58 Gosub sub08 (Regalfahrt, kein
 *      Evt_next dazwischen), EIN Se_on je Fahrt -> 1190 sub10 @0x02796 Work_set(3,5) (Schrank, erster Fahrschritt im
 *      selben Bild @0x0279E For 20).
 * Toene: Mini-Bank shared_assets/RE2/TOENE39.VBS (tools/r39_b/toene39_se_cut.py, gen/re2_toene39_bank.inc), gespielt
 * ueber den Plattform-Haken re15_toene39_se_hook (platform/pc/src/toene_r39_pc.c) und dieselbe Stimmen-/Prioritaets-
 * Maschine wie die Raum-SE (se_play_layers = FUN_800453d0/FUN_80045024-Nachbau). Nicht positional (OFFEN wie Runde 38 H).
 *
 * MECHANIK (Muster engine/src/elzatoene_r38.c): ANKER = Datei-Offset + 8-Byte-Signatur im rohen RDT, einmal je Raum
 * gesucht (Haken scd_vm.c scd_register_current_rdt); vor jedem ausgefuehrten Opcode prueft der Opcode-Haken den PC.
 */
#ifndef RE15_TOENE_R39_H
#define RE15_TOENE_R39_H

#include <stdint.h>

/* ---- Saetze der Mini-Bank TOENE39.VBS (= RE2_TOENE39_SE_* in gen/re2_toene39_bank.inc) -------------------------- */
#define RE15_TOENE39_TON_SCHUBLADE   0   /* RE2 ROOM1130 Satz 0x28 (EDT @0x11AD8, VAG @0x15A08 5216 B)            */
#define RE15_TOENE39_TON_GITTER_AUF  1   /* RE1.5 ROOM2060 snd0 Satz 0x0E (EDT @0x05B70, VAG @0x0CA38 2416 B)       */
#define RE15_TOENE39_TON_GITTER_ZU   2   /* RE1.5 ROOM2060 snd0 Satz 0x0D (EDT @0x05B6C, VAG @0x0C278 1984 B)       */
#define RE15_TOENE39_TON_REGAL       3   /* RE2 ROOM1120 Satz 0x0C (EDT @0x14CA4, VAG @0x35C44 10512 B)            */
#define RE15_TOENE39_TON_N           4

/* ---- Anker (RDT-Datei-Offsets; Signaturen in engine/src/toene_r39.c) ------------------------------------------- */
/* P1 ROOM11F0/11F1 sub19 (AOT 13 main00 @0x00CE6 sce 3 `ff 00 18 13`): @0x017B6 `29 0b` Cut_chg 11 (offene Schublade). */
#define RE15_TOENE39_SCHUBLADE_OFF   0x017B6u
/* P3 ROOM1190/1191 sub10 (Raetsel geloest, Evt_exec aus sub01 @0x02468/@0x024A8): @0x02796 `2e 03 05` Work_set(3,5)
 * Schrank obj 5 (@0x02A46 bei (3920,0,-25952)), @0x0279A Speed_set(0,-100), @0x0279E For 20 {Add_speed, 2 x Evt_next}. */
#define RE15_TOENE39_SCHRANK_OFF     0x02796u
/* P2 ROOM1190/1191 sub11 (Gosub 0x0B @0x027EA/@0x027F8/@0x02806, je Hund): @0x0286A For 10 = Aufschwung der Luke obj 4
 * (Speed_set(5,150)/(0x0B,-6) @0x02860/@0x02864), @0x0288C `34 05 00 00` Member_set(5,0) nach dem Rueckschwung. */
#define RE15_TOENE39_LUKE_AUF_OFF    0x0286Au
#define RE15_TOENE39_LUKE_ZU_OFF     0x0288Cu

/* Plattform-Haken: spielt Satz `ton` der Mini-Bank (NULL = ohne Ton, z.B. Unit-Riegel ohne Spion). */
extern void (*re15_toene39_se_hook)(int ton);

/* HAKEN scd_vm.c scd_register_current_rdt: Anker des Raums im rohen RDT suchen (Signaturpruefung). */
void re15_toene_r39_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id);
/* HAKEN scd_vm.c Opcode-Schleife: vor jedem ausgefuehrten Opcode (Wachposten g_re15_toene39_anker_n, sonst 0). */
extern int g_re15_toene39_anker_n;
void re15_toene_r39_pc(const unsigned char *pc);

/* Pruefhaken. */
int      re15_toene_r39_zaehler(int ton);          /* Ausfuehrungen je Satz seit reset                              */
int      re15_toene_r39_letzter_takt(int ton);     /* VM-Takt (g_scd.tick_count) der letzten Ausfuehrung, -1 = nie  */
unsigned re15_toene_r39_anker_off(int i);          /* Datei-Offset des i-ten armierten Ankers                       */
void     re15_toene_r39_reset(void);

#endif /* RE15_TOENE_R39_H */
