/*
 * Runde 38 Spur O "montage" — Leon, Szene nach Irons' Tod: in JEDEM Montage-Schnitt, in dem Zombies durchbrechen,
 * gehen ALLE Zombies auf die Kamera zu, so wie die Zombies hinter dem Rolltor (ROOM1040 Cut 1). Der Rolltor-Schnitt
 * selbst bleibt unveraendert. Dossier: analysis/befunde_runde38/O_montage.md.
 *
 * NUTZER (analysis/befunde_runde38/AUFTRAG.md, woertlich): "Bei Leon - bei der Cutscene nach Irons Tod, wo die Zombies
 * durchbrechen, sollen sich die Zombies jeweils auch Richtung Kamera bewegen, also so, wie es bei den Zombies hinter
 * dem Rolltor schon passiert. Die anderen sollen das auch machen. Das wirkt auch bedrohlicher."
 *
 * ⛔ NUTZER-VORGABE. RE1.5 hat diese Montage nicht (Port-Szene, Runde 35 Spur L, include/re15_irons_tod.h). Die FORM
 * ist die des Rolltor-Schnitts im Port, dessen drei Bausteine Original-Adressen tragen (selbst disassembliert):
 *   1. DURCHBRUCH-FORM grid 0x0d = Angriff ab INIT. STAGE1.BIN Zombie-Wurzel 0x80100424 (Typen 0x10/0x11/0x16:
 *      Installer `addiu v0,v0,1060` @0x8011e86c, `sw v0,11244(at)` @0x8011e88c / `11248` @0x8011e894 / `11268`
 *      @0x8011e8a4): `andi v0,v0,0x1f` @0x80100f28, `bne v0,v1(0xd)` @0x80100f2c, `ori v0,zero,0x201` / `sw v0,4(v1)`
 *      @0x80100f40-44. Unter der RE2-KI (Standard) = RE2-Deskriptor 6 = GANG (re2z_init `sel == 0x0d` -> 0x101;
 *      RE2 EMZ0.BIN `addiu v1,zero,6` @0x80100aac, `addiu v0,zero,257` / `sw v0,4(s2)` @0x80100ab8-abc). Die Rolltor-
 *      Zombies tragen sie (ROOM1040 main00 @0x011FC.. Typ 0x16 grid 0x0d); die Montage-Listen 1130/1030-Cut-7 trugen
 *      bis v0.8.25 die STEHENDE Form (grid 0x02 / 0x00) -> sie standen still (gemessen, Dossier "Messung vorher").
 *   2. ZIEL = der geparkte Spieler HINTER der Kamera (Navigator-Ziel = Spielerlage, enemy_ai_common.c
 *      re15_nav_update_steer; Form EXE FUN_80039e7c). Rolltor: Parkplatz (-24950,-14900) liegt 3985 hinter der Kamera
 *      Cut 1 (RID-Rechnung unten). Fuer ROOM1030 Cut 12 wird der Spieler nach dem Schnittwechsel auf die Kameraachse
 *      gesetzt (Form Work_set(1,0) + Pos_set + Member_set(4) = ROOM11B0 @0x012B2 `2e 01 00 00 32 00 ae 9a 00 00 47 ba
 *      34 04`; Pos_set LAB_80041048: `lw a1,340(a0)` @0x8004104c, `sw v0,52(a1)` / `56` / `60` @0x80041058-70).
 *   3. EINZELSTOPP HINTER DER KAMERA: entity+0x9 |= 0x20 -> Zombie-Takt uebersprungen (STAGE1.BIN `lbu v0,9(a0)`
 *      @0x80100450, `andi v0,v0,0x20` @0x80100458, `bne v0,zero,0x80100658` @0x8010045c; Port RE15_AI_GRID_SKIP vor
 *      Navigator und Gehirn). Rolltor: irons_tod_1150.c bilanz_takt (z < Kamera-z). Hier allgemein: Skalarprodukt
 *      (Lage - Kamera) . Blickrichtung < 0 = hinter der Bildebene der Kamera des LAUFENDEN Schnitts.
 *
 * KAMERAS = RDT-RID (RDT+0x24 = camera_start 0x60 in allen vier Raeumen, 32 Byte je Schnitt, re15_camera.h: +0x04 pos_x,
 * +0x0C pos_z, +0x10 target_x, +0x18 target_z); Datei-Offset je Schnitt in der Tabelle (montage_r38.c), Riegel
 * unit_r38_montage_kameras vergleicht sie mit den ausgelieferten RDT-Bytes.
 */
#ifndef RE15_MONTAGE_R38_H
#define RE15_MONTAGE_R38_H

#include <stdint.h>

/* Ein Montage-Schnitt: Raum, Cut, Datei-Offset des RID-Satzes, Kamera (x,z) und Blickziel (x,z), halt = 1: Einzelstopp
 * hinter der Kamera gilt in diesem Schnitt (ROOM1040 Cut 1 = 0: der Rolltor-Schnitt behaelt seine eigene Regel). */
typedef struct {
    uint16_t raum;
    uint8_t  cut;
    uint8_t  halt;
    uint16_t rid_off;
    int32_t  kx, kz, zx, zz;
} re15_r38_kamera_t;

int  re15_r38_kameras(const re15_r38_kamera_t **out);   /* Tabelle (Riegel) */

/* PARKPLATZ ROOM1030 Cut 12 (Baustein 2): auf der Kameraachse Cut 12 im Abstand der Rolltor-Form hinter der Kamera.
 * Abstand = (Parkplatz 1040 - Kamera Cut 1) . Blickrichtung Cut 1 = ((-24950+26262)*1242 + (-14900+10692)*8874) /
 * |(1242,8874)| = -35712288 / 8960,5 = -3985,5 (RID ROOM1040 @0x80: pos (-26262,-10692) @0x84/@0x8C, target
 * (-25020,-1818) @0x90/@0x98; re15_irons_tod.h RE15_IT_PARK_1040_X/_Z). Achse Cut 12 = (3150,-11196) / 11630,7
 * (RID ROOM1030 @0x1E0: pos (-11790,-15282) @0x1E4/@0x1EC, target (-8640,-26478) @0x1F0/@0x1F8):
 * (-11790 - 3985,5 * 0,27083, -15282 + 3985,5 * 0,96263) = (-12869,-11445). Gierung 845 = Blick zum Tor entlang der
 * Achse (Gierung 0 = +x, 1024 = -z: atan2(0,96263, 0,27083) = 74,3 Grad = 845/4096). Riegel unit_r38_montage_kameras
 * rechnet beides aus den RDT-Bytes nach. */
#define RE15_R38_PARK_1030_12_X    (-12869)
#define RE15_R38_PARK_1030_12_Z    (-11445)
#define RE15_R38_PARK_1030_12_YAW  845

/* HAKEN re15_irons_tod_tick (jedes Bild, Kopf des VM-Laufs): Einzelstopp hinter der Kamera (Baustein 3) in den
 * Schnitten mit halt = 1 waehrend ihres Montage-Schritts, dazu die Messung je Schnitt (s.u.). */
void re15_r38_montage_takt(void);

/* MESSUNG (kein Spielverhalten): je Schnitt-Abschnitt (Raum, Cut, zusammenhaengende Bilder) und Zombie der Abstand
 * zur Bodenprojektion der Kamera |(x,z) - (Kamera x,z)| und die Tiefe (Skalarprodukt mit der Blickrichtung, normiert)
 * im ersten und letzten Bild des Abschnitts; angehalten = Abschnitts-Bild des Einzelstopps (0 = nie). debug.log-Zeile
 * `[montage-r38] ROOMxxxx Cut c ...` beim Ende jedes Abschnitts. Reset beim Szenenstart (re15_irons_tod_install). */
typedef struct {
    uint16_t raum;
    uint8_t  cut;
    uint8_t  aktor;
    uint8_t  typ;
    uint8_t  bit;          /* em_flag_id (Sce_em_set pc[7]) */
    int      bild0, bild1; /* Abschnitts-Bild 1.. des ersten/letzten Sehens */
    int32_t  d0, d1;       /* Abstand zur Kamera-Bodenprojektion */
    int32_t  t0, t1;       /* Tiefe vor der Kamera */
    int      angehalten;   /* Abschnitts-Bild des Einzelstopps, 0 = nie */
    uint8_t  halt0;        /* 1 = beim ersten Sehen im Abschnitt schon angehalten (Stopp aus einem frueheren Schnitt) */
} re15_r38_spur_t;
int  re15_r38_spuren(const re15_r38_spur_t **out);
int  re15_r38_abschnitt_bilder(uint16_t raum, uint8_t cut);   /* Laenge des (letzten) Abschnitts in Bildern */
void re15_r38_montage_reset(void);

/* Nur fuer Sonden/Riegel: 0 = Stand v0.8.25 (stehende Listen, kein Einzelstopp ausser im Rolltor-Schnitt, kein
 * Umparken in Cut 12) als Gegenprobe im selben Binary. Wirkt beim naechsten Programmbau. */
void re15_r38_montage_enable(int an);
int  re15_r38_montage_enabled(void);

#endif /* RE15_MONTAGE_R38_H */
