/*
 * RE1.5 Rebuilt — Beta-Birkin-Formen: Hangar-Form (ROOM50A0/50A1) und Bahnsteig-Form (ROOM5090/5091).
 *
 * ⛔ PORT-ERGAENZUNG, KEIN AUSLIEFERUNGSSTAND. Nutzerauftrag 2026-10-30 nach dem Videovergleich
 * analysis/video_80prozent/BEFUND.md (G1 + Kreatur-Standbilder V2 1:54.8 / 1:56.3). Beide Formen und beide
 * Auftritte gibt es nur im 80%-Material; jede Zahl, die hier NICHT mit @0x… belegt ist, ist aus dem Video gemessen
 * (Rueckprojektion durch die RID-Kamera) oder Gestaltung, und als solche markiert.
 *
 * Modelle: EMD/EM30_HANGAR.EMD (tools/birkin_formen.py): Clips, Knochen 0-15 und KI von EM30 / Typ 0x30, dazu eine
 * Schwanzkette (Knochen 16-22, Runde 41 Spur SF), traegt der Birkin im Hangar 50A0/50A1 UND im Labor 50E0/50F1.
 * RE2/BAHNSTEIG_FORM.EMD + .TIM (Stufe 3, analysis/befunde_runde41/form/STUFE3.md): die Riesenform J nach den zwei
 * 80%-Screenshots, 32 Knochen, eigene Clips nach dem Muster RE2-G4 (Bildzahl, Spur, Treffer-/Tonbits je Tick-Clip aus
 * CDEMD0.EMS kind 0x34), eigener Tick (engine/src/birkin_bahnsteig.c). Gewaehlt vom PC-Lader (re15_bf_emd_datei bzw.
 * re15_bf_bahnsteig_bank_faellig).
 *
 *   HANGAR (50A0/50A1, Cut 12, V1 4:36-4:40): Birkin packt einen anspringenden Spider-Man (Typ 0x2B), stemmt
 *     ihn ueber den Kopf und schmettert ihn vor sich auf den Boden (Birkin-Clip 0x0B = GRAB-THROW, Wurf bei
 *     Bild 0x2C wie im Original @0x80117d30), dann greift er den Spieler an (Uebergabe an die 0x30-KI).
 *     Runde 41 Spur H: Cut 12 ist im Raum unerreichbar (keine RVD-Zone, kein Cut_chg) - die Szene ist deshalb eine
 *     Zwischensequenz mit Riegel und Letterbox, ausgeloest vom ersten angeforderten Schnitt, der eine der Figuren zeigt;
 *     vorher gibt es die Figuren nicht (birkin_formen.c Kopf "Spur H", Dossier analysis/befunde_runde41/H_hangar.md).
 *     Ablauf Bild fuer Bild nach dem 80%-Video (Complete Disc 4, 45.0 s): Ansprung, Halten, Wurf, Spider-Man flieht
 *     rechts an der Kamera vorbei, Birkin geht auf die Kamera zu; danach 0x30-KI.
 *     Modell-Voraussetzungen (jedes EM30-Ersatzmodell): Knochen 15 = haltende Hand (Sichelhand,
 *     Runde 41 SF N1.2), Clip 0 (Stand, INIT), Clip 0x0B (Griff/Wurf,
 *     mindestens 0x2D Bilder), Clip 1 (Gang der KI); Spider-Man EM2B: Clips 0, 7, 0x0A, 0x0C.
 *   BAHNSTEIG (5080/5081): die Riesenform ab der Raumladung (Spawn sub00 @0x00746, Fall + Member_set(0x0C,0x13)
 *     @0x0083E), G4-Ablaeufe (re15_bb_tick); bei HP <= 0 Flucht (Clip 11) statt Tod, danach Flag (5,0x1c) wie der
 *     Birkin-Tod @0x8011aecc (STAGE5), damit sub01 @0x0078A weiterlaeuft.
 *   ZUG (5090/5091): die Riesenform ab der Raumladung (Spawn sub00 @0x0124A, Kampfstart sub04 Pos_set @0x12FE /
 *     Member_set(0x0c,0x13) @0x130A), G4-Ablaeufe (re15_bb_tick). Wo das Original nach dem Tod in Phase 3 auf den Raum-Morph wartet
 *     (grid&0xf==2, @0x8011a6c0 — in 5090 nie gesetzt), wechselt der Port in den RE2-G5 (Typ 0x36,
 *     enemy_ai_boss_g5.c). RE15_ENDKAMPF_NUR_G5=1 stellt den alten Ablauf her (0x30 -> 0x36 schon beim Spawn).
 */
#ifndef RE15_BIRKIN_FORMEN_H
#define RE15_BIRKIN_FORMEN_H

#include <stdint.h>
#include "re15_actor.h"

/* Raumabhaengiges EMD fuer `type` (relativ zum CD-Wurzelverzeichnis) oder NULL = Auslieferungsmodell.
 * RE15_BIRKIN_FORM=original|hangar|bahnsteig erzwingt fuer 0x30 eine Form (Testen). Typ 0x29 in
 * ROOM4080/4081 -> EMD/EM29_ROT.EMD (roter Affe, Video 1 4:11-4:19). */
const char *re15_bf_emd_datei(uint8_t type);
/* 1 = die Bank von `type` ist die Bahnsteig-Form (RE2-Format, Lader platform/pc/main.c). */
int  re15_bf_bahnsteig_bank_faellig(uint8_t type);

int  re15_bf_bahnsteig_aktiv(void);      /* ROOM5090/5091 und nicht RE15_ENDKAMPF_NUR_G5 */
int  re15_bf_riesenform_raum(void);      /* 5080/5081 und re15_bf_bahnsteig_aktiv: der 0x30 traegt die Riesenform (Stufe 3) */
int  re15_bf_flucht_raum(void);          /* 5080/5081: der Kampf endet mit der Flucht (Clip 11), nicht mit dem Tod */
int  re15_bf_hangar_aktiv(void);         /* ROOM50A0/50A1 und nicht RE15_KEIN_HANGAR_BIRKIN */
int  re15_bf_labor_aktiv(void);          /* ROOM50E0/50E1/50F0/50F1: Labor-Kampf, dieselbe Form wie im Hangar */

/* Frueher 5090 Original -> Bahnsteig-Form ueber die Mutation (sub 6 Phase 2 @0x8011888c, Plattform-Haken). Seit Stufe 3
 * traegt der 0x30 die Riesenform ab der Raumladung; re15_bf_mutation_offen liefert 0, der Haken bleibt fuer Tests. */
int  re15_bf_mutation_offen(void);
void re15_bf_mutation_fertig(int slot);
void re15_bf_modell_hook(void (*fn)(uint8_t type));

/* ⛔ RUNDE 41 SPUR GR, NACHBESSERUNG 3 — PORT-WAHL (Nutzerentscheid F1 2026-11-02 "ja" auf "HP anheben wie beim Gorilla
 * x 2"; Dossier analysis/befunde_runde41/GR_granate.md N3): die GANZE HP-Skala der EM30-Wurzel (0x30, STAGE3/STAGE5 Root
 * 0x80116230) laeuft mal RE15_BIRKIN_HP_FAKTOR. Mitgezogen wird jede HP-Zahl und jede HP-Schwelle dieser Wurzel, damit das
 * Verhaeltnis der Formen und Tore bleibt (Tor 100 = 1/3 der Start-HP, Verwundet-Tor 200 = 2/3, Mutations-HP 150 = 1/2,
 * Revive 50 = 1/3 der Mutations-HP). NICHT skaliert: der Fern-Rueckgabewert +5 @0x8011a134-38 (gehoert zum Schaden je
 * Treffer, der unveraendert bleibt). Original (STAGE3.BIN, selbst disassembliert):
 *   INIT        `ori v0,zero,0x12c` / `sh v0,154(v1)`  @0x8011690c-910      300
 *   Verwundet   `slti v0,v0,200`                       @0x801164fc          hp < 200 (+0x1dd ^= 4, DECIDE-Mutationstor)
 *   Mutation    `slti v0,v0,100` / `ori v0,zero,0x601` @0x8011667c-684      hp < 100 -> 0x601
 *   Mut.-HP     `ori v0,zero,0x96` / `sh v0,154(v1)`   @0x80118094-098      150
 *   Revive      `ori v0,zero,0x32` / `sh v0,154(v1)`   @0x8011a530-534      50 */
#define RE15_BIRKIN_HP_FAKTOR     2
#define RE15_BIRKIN_HP_INIT       (300 * RE15_BIRKIN_HP_FAKTOR)   /* 0x12c @0x8011690c */
#define RE15_BIRKIN_HP_VERWUNDET  (200 * RE15_BIRKIN_HP_FAKTOR)   /* slti 200 @0x801164fc */
#define RE15_BIRKIN_HP_MUT_TOR    (100 * RE15_BIRKIN_HP_FAKTOR)   /* slti 100 @0x8011667c */
#define RE15_BIRKIN_HP_MUTATION   (150 * RE15_BIRKIN_HP_FAKTOR)   /* 0x96 @0x80118094 */
#define RE15_BIRKIN_HP_REVIVE     ( 50 * RE15_BIRKIN_HP_FAKTOR)   /* 0x32 @0x8011a530 */

/* Runde 41 Spur GR, Nachbesserung 2 (damals Bossschaden der Handgranate; seit Spur WS nur noch das Riesenform-Praedikat der
 * Rakete in re15_rakete_schaden, re15_damage.c): Start-HP und Ende (hp < ende) der
 * laufenden Form eines 0x30 - INIT/0 (Form-2-Bit vom Spawn), INIT/MUT_TOR (Form 1 bis zur Mutation), MUTATION/0
 * (Mutation laeuft, danach), RE15_BB_FORM_HP/0 (Bahnsteig-Form). RE15_BB_FORM_HP = die Zuweisung im Eintritt von re15_bb_tick
 * (birkin_bahnsteig.c).
 * ⛔ NACHBESSERUNG 3 (Nutzerentscheid F2 "ja"): die Bahnsteig-/Riesenform traegt die HP des RE2-G4, dessen Ablaeufe sie
 * faehrt - G4-Ctor (CDEMD0.EMS rec 1 @0x80100000): `andi v0,v0,0x40` @0x80100670 (Bit gesetzt) -> `addiu v0,zero,1000`
 * @0x80100678 / `sh v0,342(s1)` @0x8010067c; `andi v0,v0,0x20` @0x80100688 (Bit klar) ueberspringt die 900
 * @0x80100690-98. Bit 0x40 ist im ausgelieferten RE2 immer gesetzt (Schreiber-Zensus re15_damage.c "START-HP AUS DEN
 * RE2-EM-MODULEN"). Bit 0x20 ist das Schwierigkeits-Bit EASY/ROOKIE: es setzt nur das RE2-TITLE-Overlay (gelinkt @0x8019xxxx;
 * `ori v0,a0,0x20` @0x80192994 / `sw v0,-1164(at)` @0x8019299c bei Wahl s0[0xe] == 1, `ori a2,a2,0x20` @0x80192cb4 /
 * `sw v0,-1164(at)` @0x80192ce4; Spur WS, analysis/befunde_runde41/WS_waffenschaden.md Abschnitt 7). Der Port faehrt Normal
 * -> 0x20 = 0 -> 1000 ist der Port-Zweig; die 900 gilt nur auf EASY/ROOKIE (1100 @0x8010069c / 1000 @0x801006b4 bleiben
 * toter Code).
 * Vorher 150 (Port-Mass, die 0x96 der Mutation).
 * Form Stufe 3 (Bytes): `sh v0,342(s1)` @0x8010067c = 56 01 22 a6; `addiu v0,zero,1000` @0x80100678 = e8 03 02 24; die 0x96 der
 * Mutation @0x80118094-98 STAGE3.BIN = 96 00 02 34 9a 00 62 a4. Engine und Tests (auch integration_r41_form_wagenweg, liest die
 * naechste Zeile per Regex ^#define RE15_BB_FORM_HP [0-9]+) nehmen den Wert von hier. */
#define RE15_BB_FORM_HP 1000
void re15_bf_granate_form(const re15_actor_t *e, int *form_hp, int *ende);

/* Birkin (0x30) in 5090 nach Todesphase 2 an den G5 uebergeben (Typwechsel 0x30 -> 0x36). */
void re15_bf_morph_zu_g5(int slot);

/* Rueckstoss-Szene 5090 (Bahnsteig-Form -> G5): Start beim Todeseintritt des 0x30; re15_bf_pad ersetzt waehrend
 * des Beschusses die Spieler-Eingabe (Zielen + Feuern, keine Richtung) und liefert die Quadrat-Flanke. */
void     re15_bf_rueckstoss_start(int slot);
int      re15_bf_rueckstoss_phase(void);   /* 0 = aus, 1 = Beschuss, 3 = Sturz aus dem Wagen, 2 = hinter dem Zug */
uint16_t re15_bf_pad(uint16_t pad, uint16_t *pressed);

/* Kameraschnitt zur Spielerlage in ROOM5090/5091 (Baender der RVD-Zonen, s. birkin_formen.c). */
int      re15_bf_schnitt_aus_lage(int32_t x);

/* Je KI-Tick vor der Gegner-Schleife: setzt im Hangar Birkin + Spider-Man und spielt die Wurfszene. */
void re15_bf_raumtick(void);
/* 1 = dieser Slot wird gerade von der Szene gefuehrt (KI-Tick ueberspringen). */
int  re15_bf_szene_besitzt(int slot);
void re15_bf_reset(void);                /* Raumwechsel */

/* Bahnsteig-Form, Ablaeufe nach dem RE2-G4-Overlay (engine/src/birkin_bahnsteig.c). */
int  re15_bb_bank_da(const re15_actor_t *e);    /* 1 = dieser 0x30 traegt die Bahnsteig-Form (Bank-Bit bb_form, sonst 27 Knochen) */
void re15_bb_tick(int slot);                    /* ersetzt re15_birkin_ai_tick fuer die Form */
void re15_bb_reset(void);
void re15_bb_audio_hook(void (*se_fn)(int se_id, int flag2000), void (*bank_fn)(int bank));
/* Stufe 3 (PORT-WAHL): Stehen/Atmen, solange grid & 0x20 die Wurzel einfriert; Sturz-Clip der Rueckstoss-Szene. */
void re15_bb_wartet(int slot);
void re15_bb_sturz_start(int slot);
int  re15_bb_sturz_tick(int slot);              /* 1 = Clip zu Ende */
void re15_bb_taumeln_start(int slot);          /* Nachbesserung 1 M5: Rueckstoss Phase 1, Clip 13 */
void re15_bb_taumeln_tick(int slot);
int  re15_bb_ton_zaehler(void);                 /* Testhaken M3: Raum-Ton 10 fuer stumme EDD-Ids 9-15 */
/* Nachbesserung 1 M7/D (PORT-WAHL): Koerper-Segmente (form_weicht 1 = die Form weicht, 0 = der Spieler wird geschoben), Wandklemme,
 * Schatten-Rechteck im Aktorraum (x0..x1 laengs, +-hz quer). */
int     re15_bb_koerper_schub(re15_actor_t *e, re15_actor_t *pl, int form_weicht);
int32_t re15_bb_klemme_r(const re15_actor_t *e);
void    re15_bb_schatten(int32_t *x0, int32_t *x1, int32_t *hz);
/* Testhaken, kein Verhalten: Trefferzaehler +0x21d und Zahl der abgesetzten Treffer-Blutspawns der Form. */
int  re15_bb_zaehler(void);
int  re15_bb_blut_zaehler(void);
void re15_bb_blut_letzte(int32_t out[3]);       /* Testhaken M8a: Lage des letzten Blutspawns */
int  re15_bb_cd_lauf_test(int setzen);
/* Messhaken RE15_BB_POSE="x,z,gier,clip,bild,schnitt" (Sichtpruefung): der 0x30 steht fest, bild < 0 spielt ab. */
int  re15_bb_pose_aktiv(void);
void re15_bb_pose_tick(void);

/* Diagnose fuer Tests: Szenenphase (0 = wartet auf den Ausloeser, keine Figuren; 1 = Vorlauf in Cut 12; 2 = Sprung;
 * 3 = Griff/Halten; 4 = Wurf; 5 = am Boden und Flucht; 6 = Abgang (Gang auf die Kamera zu, Zeitplan); 7 = frei). */
int  re15_bf_hangar_phase(void);
int  re15_bf_hangar_slot(int birkin);    /* 1 = Birkin-Slot, 0 = Spider-Man-Slot, -1 = keiner */
int  re15_bf_hangar_riegel(void);        /* 1 = die Szene haelt Set(2,7)/Set(1,27) */
void re15_bf_hangar_vergessen(void);     /* Testhaken: "Szene gesehen" loeschen (Prozess-Merker) */
/* 1 = der Schnitt `cut` des aktuellen Raums zeigt eine Szenenfigur (Anker-Viereck + Bild), = Ausloeser-Bedingung. */
int  re15_bf_szene_im_schnitt(int cut);
/* Region-Cull-Ausnahme (Zeichner): 1 = `a` ist eine Figur der Hangar-Szene und die Szene haelt Cut 12. */
int  re15_bf_szene_zeigt(const re15_actor_t *a);
/* Runde 41 SF Neuaufbau ab NB2 (aus NB3): 1 = die Hangar-Szene haelt Cut 12 -> Effekte (gruenes Blut) nicht gegen den
 * Platzhalter-Anker cullen. */
int  re15_bf_szene_effekte(void);
/* Schatten des Spider-Man der Hangar-Szene auf den Boden (Spider-Man-Schatten @0x80111bc0 `lh a1,442` = +0x1ba, birkin_formen.c);
 * 1 = *y gesetzt. */
int  re15_bf_schatten_y(const re15_actor_t *a, int32_t *y);

#endif
