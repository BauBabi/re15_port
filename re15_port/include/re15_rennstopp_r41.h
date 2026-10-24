/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Nachbesserung 3 (Orchestrator-Entscheid E-E2, Abnahme 2 Maengel 1/2):
 * RENN-TEMPO und RENN-STOPP des Originals, spielweit. Dossier analysis/befunde_runde41/E_kleinkram_b.md
 * ("Nachbesserung 3"); Ausgang Stand 635be823 (Renn-Stopp), dazu das Renn-Tempo.
 *
 * GEMESSEN (DuckStation-GDB, Dossier "Nachbesserung 2", Tempo = Spieler +0x8c = DAT_800acae0):
 *   o_renn2     Kurbel ROOM2020 im Rennen, Start Stehclip 2 (zuletzt Bild 43), Blick 1024: Tempo je Bild
 *               198 210 171 182 204 211 214, dann die Fragen; nach Pos_set @0x0A8C (3650,-2925) der Stopp 174 134 94 54 14 -26
 *               -> Leon kurbelt bei (3650,-3369).
 *   o_rennstopp freie Strecke, Blick 0: 269(*) 210 170 182 205 210 214 210 188 159 176 197, loslassen -> 157 117 77 37 -3.
 *               (*) Messartefakt: der GDB-Treiber hatte die Lage im RAM versetzt (s. "Weltlage" unten).
 * ORIGINAL (selbst disassembliert, info/Re1.5/PSX.EXE):
 *   RENN-TEMPO  ANIMATE 0x80032778 (Tabelle @0x80073ff0[2]) Fall 0 @0x800327cc-@0x80032830: Tempo := @0x80073f24[Figur*2]
 *               (200), +0x8f = 7, Clip 0, Bild 0, faellt in Fall 1 @0x80032834: anim_set @0x80032884 (a3 = 0x200), dann
 *               @0x800328b4 Bildwort & 0x2000 -> Fuss 0 (@0x800328c4), sonst Fuss 1 (@0x800328c8), @0x800328cc
 *               `jal 0x800369f8` (Modus 1) und @0x800328dc-@0x800328f4 Tempo := ((int16)Tempo >> 2) + 150.
 *               FUN_800369f8 Modus 1: Kette Entity-Matrix +0x20 (Rotation aus +0x68 @0x80036a34) * Teil 0 (@0x80036a48)
 *               * Teil 1 (@0x80036a58) * Teil 2+3f (@0x80036a7c) * Teil 3+3f (@0x80036a8c) * Teil 4+3f (@0x80036a9c), je
 *               FUN_80022da0 (Rotation A*B je Element >>12, Translation A.t + (A*(s16)B.t) >> 12); Tempo +0x8c :=
 *               SquareRoot0((s16)(x - Teil[+0x54])^2 + (s16)(z - Teil[+0x5c])^2) (@0x80036af0-@0x80036b44) — Teil+0x40.. ist
 *               die Weltmatrix des Fussteils aus dem LETZTEN Zeichnen. Das Tempo ist also der Weg des Fusses seit dem
 *               letzten Bild, geviertelt, plus 150.
 *               Pose (FUN_8001f3bc, Zaehler +0x8f VOR dem Abzaehlen, w = 0x200 * Zaehler): Wurzel = GPF12(kf, 0x1000 - w)
 *               + GPL12(vorige, w); Winkel FUN_80020510: vorige &= 0xfff (@0x80020520), kf auf +-0x800 gefaltet
 *               (@0x8002053c-@0x8002055c), LoadAverageShort12 @0x800664e0 = GPF @0x80066508 + GPL @0x80066538:
 *               floor(vorige*w/4096) + floor(kf*(0x1000-w)/4096) — zwei Abrundungen.
 *   RENN-STOPP  DECIDE 0x80073fb0[2] = 0x80032604: +6 < 2 (@0x800326e0-@0x800326ec) und NICHT (gehalten & 0x201) == 0x201
 *               (@0x800326f4-@0x80032700) oder Zielen 0x100 mit Waffe (@0x80032708-@0x80032720) -> @0x8003272c `sb 2,+6`.
 *               +6 >= 2 (@0x80032730-@0x80032760): OBEN -> 0x0101 (Gehen), UNTEN -> 0x0401, LINKS/RECHTS -> 0x0301.
 *               ANIMATE Fall 2 @0x800328f8-@0x80032910 +6 = 3, Clip 3, +0x8f = 7, weiter in Fall 3 @0x80032918: anim_set,
 *               @0x80032930-@0x80032944 Tempo -= 40, @0x8003294c `bgez` / @0x80032958 Stehen bei Tempo < 0, @0x8003295c
 *               Vorschub um das Tempo (auch den letzten, negativen Schritt). Der Stopp beginnt mit dem LETZTEN Renn-Tempo.
 * NACHGERECHNET (Sonde <sp>/E41n3/probe/renntempo_blend.c, Riegel unit_r41_kleinkram_b_takt_renntempo): mit PL00-Bindepose +
 *   PL00W03-Keyframes (der Port-Verbund der Handgun-Bank) trifft die Rechnung o_renn2 7/7 und o_rennstopp ab Bild 2 11/11.
 * PORT:
 *   - Pose: der Port-Renderer posiert erst NACH dem Spieler-Takt (re15_skel_compute_pose) und rundet die Mischung nur
 *     einmal ab. Das Modul fuehrt darum die Pose der Fusskette selbst wie FUN_8001f3bc (zwei Abrundungen) und setzt sie beim
 *     ersten Renn-Bild aus dem letzten gezeichneten Bild (prev_angles/prev_root des Aktors) auf.
 *   - Weltlage: das Original vergleicht mit der Weltmatrix des letzten Zeichnens; zwischen Zeichnen und Spieler-Takt aendert
 *     sich die Lage im Rennen nicht — der Port rechnet beide Fusslagen an der aktuellen Lage, mit der Gier des letzten
 *     Bilds (Drehen im Rennen kommt vom Aufrufer). Ein Versatz der Lage von aussen dazwischen (Skript-Pos_set,
 *     RE15_PLAYER_POS, GDB) ergaebe im Original einen grossen ersten Schritt (o_rennstopp Bild 1 = 269); der Port nicht.
 *   - Ohne gespiegelte Waffen-Bank (re15_player_w_skel/_anim NULL, z.B. Sonden ohne Bank) bleibt das Tempo der
 *     Tabellenwert aus Fall 0 (@0x80073f24 = 200), wie im Original bei Figur 0xf (@0x80032894).
 *   - Vorschub: der Port-Weg des Gehens/Rennens (player_common.c: x += cos*v/4096, z += -sin*v/4096).
 *   - Waehrend des Stopps kein Zielen (R1 wird erst im Stehen wieder gelesen, wie im Original).
 *   - Skript-Modus (Port player_mode 2, Pad gesperrt — Kurbel nach Set(2,7)/(1,27) @0x0A80/@0x0A84): das Original laesst
 *     den Spieler-Takt mit leerem Pad laufen, der Stopp laeuft durch.
 * NACHBESSERUNG 3, Fortsetzung (selbst disassembliert, Dossier "Nachbesserung 3 — Fortsetzung"):
 *   - DREHEN im Unterzustand 2: DECIDE 0x80032604 dreht in JEDEM Bild des Unterzustands 2 (Rennen +6 < 2 UND Stopp +6 >= 2)
 *     zuerst um @0x80073f25[Figur*2] (= 72): held & 0x2 @0x80032620 -> += (@0x80032654/@0x8003265c), held & 0x8 @0x80032660
 *     -> -= (@0x80032698/@0x800326a0) — VOR der OBEN/RENNEN-Pruefung @0x800326fc und VOR Zielen @0x80032708. Der Port
 *     waehlte die Rate nach dem Pad (ohne OBEN 96, OBEN ohne RENNEN 48) und drehte mit R1 gar nicht ->
 *     re15_rennstopp_r41_unterzustand2 (Aufrufer: Rate 72, Drehen auch mit R1).
 *   - OBEN im Stopp -> 0x0101 GEHEN (@0x80032738-@0x8003275c), auch mit gehaltenem RENNEN: Rennen erst im naechsten Bild
 *     aus dem Gehen-DECIDE 0x800322e8 (& 0x201 == 0x201 @0x80032444-@0x80032454 -> 0x0201). Der Dispatcher 0x80031de8 liest
 *     +5 nach dem DECIDE neu (@0x80031efc) -> das ANIMATE des neuen Unterzustands laeuft im selben Bild (Rueckgabe 2:
 *     der Aufrufer geht in diesem Bild, statt zu rennen).
 *   - STEHEN erst NACH dem Stopp: Fall 3 schreibt bei Tempo < 0 das Kommandowort 0x0001 (@0x80032950/@0x80032958); das
 *     Stehen-ANIMATE 0x80032038 Fall 0 laeuft im FOLGEbild (@0x80032074-@0x800320bc: +6 = 1, Clip 3, Bild 0 +0x95 = 0,
 *     Mischung +0x8f = 7, Tempo 0, Zeitgeber rng & 0x1f + 90). Waehrend der Stopp-Bilder laeuft also KEIN Stehen-FSM
 *     (Aufrufer: Rueckgabe 1 -> Bewegung bleibt, Leerlauf-FSM nicht betreten); im Folgebild setzt das Modul Bild 0 /
 *     Mischung 7 (re15_rennstopp_r41_takt/_skript), der Leerlauf-FSM des Aufrufers beginnt dort.
 */
#ifndef RE15_RENNSTOPP_R41_H
#define RE15_RENNSTOPP_R41_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_emd.h"

/* RENN-TEMPO je Spieler-Takt im Rennen, vor dem Vorschub (ANIMATE 0x80032778 Fall 0/1). erstes_bild = 1 im Bild, in dem
 * das Rennen beginnt (der Aufrufer setzt danach Clip 0 / Bild 0 / Mischung 7 — genau diese Pose wird gerechnet), sonst
 * gilt p->anim_frame / p->anim_frac dieses Bilds. gier_vorher = die Gier des letzten gezeichneten Bilds (der Aufrufer
 * zieht eine Drehung dieses Takts ab). tabelle = @0x80073f24 (Port RUN_SPEED_PER_FRAME). Rueckgabe = Tempo dieses Bilds. */
int32_t re15_rennstopp_r41_lauftempo(re15_actor_t *p, int erstes_bild, int gier_vorher, int32_t tabelle);

/* Renn-Stopp, Bewegungs-Block des Spielers, je Spieler-Takt VOR der Bewegungswahl: 1 = dieses Bild gehoert dem Stopp
 * (Leon ist um das Tempo verschoben, Bewegung = steh_motion), dann behaelt der Aufrufer die Bewegung, betritt den
 * Leerlauf-FSM nicht und zielt nicht. 2 = OBEN im Stopp: in diesem Bild GEHEN, auch wenn RENNEN gehalten ist
 * (0x0101 @0x80032738-@0x8003275c). 0 = kein Stopp.
 * Der Stopp beginnt mit dem letzten Renn-Tempo (re15_rennstopp_r41_lauftempo); tabelle (@0x80073f24) gilt nur, wenn noch
 * keins gerechnet wurde. waffe = Waffe in der Hand (DAT_800aca5d != 0). */
int re15_rennstopp_r41_takt(re15_actor_t *p, uint16_t pad_bits, int lauf_motion, int steh_motion, int32_t tabelle,
                            int waffe);

/* 1 = der Spieler beginnt dieses Bild im Unterzustand 2 (Rennen oder Renn-Stopp, Kommando 1): DECIDE 0x80032604 dreht
 * dann mit @0x80073f25 (72) — auch ohne OBEN und mit R1 (@0x80032620-@0x800326a0 vor @0x800326fc/@0x80032708). Vor dem
 * Drehen aufrufen (Zustand vom Ende des letzten Bilds). */
int re15_rennstopp_r41_unterzustand2(const re15_actor_t *p, int lauf_motion);

/* NACHBESSERUNG 4 (Abnahme 3 Mangel 1, Entscheid E-E4) — UEBERNAHME: der Stopp lebt im Original nur in +5 = 2 / +6 = 3
 * (@0x8003272c / @0x800328fc); jeder andere Zustand ueberschreibt sie (Treppe sce 12 @0x80043544/@0x80043594, sce 13 @0x80043610/@0x80043660, Abschluss
 * LAB_80038e50 @0x80038ea8/@0x80038eb0, Plc_ret @0x80041f90-@0x80041f9c, Plc_motion @0x80041bb0-@0x80041bc4, Griff STAGE1
 * @0x80102640 / Freigabe @0x8011c38c, Treffer/Tod @0x80012ef4-@0x80012efc). Port: (a) das Modul fuehrt die Marke +6 = 3
 * (re15_actor_t.sub_state_2) waehrend des Stopps; wer +6 schreibt (Plc_ret, Plc_motion, Raumeintritt, Treffer, Flinch-Ende),
 * beendet ihn; (b) re15_rennstopp_r41_dispatcher VOR der Zweig-Kette des Spieler-Dispatchers (game_step_common.c): lief der
 * letzte ungesperrte Durchlauf ohne den Kommando-1-Schritt (Treppe, Klettern, Griff, Tod, Knockdown, Flinch, Schieben),
 * ist der Stopp geloescht. laeuft = rdt_ok && !(g_re15_pauseflags & 0x80000000) (`bltz` @0x80031c78: gesperrte Durchlaeufe
 * lassen +4..+7 stehen). re15_rennstopp_r41_halten: Nachricht ohne Spieler-Bit (msg_block-Zweig) = keine Uebernahme. */
void re15_rennstopp_r41_dispatcher(re15_actor_t *p, int laeuft);
void re15_rennstopp_r41_halten(void);

/* Skript-Zweig (player_mode 2, ohne Pad): derselbe Stopp mit leerem Pad. 1 = Stopp aktiv (nichts weiter tun). */
int re15_rennstopp_r41_skript(re15_actor_t *p, int lauf_motion, int steh_motion, int32_t tabelle);

/* Pruefhaken */
/* NUR fuer Sonden-Gegenproben (Messung des Einflusses auf fremde Sonden, z.B. unit_r36_zombieki_affen_block): 1 = Renn-Tempo
 * und Renn-Stopp aus = Bewegung wie vor Nachbesserung 3 (Tabellen-Tempo, sofort Stehen). Kein Spielweg, keine Umgebung. */
void    re15_rennstopp_r41_pruef_aus(int aus);
/* NUR Riegel-Gegenprobe (unit_r41_kleinkram_b_takt_uebernahme): 1 = Uebernahme-Erkennung aus (Stand vor Nachbesserung 4). */
void    re15_rennstopp_r41_pruef_uebernahme_aus(int aus);
int     re15_rennstopp_r41_aktiv(void);
int32_t re15_rennstopp_r41_tempo(void);
void    re15_rennstopp_r41_zuruecksetzen(void);
/* Rechenkern (auch fuer die Sonden): Fussweg-Tempo eines Bilds aus vorgegebener Bank. Vorige Pose = Modulzustand; mit
 * neu = 1 wird sie aus vor_winkel/vor_wurzel gesetzt (NULL: die Pose dieses Bilds ungemischt). bild = Bild im Clip,
 * mischung = Zaehler +0x8f dieses Bilds. 1 = gerechnet (*tempo, *roh = SquareRoot0-Wert), 0 = Bank/Clip/Bild ungeeignet. */
int re15_rennstopp_r41_fussrechnung(const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an, int clip, int bild,
                                    int mischung, int gier_vorher, int gier, int neu,
                                    const int16_t (*vor_winkel)[3], const int32_t *vor_wurzel, int32_t *tempo, int *roh);

#endif /* RE15_RENNSTOPP_R41_H */
