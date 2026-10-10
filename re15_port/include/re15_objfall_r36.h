/*
 * re15_objfall_r36.h — OBJEKT-FALL der Typ-4-Objekte (Kisten/Tritte), Runde 36 Spur A,
 * Nachbesserung 3 (Abnahme 2, Mangel 1+2; Orchestrator-Entscheidung "byte-true portieren").
 *
 * Original (info/Re1.5/PSX.EXE = Auslieferungsstand, @0x80026e4c `08 00 e0 03`; alles selbst
 * disassembliert mit .claude/skills/re15-psx-disasm/scripts/re15_disasm.py, Belege im Dossier
 * analysis/befunde_runde36/A_klettern.md, Abschnitt "Nachbesserung 3"):
 *
 *   Objekt-Pass FUN_8002bd44 (Haupt-Loop @0x8001ce14), je Pool-Eintrag RUECKWAERTS
 *   (@0x8002bdb0-c8, Pool 0x800b3f98, Stride 148):
 *     @0x8002bde0-be08  aktiv -> Typ-Handler aus der Tabelle @0x80073c70[obj+0x08], VOR Schub und
 *                       Ausschiebung. [1..3] = `jr ra`, [4] = FUN_8002c40c, [5] = `jr ra` @0x8002cab4.
 *                       FUN_8002c40c: obj+4 == 1 -> `jal 0x8002c444` @0x8002c42c (der FALL).
 *     ... Schub/Ausschiebung (aot_common.c re15_prop_push_tick, re15_collision_objects) ...
 *     @0x8002c0fc-c14c  AUSLOESER: Typ 4 UND obj+4 == 0 UND FUN_8003b7f0(&obj+0x34, 450,
 *                       obj+0x82) & 2 -> FUN_8002d3d8(obj) (Fallrichtung), obj+4 = 1, obj+5 = 0.
 *     @0x8002c154       obj+0x88 = 0 (Delay-Slot der Schleife, JEDER Pool-Eintrag).
 *   AUTO-Scan FUN_800436a8 (@0x8001ce1c, Objekt-Pool @0x80043768-b4, FUN_80042bac a1 = 4, a2 = 0):
 *     WASSER-Zone (sce 8, Handler @0x8007469c[8] = 0x8004330c `lhu v0,0(a0); sh v0,136(entity)`)
 *     stempelt obj+0x88 = Wasserspiegel (Nutzlast u16@0). Der Fall liest ihn im Folgebild.
 *
 * Der Port fuehrt obj+4/+5/+6/+0x88 in objfall_r36.c je Prop-Platz (g_scd.props[] hat diese
 * Felder nicht); Band/Lage/Drehung schreibt der Fall direkt in g_scd.props[].
 */
#ifndef RE15_OBJFALL_R36_H
#define RE15_OBJFALL_R36_H

#include <stdint.h>
#include "re15_rdt.h"

/* Raumaufbau (scd_room_setup.c, Ende des Installer-Blocks): Fallzustand aller Plaetze auf 0. */
void re15_objfall_install(uint16_t room_id);

/* Haken 1 — Objekt-Pass-Anfang (game_step_common.c, direkt VOR re15_prop_push_tick):
 * die Typ-Handler @0x8002bde0-be08 (Typ 4 mit obj+4 == 1 -> FUN_8002c444). */
void re15_objfall_vor_schub(const re15_rdt_t *rdt);

/* Haken 2 — Objekt-Pass-Ende (game_step_common.c, hinter re15_actor_prop_pushout, JEDES Bild):
 * Typ-Handler, falls sie in diesem Bild noch nicht liefen (Zweige ohne Schub), dann je Platz der
 * Ausloeser @0x8002c0fc-c14c und obj+0x88 = 0 @0x8002c154, danach der WASSER-Stempel des
 * AUTO-Scans (@0x8001ce1c -> LAB_8004330c) auf die Objekte. */
void re15_objfall_nach_pass(const re15_rdt_t *rdt);

/* ---- Mess-/Testzugriff (keine Spiellogik) -------------------------------------------------- */
typedef struct {
    uint8_t  fall;        /* obj+0x04 */
    uint8_t  zustand;     /* obj+0x05 (0 Init, 1 Kippen/Fallen, 2 Landen, 3 Nachfedern) */
    uint8_t  zaehler;     /* obj+0x06 */
    int16_t  wasser;      /* obj+0x88 */
    int16_t  boden;       /* DAT_8008f694 (FUN_8001c6e8 bei Bild 13) */
    int16_t  winkel;      /* DAT_8008f690 (aufsummierte Kippung, Ziel 1024) */
    uint8_t  spritzer;    /* DAT_8008f6a0 */
    int16_t  richt_x;     /* DAT_800b526c */
    int16_t  richt_z;     /* DAT_800b527c */
} re15_objfall_info_t;
int  re15_objfall_info(int prop_idx, re15_objfall_info_t *out);   /* 0 = ok */
/* Zaehler fuer Riegel: Ausloesungen, Landungen (Zustand 1 -> 2), Spritzer (12er-Kranz). */
unsigned re15_objfall_n_ausloeser(void);
unsigned re15_objfall_n_landungen(void);
unsigned re15_objfall_n_spritzer(void);
/* Die Mathematik-Zwillinge (fuer Sonden): FUN_8002dacc auf einen Prop anwenden. */
void re15_objfall_drehe(int prop_idx, const int16_t delta[3]);

#endif /* RE15_OBJFALL_R36_H */
