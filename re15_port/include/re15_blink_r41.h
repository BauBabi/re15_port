/* re15_blink_r41.h — Runde 41 Spur D "kleinkram_a": Helligkeitspuls aufnehmbarer Port-Gegenstaende
 * ("leichter Blink-Effekt"). Dossier: analysis/befunde_runde41/D_kleinkram_a.md §2. Code: engine/src/blink_r41.c.
 * Nutzer: Feuerloescher ROOM1030 (feuerloescher_r37.c registriert ihn); Spur F: Schluessel ROOM2090 (dieselbe API).
 *
 * NUTZER-VORGABE (AUFTRAG.md 2026-10-23): "Kannst du den Feuerloescher in room 1030 einen leichten Blink Effekt
 * geben, das man sieht, das der aufnehmbar ist? Ich glaube so etwas existiert bereits in Resident Evil 2, uebernehme den."
 *
 * RE2 HAT KEIN BLINKEN (ghidra_re2_Leon.txt, selbst gelesen): `Item_aot_set` FUN_80054cd4 macht das Item-Modell zum
 * Raum-Objekt Om[md1] (`lbu s2,0x14(s0)` @0x80054cf8; Feld 0x800d0324, Schritt 0x1f8 @0x80054da0-a8) und schreibt
 * nur word0 := 0x80000000 (genommen / action & 2, @0x80054db4/@0x80054dec) und +0x145 := action (@0x80054de4). Der
 * Objekt-Zeichenpass FUN_800363f0 (`jal` @0x80026964) setzt je Objekt die aus der LAGE berechneten Matrizen
 * (FUN_80076f88 @0x800364ac -> FUN_80077200 schreibt DAT_8009db64/DAT_8009db84 @0x80077288/c8; SetColorMatrix
 * @0x800364bc, SetLightMatrix @0x800364d8) und zeichnet (FUN_80027bec/dbc @0x8003666c/9c) — kein Bildzaehler, keine
 * periodische Farbe (der Zweig +0x14c & 8 @0x80036560 ist ein EINMALIGES Ausblenden: Zaehler +0x1ec bis 0x1e, dann
 * word0 := 0 @0x800365bc-dc). RE1.5 ebenso (FUN_8002c18c: flach 0x808080 bei attr 0x10, sonst NCCT).
 *
 * => PORT-WAHL, FORM aus dem eigenen Spiel: der Helligkeitspuls der aktiven Titelzeile FUN_801028ec (TITLE.BIN,
 * dieselben Konstanten wie include/re15_title_pulse.h, dort belegt): Wert 0x80; solange Zaehler < 0x1f
 * (`sltiu v0,v1,0x1f` @0x801028fc) +2 (`addiu a0,a0,2` @0x8010290c), sonst -2 (`addiu a0,a0,-2` @0x80102910);
 * Zaehler +1 (@0x80102914); bei 0x3c (`ori v0,zero,0x3c` @0x80102918) Zaehler := 0 (@0x80102924), Wert := 0x80
 * (@0x80102928). Folge: 0x82..0xBE (31 Takte hoch), 0xBC..0x86 (28 runter), 0x80. Im Titel ein Takt je Durchgang
 * (2 VBlanks); hier ein Takt je Spielbild (30 Hz) -> Periode 60 Bilder = 2 s, Spitze 0xBE/0x80 = +48,4 %.
 * ANWENDUNG: Lichtkontext des Props (ambient + Lichtfarben C) x Wert/0x80 vor dem NCCT-Schattieren — dieselbe
 * Farbbyte-Semantik (0x80 = neutral) wie das modulierte Titel-Rechteck (`sb` @0x80102850/54/58). Gilt fuer NCCT-
 * beleuchtete Props (attr ohne Bit 0x10); flach gezeichnete (0x808080) bleiben unveraendert. */
#ifndef RE15_BLINK_R41_H
#define RE15_BLINK_R41_H

#include <stdint.h>
#include "re15_light.h"   /* re15_actor_lightctx_t */

#ifdef __cplusplus
extern "C" {
#endif

#define RE15_BLINK_R41_START     0x80   /* Datenwort @0x80102944 / `ori a0,zero,0x80` @0x80102928 */
#define RE15_BLINK_R41_UP_BELOW  0x1f   /* `sltiu v0,v1,0x1f` @0x801028fc                          */
#define RE15_BLINK_R41_DELTA     2      /* `addiu a0,a0,2` @0x8010290c / `addiu a0,a0,-2` @0x80102910 */
#define RE15_BLINK_R41_PERIODE   0x3c   /* `ori v0,zero,0x3c` @0x80102918                           */
#define RE15_BLINK_R41_MAX       0xBE   /* 0x80 + 31 * 2 (Folge aus den Konstanten oben)           */
#define RE15_BLINK_R41_EINTRAEGE 8

/* Pulswert nach `takt` Schritten (FUN_801028ec, takt mod 0x3c Schritte ab dem Reset): 0x80 .. 0xBE. */
int  re15_blink_r41_wert(unsigned takt);

/* Prop (Raum, Obj_model_set-Id) blinken lassen (an = 1) oder nicht (0). Eintraege anderer Raeume stoeren nicht;
 * dieselbe (Raum, Id) ueberschreibt. Rueckgabe 0 = eingetragen, -1 = Tabelle voll. */
int  re15_blink_r41_setzen(uint16_t raum, int obj_id, int an);
int  re15_blink_r41_aktiv(uint16_t raum, int obj_id);
void re15_blink_r41_reset(void);

/* Helligkeitsfaktor Q8 (256 = unveraendert) des Props im Spielbild `takt`. */
int  re15_blink_r41_faktor_q8(uint16_t raum, int obj_id, unsigned takt);
/* Lichtkontext des Props skalieren (ambient geklemmt 255, C linear); inaktiv -> unveraendert. Rueckgabe = Pulswert
 * (0x80 wenn inaktiv). */
int  re15_blink_r41_lichtctx(uint16_t raum, int obj_id, unsigned takt, re15_actor_lightctx_t *ctx);

#ifdef __cplusplus
}
#endif
#endif /* RE15_BLINK_R41_H */
