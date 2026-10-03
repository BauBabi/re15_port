/* re15_granate_r35.h — Runde 35 Spur A: die Handgranate (Id 0x09/0x0A/0x0B) im Flug gegen Waende
 * und Gegner, die Reichweite ihrer Explosion und ihr Explosionston.
 *
 * EINORDNUNG (Beta -> Retail, VERTRAG §2.2): RE1.5 fuehrt den Wurf vollstaendig (Routinen 30/29/31
 * @0x8001843c/@0x80018320/@0x8001854c, re15_esp.c), ist beim FLUG aber nachweislich unfertig —
 * Routine 29 kennt nur die Ebene Welt-y > 0 (`lh t1,42(t0)` / `blez t1` @0x80018330-38), weder
 * sie noch Routine 30/31 noch der Tick FUN_80019e20 rufen die Raumkollision (kein `jal 0x8001c6e8`,
 * kein `jal 0x8003b0a4`): die Granate fliegt durch Waende (gemessen Runde 35 W1: 7 Bilder in einer
 * soliden SCA-Zelle). Das vollstaendige Retail-Geschoss ist die RE2-Granatwerfer-Runde:
 * FUN_8001ED9C (Flug: Wand @0x8001ef80-0x8001f104, Gegner-Kontakt @0x8001ee90-ef14) und Op 47
 * @0x80020c3c (Explosion: Box @0x80010918, SE 0x01110001 @0x80020d40-48, zwei Pruefhoehen
 * @0x80020d78/@0x80020db0). Dossier: analysis/befunde_runde35/A_granate.md.
 *
 * Die drei RE1.5-Routinen bleiben byte-true; dieses Modul haengt an zwei Stellen von re15_esp.c:
 *   - Routine 29 (vor dem Bodentest): re15_granate_r35_flug (Wand -> Rueckzug -> Explosion sofort)
 *   - Routine 31 (Zuender 7):          re15_granate_r35_explosion / _explosion_se
 */
#ifndef RE15_GRANATE_R35_H
#define RE15_GRANATE_R35_H

#include <stdint.h>
#include <stdio.h>
#include "re15_esp.h"
#include "re15_rdt.h"

/* ---- RE2-Konstanten (info/re2leon/PSX.EXE, selbst disassembliert `re2_disasm.py`) ------------ */

/* Box des Flugkontakts {-1400, 0, 350, 250}: `lui a2,0x8001 / addiu a2,a2,2304` @0x8001edb8-bc ->
 * 8 Byte @0x80010900 `88 fa 00 00 5e 01 fa 00`. */
#define RE15_GRANATE_R35_BOX_KONTAKT   { -1400, 0, 350, 250 }
/* Pruefhoehen des Flugkontakts: y + 1000 (`addiu v0,v0,1000` @0x8001eec8), dann y - 1000
 * (`addiu v0,v0,-2000` @0x8001eef8). */
#define RE15_GRANATE_R35_KONTAKT_DY    1000
/* Box der Explosion {-2000, 0, 1000, 500}: `lui a2,0x8001 / addiu a2,a2,2328` @0x80020c58-5c ->
 * @0x80010918 `30 f8 00 00 e8 03 f4 01`; gewaehlt ueber +0x1E == 12 (`addiu a2,a2,-96` @0x80020d74
 * mit 12*8). FUN_80041EF8 (re2gl_box_test): Ecke P + RotY(Gier)*(-2000,0,-2000), Kanten 4000/4000
 * -> Quadrat +-2000 um P. */
#define RE15_GRANATE_R35_BOX_EXPLOSION { -2000, 0, 1000, 500 }
/* Zweite Pruefhoehe der Explosion: P.y + 900 (`addiu v0,v0,900` @0x80020d98, `sw v0,20(sp)`
 * @0x80020d9c, `jal 0x800470c0` @0x80020db0). */
#define RE15_GRANATE_R35_EXPLOSION_DY  900
/* Explosions-SE: `lui a0,0x111` @0x80020d40 / `ori a0,a0,0x1` @0x80020d44 / `jal 0x8005ba28`
 * @0x80020d48 = Bank 1 (ARMS der ausgeruesteten Waffe = RE2 ARMS09) Record 0x11 -> ARMS09.EDH @0x44
 * `00 00 33 20` -> VAG 3 (12800 B). RE1.5 liefert dieselbe Bank als SOUND/ARMS0F (.VB md5
 * 786ad6910be7a9ea8bf1df0b145ad55b = RE2 ARMS09.VB; ARMS0F.EDH @0x28 Record 0x0A `00 00 33 20`). */
#define RE15_GRANATE_R35_SE_EXPLOSION  0x01110001u
/* Wand-Maske des Zelltests: u0 & 1 = die Zellklasse, die den SPIELER stoppt (FUN_8003b0a4 mit
 * a2 = 1, Port re15_collision_constrain PR/1u). PORT-WAHL: RE2 FUN_8004fba0(&Lage, 2, 0x2000, 0)
 * @0x8001ee60-a0 testet eine EIGENE Geschoss-Klasse (Bit 0x2000 im Wort Zelle+8, `and v0,v1,t0`
 * @0x8004fdc0-d0; Spieler/Gegner laufen mit 0x8000/0x4000 @0x800376cc/@0x80036930). Die RE1.5-Zelle
 * kennt nur u0 = 01/02/04/fb/fd/ff (Zensus Dossier N1.1) ohne Geschoss-Klasse; die Granate haelt an
 * derselben Wand wie Leon. */
#define RE15_GRANATE_R35_WAND_MASKE    1u

/* Flug-Haken der Routine 29: EIN Aufruf am Kopf von esp_fx_dispatch_b_29 (re15_esp.c). Rueckgabe 1 =
 * Wand getroffen, Rueckzug + Explosion sind erledigt (der Aufrufer kehrt zurueck); 0 = frei, Routine 29
 * laeuft unveraendert weiter. */
int  re15_granate_r35_flug(re15_esp_fx_t *f);
/* Flugtest je Bild (Routine-29-Kontext, nach der Weltlage). Rueckgabe:
 *   0 = frei, 1 = Wand (RE2 DAT_800dcbc8 != 0 @0x8001ef84-8c). Der RE2-Gegnerkontakt (`bne s0,zero`
 *   @0x8001ef14) ist fuer die Handgranate NICHT verdrahtet — gemessen, Begruendung in granate_r35.c. */
int  re15_granate_r35_flugtest(const re15_esp_fx_t *f);
/* Nur der Wandteil: STRECKE vorige Weltlage (im Wurfbild: Werfer) -> neue Weltlage gegen die soliden
 * Zellen des Werfer-Bandes, formgenau (Typ 1..9), je Quadrant die eigene Liste. */
int  re15_granate_r35_wand(const re15_esp_fx_t *f);
/* Die Geometrie darunter (fuer Sonden): Strecke bzw. Punkt gegen die Zellen eines Raums. 1 = blockiert. */
int  re15_granate_r35_strecke(const re15_rdt_t *rdt, int32_t x0, int32_t z0, int32_t x1, int32_t z1,
                              int band, unsigned maske);
int  re15_granate_r35_punkt(const re15_rdt_t *rdt, int32_t x, int32_t z, int band, unsigned maske);
/* Der RE2-Flugkontakt (FUN_8001ED9C @0x8001ee90-ef14) als reiner Geometrietest — fuer Sonden/Dossier,
 * im Flug nicht verwendet (s. granate_r35.c). Zaehlt s_n_kontakt bei jedem Aufruf. */
int  re15_granate_r35_kontakt(const re15_esp_fx_t *f);
/* Rueckzug aus der Wand (RE2 @0x8001efd4-0x8001f0c8 auf den RE1.5-Slot-Feldern +0x10 vel /
 * +0x08 acc / +0x34 xlat): vel -= 2*acc; xlat -= (vel - acc) + (vel - 2*acc)/3 je Achse. */
void re15_granate_r35_rueckzug(re15_esp_fx_t *f);
/* Explosion an P (= Liegestelle, y - 500, @0x800185a8) mit Gier und Resolver-Art: RE2-Reichweite
 * (Box @0x80010918 an P und P+900) -> RE1.5-Gegnerzweig von FUN_80012d60 (Gate B, Richtung, Schaden
 * E4, RE2-Stempel). Rueckgabe = Zahl der getroffenen Gegner. KEIN Spielertreffer (RE2 FUN_800470C0
 * laeuft nur ueber die Gegnerliste 0x800CFBF3 @0x800470c4-0x8004740c). */
int  re15_granate_r35_explosion(const int32_t p[3], int16_t gier, uint8_t art);
/* Explosions-SE 0x01110001 ueber den RE2-FX-Tonhaken (re2fx_se_hook; Plattform -> ARMS0F Satz 10). */
void re15_granate_r35_explosion_se(const int32_t p[3]);
/* MESSSCHIENE (kein Verhalten): Zaehler der Ausloeser seit Programmstart. */
void re15_granate_r35_zaehler(unsigned *wand, unsigned *kontakt, unsigned *explosionen);
void re15_granate_r35_zaehler_reset(void);

/* Dienste aus re15_esp.c fuer dieses Modul (dort je eine Zeile "Runde 35 Spur A"): Weltlage des Platzes
 * neu rechnen (@0x8001a118-2a4), Routine A des Platzes im selben Tick laufen lassen (Tabelle 0x80071d40),
 * RE15_GRANATE_LOG-Datei + ESP-Tick (Diagnose). */
void  re15_esp_r35_weltlage(re15_esp_fx_t *f);
void  re15_esp_r35_routine_a(re15_esp_fx_t *f);
FILE *re15_esp_r35_log(unsigned *tick);

#endif /* RE15_GRANATE_R35_H */
