/* re15_spiderman_r38.h — Runde 38 Spur T "spiderman" (Dossier analysis/befunde_runde38/T_spiderman.md).
 *
 * Typ 0x2B = "SPIDER-MAN" (RE15Editor ScdUtilities.java:230 "2B - SPIDER-MAN - Stage 4,5"; im Port bisher
 * "TYRANT"). Nutzer (AUFTRAG.md Runde 38):
 *   1. "In Stage 4 die Spiderman kommen nicht richtig aus den Schaechten raus wie im Original, obwohl die
 *      Schaechte korrekt hoch gehen"
 *   2. "In Stage 4 die Spiderman fuehren - mal wieder - dazu das Leon teilweise komplett von der Karte
 *      verschwindet, wenn sie einen Angreifen. Sowas darf GARNICHT passieren, bei keinen Gegner."
 *
 * Alle Adressen STAGE4.BIN (RAW @0x80100000); STAGE5 traegt dieselben Maschinen um +0x180 verschoben
 * (Root 0x80111A50, Hook A 0x80115E40, Hook B 0x8011635C).
 *
 * (1) AUSTRITT (Zustand 4, Untertabelle @0x8011A270 [+0x5]): [1] 0x801152EC (grid 0x41) / [0] 0x8011502C
 *     (grid 0x40). P0 versteckt (Teile-Bit 0 aus, word0 |= 2|0x40, Schatten 0), P1 WARTET auf grid 0x42
 *     (das Raumskript setzt es, wenn der Deckel hochfaehrt), P2 spielt Clip 0x1D/0x1C: sichtbar ab Bild 6,
 *     ab Bild 0x21 (bzw. 0x32) greifbar mit Schatten und grid 0, Clip-Ende -> ACTIVE +0x5 = 1.
 * (2) GRIFFE: B[5]/B[9] setzen den GEMEINSAMEN ANKER (FUN_8001ac38) und kopieren ihn auf den Spieler — der
 *     Port tat das nicht, der Spieler wurde vom alten Anker aus platziert (gemessen 14 600 Einheiten weit).
 *     B[15] setzt den Spieler EINMAL auf die Gegnerlage + 0x66C hinter dessen Blickrichtung und platziert
 *     danach nie wieder (Spieler-Hook A [2]/[3] 0x80115FF0 ohne FUN_8001ad68). */
#ifndef RE15_SPIDERMAN_R38_H
#define RE15_SPIDERMAN_R38_H
#include <stdint.h>
#include "re15_actor.h"

#define RE15_R38_SM_TYP 0x2Bu

/* ---- Zustand 4 (AUSTRITT) ---- */
/* Tick des Austritts fuer +0x5 = 0/1. Rueckgabe 1 = behandelt; 0 = andere Variante (grid 0x43, +0x5 = 2,
 * in keinem RDT gesetzt) -> der bisherige Port-Pfad bleibt zustaendig. */
int  re15_r38_sm_austritt(re15_actor_t *e);
/* 1 = der Spiderman steckt noch im Schacht bzw. kommt gerade heraus (grid & 0x60): kein Ziel fuer den
 * Aim-Latch (FUN_8003703c `andi v0,a0,0x60` @0x8003710c) und ohne Hoehenband fuer den Treffer-Resolver
 * (INIT @0x80111C14-20 loescht es, gestempelt wird nur im ACTIVE-Schwanz @0x80111DCC-E24). */
int  re15_r38_sm_versteckt(const re15_actor_t *e);
/* 1 = kein Schatten (P0 +0xBC/+0xBE = 0 @0x80115370-74 bis Bild 0x21 @0x80115548-54). */
int  re15_r38_sm_schatten_aus(const re15_actor_t *e);

/* ---- Griffe ---- */
void re15_r38_sm_griff1(re15_actor_t *e, re15_actor_t *pl, int hinten);   /* B[5] 0x80112A64 / B[9] 0x80112F58 */
void re15_r38_sm_kollaps(re15_actor_t *e, re15_actor_t *pl, int hinten);  /* B[8] 0x80112E20 / B[10] 0x80113300 */
void re15_r38_sm_griff2(re15_actor_t *e, re15_actor_t *pl);               /* B[15] 0x8011411C */

/* ---- Spieler-Opfer-Maschinen (Hook A 0x80115CC0 / Hook B 0x801161DC) ---- */
/* Clip-Belegung der Opferbank fuer den Greifer `typ` und aca59 `v`. Rueckgabe 1 = Spiderman (Clips gesetzt). */
int  re15_r38_sm_opfer_clips(uint8_t typ, uint8_t v, uint8_t *c_intro, uint8_t *c_hold,
                             uint8_t *c_release, uint8_t *c_collapse);
/* 1 = Griff-2-Opfermaschine (aca59 2/3): KEINE Platzierung, KEIN Yaw-Schreiben (0x80115FF0-0x801161D8). */
int  re15_r38_sm_griff2_opfer(uint8_t typ, uint8_t v);

/* ---- Kein Griff durch eine Wand (alle Greifer) ---- */
/* 1 = die Strecke (x0,z0)->(x1,z1) schneidet das Innere einer soliden Rechteckzelle (Typ 1, u0 & 1) des
 * Spielerbandes. Gerufen von re15_victim_place (enemy_ai_common.c) und der Griff-2-Lage. NUTZER-VORGABE,
 * Form und Belege im Funktionskopf (spiderman_r38.c). */
int  re15_r38_griff_durch_wand(int32_t x0, int32_t z0, int32_t x1, int32_t z1);

/* ---- Messzugang (Riegel) ---- */
/* Zaehler der Griff-Latches dieses Moduls seit Programmstart (Sonde). */
unsigned re15_r38_sm_latches(void);

#endif /* RE15_SPIDERMAN_R38_H */
