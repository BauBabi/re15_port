/*
 * re15_zopf_r38.h — Elzas Zopf (Runde 38 Spur D, Dossier analysis/befunde_runde38/D_zopf.md).
 *
 * Das Original RE1.5 traegt Elzas Zopf vollstaendig: PL04..PL07.MD1 haben 21 Teile (EMR 15 Knochen),
 * die Teile 17..20 sind vier Zopfglieder, deren Weltmatrix NICHT aus dem Skelett kommt (Flag 0x40,
 * FUN_8001e9ec `andi 0x40` -> keine FUN_80022da0-Verkettung), sondern aus einer Pendel-Routine im
 * Spieler-Dispatcher:
 *   Init     Kommando 0 @0x800318f8 (Tabelle @0x80073f90[0]), Elza-Zweig @0x80031a78-80031bf4
 *   Treiber  FUN_80024c30, gerufen @0x80031d98 hinter `andi v0,v0,0x4` @0x80031d8c (Modell-Bit 2 = Elza)
 *   Glied    FUN_80024e40 (je Glied einmal, vier Aufrufe @0x80024d50/d98/dd0/e08)
 * RE2 fuehrt fuer Claire dieselbe Routine (FUN_8001844c/FUN_80018728, Aufrufer @0x8003bf10).
 *
 * Der Port rechnet die Zopf-Lage im Zeichenpfad, direkt nachdem die Spielerpose fuer das Bild steht
 * (dort liegen die Teil-0/Teil-8-Matrizen vor), aber nur so oft, wie der Spielschritt den
 * Spieler-Dispatcher-Rumpf durchlaufen hat (re15_zopf_r38_takt; im Original sitzt der Aufruf in
 * FUN_80031c44 hinter dem Spieler-Freeze `bltz` @0x80031c78).
 */
#ifndef RE15_ZOPF_R38_H
#define RE15_ZOPF_R38_H

#include <stdint.h>

#define RE15_ZOPF_GLIEDER     4    /* vier FUN_80024e40-Aufrufe @0x80024d50..@0x80024e08 */
#define RE15_ZOPF_ERSTES_TEIL 17   /* erstes Glied = Teil 17: Teile-Feld + 0xb6c (= 17*0xac) @0x80024c84 */
#define RE15_ZOPF_KOPF_TEIL   8    /* Elternmatrix des 1. Glieds = Teil 8 + 0x40 (Teile + 0x5a0) @0x80024c98 */
#define RE15_ZOPF_MIN_TEILE   21   /* MD1 object_count/2 der Elza-PLDs (PL04..PL07), FUN_8001e56c */

/* Eingang des Treibers FUN_80024c30 — was das Original aus dem Spielerblock 0x800aca54 und dem
 * Teile-Feld (+0x188) liest. Matrizen Q12, Zeilen-Major [r*3+c]. */
typedef struct {
    int16_t ent_rot[3];     /* Spieler +0x68/+0x6a/+0x6c — RotMatrix(ent+0x68, ent+0x20) @0x80024c68 */
    int32_t ent_pos[3];     /* Spieler +0x34/+0x38/+0x3c = t von ent+0x20                            */
    int32_t l0_rot[9];      /* Teil 0 lokal (+0x18) — erste FUN_80022da0 @0x80024c88                  */
    int32_t l0_t[3];        /*   (Wurzel-Versatz des Bildes)                                           */
    int32_t l8_rot[9];      /* Teil 8 lokal (+0x578) — zweite FUN_80022da0 @0x80024c9c                */
    int32_t l8_t[3];        /*   (EMR-Versatz Knochen 8)                                               */
    int16_t teil0_vy;       /* Teil 0 rot.vy (+0x62)   lhu @0x80024ca8                                 */
    int16_t teil0_vz;       /* Teil 0 rot.vz (+0x64)   lhu @0x80024cb8                                 */
    int16_t teil8_vy;       /* Teil 8 rot.vy (+0x5c2)  lhu @0x80024cac                                 */
    int16_t nacken_gier;    /* Teil 8 +0x94 (Nacken-Gier)  lhu @0x80024cf4                             */
    uint8_t nacken_flags;   /* 0x800acc0c (= Spieler +0x1b8) lbu @0x80024ce0, Bit 0 sperrt die Gier   */
} re15_zopf_eingang_t;

/* Zustand eines Glieds = die Felder, die FUN_80024e40 im Teil liest/schreibt. */
typedef struct {
    int32_t off[3];         /* +0x2c/+0x30/+0x34 lokaler Versatz (Init @0x80031b44-bb8)               */
    int16_t m[9];           /* +0x40 Weltmatrix (Rotation, s16)                                        */
    int32_t t[3];           /* +0x54/+0x58/+0x5c Weltlage                                              */
    int16_t rot[3];         /* +0x60/+0x62/+0x64                                                       */
    int16_t v94, v96;       /* +0x94/+0x96 Winkelgeschwindigkeiten                                     */
} re15_zopf_glied_t;

/* PLD (neu) geladen: Teile kommen aus der FUN_8001e5b0-Vorlage (Matrix = Einheit, t = 0, rot = 0). */
void re15_zopf_r38_pld_reset(void);
/* Spieler-Kommando 0 (Raumwechsel @0x8001cbdc / PLD-Laden @0x80031518 -> Handler @0x800318f8):
 * Elza-Modelle 4,5,6 (@0x80031a88-aa0) setzen Versaetze, Geschwindigkeiten und rot.vy.
 * Die alte Lage (+0x54..) bleibt. */
void re15_zopf_r38_kommando0(int modell);
/* Spielschritt: der Spieler-Dispatcher-Rumpf lief (kein Spieler-Freeze). */
void re15_zopf_r38_takt(int lief);
/* Zeichenpfad: offene Takte abarbeiten (hoechstens einer je Bild). 1 = Schritt gerechnet. */
int  re15_zopf_r38_bild(const re15_zopf_eingang_t *in);
/* Ein Treiber-Schritt FUN_80024c30 ohne Takt-Gate (Riegel). */
void re15_zopf_r38_schritt(const re15_zopf_eingang_t *in);
/* Zopf aktiv? Modell-Bit 2 (@0x80031d8c) UND Init-Modell 4/5/6 (@0x80031a88-aa0) UND >= 21 Teile. */
int  re15_zopf_r38_aktiv(int modell, int mesh_count);
/* Zeichnen? aktiv UND mindestens ein Treiber-Schritt seit dem PLD-Laden. */
int  re15_zopf_r38_zeichnen(int modell, int mesh_count);
/* Weltmatrix des Glieds k (0..3) fuer den Zeichner (Rotation Q12 + Lage). 0 = k ungueltig. */
int  re15_zopf_r38_welt(int k, int32_t rot[9], int32_t t[3]);
/* Riegel-Zugriff auf den Zustand. */
const re15_zopf_glied_t *re15_zopf_r38_glied(int k);
/* Fangpuffer fuer die lokalen Knochen-Rotationen der Spielerpose (skeleton_common.c g_skel_lokal_rot);
 * Teil 8 = die lokale Matrix +0x578, die FUN_80024c30 @0x80024c9c verkettet. */
int32_t (*re15_zopf_r38_lokal(void))[9];
/* Zeichenpfad-Bequemlichkeit: Eingang aus (re15_actor_t*, re15_emd_skeleton_t*, re15_skel_pose_t[]) bauen
 * und re15_zopf_r38_bild rufen (void*, damit dieser Kopf ohne actor/skeleton-Koepfe auskommt). */
int  re15_zopf_r38_bild_aus_pose(const void *akteur, const void *skelett, const void *posen);
/* libgte-Nachbauten (fuer die Riegel). */
void re15_zopf_r38_rotmatrix(const int16_t r[3], int16_t m[9]);      /* RotMatrix    @0x80068098 */
void re15_zopf_r38_rotmatrix_yxz(const int16_t r[3], int16_t m[9]);  /* RotMatrixYXZ @0x800666d0 */
void re15_zopf_r38_rotmatrix_y(int32_t a, int16_t m[9]);             /* RotMatrixY   @0x800659d0 */
void re15_zopf_r38_rotmatrix_z(int32_t a, int16_t m[9]);             /* RotMatrixZ   @0x80065b70 */
int16_t re15_zopf_r38_winkel_z(int32_t x, int32_t y);                /* FUN_80025360 */
int16_t re15_zopf_r38_winkel_x(int32_t x, int32_t y);                /* FUN_800253fc */

#endif /* RE15_ZOPF_R38_H */
