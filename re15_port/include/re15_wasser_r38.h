/*
 * re15_wasser_r38.h — Runde 38 Spur N "wasser": der Wasser-Effekt beim Gehen durch Wasser.
 *
 * Nutzer (AUFTRAG Runde 38): "In Room 1260 und folgende fehlt noch der Wasser Effekt, wenn sich Leon
 * oder Elza im flachen Wasser bewegen." Dossier analysis/befunde_runde38/N_wasser.md (B1-B8).
 *
 * ORIGINAL (info/Re1.5/PSX.EXE, selbst disassembliert): die Spieler-Schrittfunktion FUN_80045630
 * (a0 = Tonversatz, a1 = Part-Index des Fusses) hat VOR dem Schritt-Ton einen Effekt-Zweig:
 *
 *   Bodentyp = FUN_800437d4(x +0x34, z +0x3c)                     `jal 0x800437d4` @0x8004566c
 *   Bit 0x80 (Wasserboden, FLR-Typ 0x97)                           `andi v0,s0,0x80` @0x800456ac
 *       -> FUN_80019700(0x06050D00, Gier +0x6a, Part[a1]+0x40, &{0,100,0})   @0x800456b8-0x80045710
 *   sonst Wasserstempel +0x88 != 0 (sce-8-Zone)                    `lh v0,136(a1)` @0x800456dc
 *       -> Bodentyp -= 3                                           `addiu s0,a0,-3` @0x800456ec
 *       -> FUN_80019700(0x06032000, 0x800, Part[a1-1]+0x40, &{0,100,0})      @0x800456e8-0x80045710
 *   Tonindex = a0 + (Bodentyp & 0x7f)                              @0x80045718-24
 *   Spieler (Typ < 0x40) auf Objekt (0x800aca3c & 0x4000) -> 9     @0x80045770-90
 *
 * Der Port spielte bisher nur den Ton (re15_audio_footstep mit dem rohen Bodentyp) — Effekt, Watsatz
 * (-3), Lauf-Versatz (a0 = 1 @0x80032850) und Objekt-Index fehlten, +0x88 wurde nie gelesen/geloescht.
 *
 * Zeitfolge (B3/B6/B7): der Schritt liegt im Geh-Executor VOR anim_set (@0x80032578) und pos_advance
 * (@0x800325ec), die Part-Weltmatrizen fuellt der Zeichner (FUN_8001e9ec) — der Schritt sieht also Lage,
 * Pose und Stempel des zuletzt gezeichneten Bilds. Der Port haelt dafuer am Anfang von re15_game_step
 * einen Schnappschuss des Spielers (re15_wasser_r38_bildanfang) und rechnet Boden, Anker und Stempel
 * daraus.
 */
#ifndef RE15_WASSER_R38_H
#define RE15_WASSER_R38_H

#include <stdint.h>
#include "re15_game_step.h"
#include "re15_skeleton.h"

/* FUN_80019700-Codes des Schritts (a0 = Effekt-Id << 24 | Sub << 16 | Skala). */
#define RE15_WASSER_R38_CODE_BODEN   0x06050D00u   /* `lui a0,0x605` / `ori a0,a0,0xd00` @0x800456b8/bc */
#define RE15_WASSER_R38_CODE_STEMPEL 0x06032000u   /* `lui a0,0x603` / `ori a0,a0,0x2000` @0x800456f0/f4 */
#define RE15_WASSER_R38_GIER_STEMPEL 0x800         /* `ori a1,zero,0x800` @0x800456e8 */
#define RE15_WASSER_R38_VERSATZ_Y    100           /* `ori v0,zero,0x64` @0x800456b4 / @0x80045700 -> sp+20 */
#define RE15_WASSER_R38_WATSATZ      3             /* `addiu s0,a0,-3` @0x800456ec */
#define RE15_WASSER_R38_BIT_WASSER   0x80          /* `andi v0,s0,0x80` @0x800456ac */
#define RE15_WASSER_R38_OBJEKT_INDEX 9             /* `ori s3,zero,0x9` @0x80045790 */

/* Bildanfang (Start re15_game_step, nach dem Positions-Spiegel): Schnappschuss des Spielers (Lage,
 * Gier, Pose = zuletzt gezeichnetes Bild), Latch des Wasserstempels +0x88 (Scan des Vorbilds,
 * @0x8001ce1c) und des Objekt-Bits 0x4000 (@0x80031d24 im Vorbild); danach Spieler+0x88 := 0
 * (`sh zero,-0x3524(at)` @0x80031da4), damit der Scan dieses Bilds neu stempelt. Zuletzt die
 * FOLGE-Plaetze (Flags-Bit 0x04, @0x80019f54-a4) auf die Part-Lage des Schnappschusses setzen. */
void re15_wasser_r38_bildanfang(const re15_game_ctx_t *c);

/* FUN_80045630-Zwilling fuer einen Spieler-Schritt. a0 = Tonversatz des Aufrufers (Echtzeit-Lauf 1
 * @0x80032850, sonst 0), fuss = Part-Index 7/4 (7 - 3*Bit12, @0x80032548-60). Spawnt den Effekt,
 * spielt den Schritt-Ton (re15_audio_footstep) und liefert den EDT-Index (Mess-Schiene). */
int  re15_wasser_r38_schritt(const re15_game_ctx_t *c, int a0, int fuss);

/* Tonversatz a0 des Original-Aufrufers aus dem Port-Zustand: Echtzeit-Lauf (Sub 2 @0x80073ff0 ->
 * 0x80032778, `ori a0,zero,0x1` @0x80032850) = 1; Gehen/Rueckwaerts/Drehen (@0x80032544/b6c/dd0) und
 * JEDER Plc-Skriptlauf (@0x80030e18/e7c, @0x80031110/280/460) = 0. */
int  re15_wasser_r38_a0(int motion, int skriptlauf);

/* ZEICHNER-FUETTERUNG (PC-Zeichner, Spieler-Block neben Knochen 11): die Weltmatrizen ALLER Parts des
 * gerade gezeichneten Bilds — R = R_y(Gier) x Pose.rot, T = R_y(Gier) x Pose.trans + Lage, dieselbe
 * Rechnung wie re15_player_set_hand_world/_rot. Das ist Part+0x40 (FUN_8001e9ec: CompMatrix(Eltern,
 * lokal) -> +0x40), den das Original beim naechsten Schritt liest. Ohne Fuetterung (PSX, Sonden)
 * rechnet re15_wasser_r38_schritt die Pose aus dem Bildanfang-Schnappschuss. */
void re15_wasser_r38_zeichner(const re15_skel_pose_t *poses, int n, const int32_t yaw_rot[9],
                              int32_t px, int32_t py, int32_t pz);

/* Mess-Schiene (Sonden/Riegel): Zahl der Effekt-Spawns seit Start je Zweig, letzter Anker. */
typedef struct {
    unsigned boden;          /* Zweig Bit 0x80 (0x06050D00)                 */
    unsigned stempel;        /* Zweig +0x88 (0x06032000)                    */
    unsigned trocken;        /* Schritte ohne Effekt                        */
    int32_t  anker[3];       /* Weltlage des letzten Spawns                 */
    int      part;           /* Part-Index des letzten Spawns               */
    int      plaetze;        /* Plaetze des letzten Spawns (Stroeme)        */
    int      edt;            /* zuletzt gespielter EDT-Index                */
    int      folge;          /* zuletzt nachgefuehrte Folge-Plaetze         */
} re15_wasser_r38_mess_t;
const re15_wasser_r38_mess_t *re15_wasser_r38_mess(void);
void re15_wasser_r38_reset(void);

/* Anker = Part-Weltmatrix * (0,100,0) + T fuer einen Aktor-Zustand (Pose aus skel/anim, Clip
 * clip_override, Bild cur). Liefert 0, oder -1 (dann out = Aktorlage). Fuer Sonden exportiert. */
int  re15_wasser_r38_anker(const void *skel, const void *anim, const void *aktor,
                           int clip_override, uint32_t cur, int part, int32_t out[3]);

#endif /* RE15_WASSER_R38_H */
