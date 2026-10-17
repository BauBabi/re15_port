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
 *      re15_nav_update_steer; Form EXE FUN_80039e7c). ⛔ Nur wenn der Parkplatz in einer BLK-Zone liegt: gleiche Zone ->
 *      Ziel = Spielerlage jeden Takt (`bne a0,a1` @0x80039fbc, `sh s1,444(v0)` @0x80039fd0); sonst Pfadsuche FUN_8003a524
 *      und UNBEDINGTE Kopie der geteilten Globals @0x8003a040/@0x8003a058 — ausserhalb jeder Zone (0xff @0x8003a198)
 *      bleibt dort das Ergebnis eines frueheren Raums stehen (GEMESSEN, Dossier "Zwischenmessung" Befund 1).
 *      Rolltor: Parkplatz (-24950,-14900) liegt 3985 hinter der Kamera
 *      Cut 1 (RID-Rechnung unten). Fuer ROOM1030 Cut 12 wird der Spieler nach dem Schnittwechsel auf die Kameraachse
 *      gesetzt (Form Work_set(1,0) + Pos_set + Member_set(4) = ROOM11B0 @0x012B2 `2e 01 00 00 32 00 ae 9a 00 00 47 ba
 *      34 04`; Pos_set LAB_80041048: `lw a1,340(a0)` @0x8004104c, `sw v0,52(a1)` / `56` / `60` @0x80041058-70).
 *   3. EINZELSTOPP HINTER DER KAMERA: entity+0x9 |= 0x20 -> Zombie-Takt uebersprungen (STAGE1.BIN `lbu v0,9(a0)`
 *      @0x80100450, `andi v0,v0,0x20` @0x80100458, `bne v0,zero,0x80100658` @0x8010045c; Port RE15_AI_GRID_SKIP vor
 *      Navigator und Gehirn). Rolltor: irons_tod_1150.c bilanz_takt (z < Kamera-z). Hier allgemein: Skalarprodukt
 *      (Lage - Kamera) . Blickrichtung < 0 = hinter der Bildebene der Kamera des LAUFENDEN Schnitts.
 *   4. DURCH DAS AUFGEBROCHENE TOR NUR KRIECHEND (ROOM1030 Cut 12). Die Torzelle SCA 18/28 (@0x01AE8/@0x01B60, z
 *      -24420..-22771) wird in der Szene mit `37 02 06 f7` / `37 03 06 f7` (sub08 @0x0278E/@0x02792, Sca_id_set u0 0xff
 *      -> 0xf7) nur fuer Kriecher frei; aufrechte Zombies stehen an ihr fest (GEMESSEN Lauf x_kette: z -24838). Das
 *      Original laesst im Spiel JEDEN Zombie kriechen, der im Warte-Rechteck hinter dem Tor steht: ROOM1030 sub06
 *      @0x024C8.. je Gegner-Index i = 0x00..0x13 (`21 05 i 00` Ck(5,i)=0, `2e 02 i` Work_set, `3e 00 0f 00 05 00`
 *      Member_cmp AOT-Stempel == 5, `18 07` Gosub sub07, `22 05 i 01` Set(5,i)=1); sub07 @0x02754 `3d 04 10` /
 *      `26 00 05 04 00 10` / `35 10 04` = member 0x10 (entity+0x1c4) |= 0x1000. Die Szene selbst (sub08) ruft nur
 *      viermal sub09 (dieselbe Regel, ein Zombie je Aufruf, `59 05 04 01` @0x02822 = Set(5,i)). NUTZER-VORGABE "alle"
 *      -> im Montage-Schritt auf Cut 12 gilt die sub06-Regel fuer jeden Zombie (ohne die Spiel-Drossel sub02 @0x021CC
 *      "weniger als 4 im Hauptraum" und ohne das Spiel-Bit (5,0x22) @0x024CC, die der Montage-Ablauf nicht setzt).
 *   5. DIE ZOMBIES HINTER DEM TOR WARTEN AUF IHREN SCHNITT (ROOM1030, Cut-7-Teil). GEMESSEN (Dossier "Dritter Lauf"): liefen
 *      sie schon waehrend Cut 7 (Kamera auf dem 1070-Gang) los, stauten sie sich 120 Bilder am geschlossenen Tor vor dem
 *      Zone-9-Uebergang; einer wurde westlich der Klemmlinie von SCA 30 (@0x01B78, x -12566 + Radius 400) geschoben und kam
 *      in Cut 12 nie durch (+120 von der Kamera weg; FUN_8003bca8 code 0 nimmt jeden Schritt in die Flaeche zurueck). Form
 *      ROOM1141 sub02: `2e 02 01 00 34 0c 30 00 2e 02 02 00 34 0c 30 00` @0x00E6C (Work_set(2,i) + Member_set(12,0x30) =
 *      entity+0x9 Bit 0x20) unmittelbar vor `29 00` Cut_chg 0 @0x00E80, Freigabe `34 0c 10 00` @0x00F64/@0x00F74; Wirkung
 *      @0x80100450-5c (s. 3). Hier: solange der 1030-Schritt NICHT Cut 12 zeigt, haelt jeder Zombie in BLK-Zone 0/10 (hinter
 *      dem Tor) an; mit dem ersten Bild von Cut 12 laufen genau diese los — wie in 1130 Cut 0, 1040 Cut 1 und 1030 Cut 7,
 *      deren Zombies mit IHREM Schnitt losgehen.
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

/* BAUSTEIN 4 (Kriechen durchs Tor, ROOM1030 Cut 12) — Werte aus ROOM1030.RDT sub06/sub07 (Datei-Offsets oben). */
#define RE15_R38_KRIECH_CUT      12        /* Cut_chg 12 @0x027AA (sub08) = das aufgebrochene Tor */
#define RE15_R38_WARTE_STEMPEL   5         /* `3e 00 0f 00 05 00` @0x024E0 (sub06) / @0x02804 (sub09): member 0x0F == 5 */
#define RE15_R38_KRIECH_BIT      0x1000u   /* `26 00 05 04 00 10` @0x02758 (sub07): Calc ODER 0x1000 auf member 0x10 */
#define RE15_R38_KRIECH_BANK     5         /* `22 05 i 01` @0x024E8.. (sub06) / `59 05 04 01` @0x02822 (sub09) */
#define RE15_R38_KRIECH_N        20        /* sub06 prueft die Indizes 0x00 (@0x024D4) .. 0x13 (@0x02734) */

/* BAUSTEIN 5 (Zombies hinter dem Tor warten auf ihren Schnitt, ROOM1030) — BLK-Zonen hinter dem Tor (ROOM1030.RDT BLK
 * @0x01BB8, 12 Byte je Knoten): Zone 0 @0x01BBC (x -13470..-3660, z -32760..-23860) und Zone 10 @0x01C34 (x -21290..-13470,
 * z -31360..-26960, Nachbar nur Zone 0). */
#define RE15_R38_ZONE_HINTER_TOR_A  0
#define RE15_R38_ZONE_HINTER_TOR_B  10

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
    uint8_t  kriech0;      /* 1 = beim ersten Sehen im Abschnitt schon mit Kriech-Bit (sub09 der Szene) */
    int      kriecht;      /* Abschnitts-Bild, in dem Baustein 4 das Kriech-Bit setzte, 0 = nie */
    int32_t  x1, z1;       /* Lage im letzten Bild des Abschnitts */
    int32_t  dm;           /* Abstand zur Kamera-Bodenprojektion in der Mitte der eigenen Sichtzeit ((bild0+bild1)/2,
                            * auf das 16-Bilder-Raster abgerundet) — "nimmt ueber die Schnittdauer ab" = d0 > dm > d1 */
    int      warte;        /* Abschnitts-Bild, in dem er AUFRECHT im Warte-Rechteck stand (AOT-Stempel 5), 0 = nie */
} re15_r38_spur_t;
/* Je Abschnitt: Laenge, Spielerlage im letzten Bild, kleinste Spieler-HP, Zahl der Bilder mit einem Navigator-Ziel
 * ungleich der Spielerlage bei einem Zombie in DERSELBEN BLK-Zone wie der Spieler (FUN_80039e7c gleiche Zone
 * @0x80039fbc -> Ziel = Spielerlage; > 0 = veraltetes/fremdes Ziel), Kriech-Regel-Treffer. */
typedef struct {
    uint16_t raum;
    uint8_t  cut;
    int      bilder;
    int32_t  px, pz;
    int      hp_min;
    int      ziel_fremd;
    int      kriech;
    uint8_t  ki_re2;        /* 1 = die Zombies laufen mit dem RE2-Gehirn (Standard-KI, re15_ai_re2_for_type), 0 = RE1.5 */
} re15_r38_abschnitt_t;
int  re15_r38_abschnitte(const re15_r38_abschnitt_t **out);
int  re15_r38_fremde_kamera(void);   /* Bilder eines Montage-Schritts auf einer Kamera ausserhalb der Schnitt-Tabelle */
void re15_r38_abschnitt_beenden(void);   /* Riegel: laufenden Abschnitt schliessen (exe: schliesst der Raumwechsel) */
int  re15_r38_spuren(const re15_r38_spur_t **out);
int  re15_r38_abschnitt_bilder(uint16_t raum, uint8_t cut);   /* Laenge des (letzten) Abschnitts in Bildern */
void re15_r38_montage_reset(void);

/* Nur fuer Sonden/Riegel: 0 = Stand v0.8.25 (stehende Listen, kein Einzelstopp ausser im Rolltor-Schnitt, kein
 * Umparken in Cut 12, keine Kriech-Regel auf Cut 12) als Gegenprobe im selben Binary. Wirkt beim naechsten Programmbau
 * (Listen, Umparken) bzw. sofort (Einzelstopp, Kriech-Regel). Den Tuer-Parkplatz RE15_IT_PARK_1030 setzt der Riegel
 * in der Gegenprobe selbst auf den alten Wert. */
void re15_r38_montage_enable(int an);
int  re15_r38_montage_enabled(void);

#endif /* RE15_MONTAGE_R38_H */
