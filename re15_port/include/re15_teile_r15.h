/*
 * RE1.5 Rebuilt — die RE1.5-Modellteil-Schicht (Teil-Wort-Bits und Teil-Felder), wie sie der EXE-Zeichner liest.
 *
 * Teil-Record im Original: Aktor+0x188, Teil k bei +k*0xac (FUN_8001e5b0). Erzeuger sind die Overlay-Handler (Zombie-
 * Gore, Birkin-Wurf, ...), einziger Leser ist der Zeichengang FUN_8001e8c8 -> FUN_8001e9ec (sichtbar) / FUN_8001ef54
 * (nicht sichtbar), Teile flach, OHNE Eltern-Kaskade. Bits (alle @PSX.EXE):
 *   0x0001 zeichnen (@0x8001ecc4)            0x0008 in Scherben zerplatzen (FUN_80023410/80023708, FUN_8001f1e0)
 *   0x0020 Flugphysik FUN_8001f024           0x0040 eigene Weltmatrix behalten (@0x8001ea10)
 *   0x0080 Tinte Vertex 1/2 (FUN_8002441c)   0x0200 animierte Lokalrotation einfrieren (@0x8001f564/@0x8001f5e0)
 *   0x0400 Skalierung lokale Achsen (@0x8001ebb0)   0x2000 Skalierung Weltachsen (@0x8001ebec)
 *   0x1000 eigener Lichtbezug (@0x8001eb54)  0x0002 wird nirgends gelesen
 * Farbwort +0x68 multipliziert das Beleuchtungsergebnis (neutral 0x808080, FUN_8001ee68).
 */
#ifndef RE15_TEILE_R15_H
#define RE15_TEILE_R15_H

#include <stdint.h>

#define RE15_TEILE_MAX 24   /* Spieler nutzt Teil 20 (0xd70 @0x80024c30), Spinne Teil 19 (0xcc4) */

typedef struct {
    uint16_t w;            /* +0x00 Teil-Wort */
    uint32_t col;          /* +0x68 Farbwort (0x00BBGGRR) */
    int16_t  v[3];         /* +0x38/+0x3a/+0x3c Fluggeschwindigkeit */
    int8_t   grav;         /* +0x75 Schwerkraft auf vy */
    int8_t   spin_inc;     /* +0x76 */
    int16_t  spin;         /* +0x82 Z-Drehung */
    int16_t  scale[3];     /* +0x88/+0x8a/+0x8c Q12 */
    uint16_t h94[6];       /* +0x94..+0x9e roh: Tinte1 = h94[0]|h94[1]<<16 (vy = (s16)h94[1]), Tinte2 = h94[2]|h94[3]<<16
                            * (vy_inc = (s16)h94[3]), Scherben-Bild = h94[4] */
    int32_t  rot[9];       /* +0x40 Weltmatrix, gueltig wenn `fest` (Bit 0x40) */
    int32_t  t[3];
    uint8_t  fest;         /* Matrix bereits uebernommen */
} re15_teil_t;

/* Teilstand des Aktor-Slots (NULL bei ungueltigem Slot). `an` = mindestens ein Erzeuger hat geschrieben. */
re15_teil_t *re15_teile(int slot);
int          re15_teile_an(int slot);
void         re15_teile_markieren(int slot);           /* ein Erzeuger schreibt -> Zeichner nimmt die Schicht */
void         re15_teile_reset(int slot);               /* FUN_8001ee68: w = 1, col = 0x808080, Rest bleibt */
void         re15_teile_reset_alle(void);

/* Tinte/Felder lesen wie das Original (ueberlappende Halbwoerter). */
uint32_t re15_teil_tinte1(const re15_teil_t *t);
uint32_t re15_teil_tinte2(const re15_teil_t *t);

/* Eine Teilstufe aus FUN_8001e9ec fuer Teil `k` (Zeichengang, Teile in Index-Reihenfolge):
 *   rot/trans: rein = Weltmatrix aus der Skelett-Pose, raus = die gezeichnete Weltmatrix.
 *   parent:   Elternindex (-1 = Wurzel). Kinder eines verschobenen Teils werden aus dessen ERGEBNIS-Matrix
 *             zusammengesetzt (Eltern-Zeiger +0x6c zeigt auf Eltern+0x40, FUN_8001e5b0).
 *   boden_y:  Aktor+0x1ba.
 *   sichtbar: FUN_80014368 (nur sichtbar zaehlt das Scherbenbild hoch, @0x80023a0c).
 * Rueckgabe: 0 = nicht zeichnen (Bit 0 aus). */
int re15_teil_stufe(int slot, int k, int parent, int32_t rot[9], int32_t trans[3], int32_t boden_y, int sichtbar);

/* Nach dem Zeichnen eines 0x08-Teils: Bild++ (nur sichtbar, @0x80023a0c) und FUN_8001f1e0 (Drift, Ende ab 0x1d). */
void re15_teil_scherben_nach(int slot, int k, int sichtbar);

/* Scherbenversatz eines Prims (FUN_80023410 @0x80023528-78): v1 = zweiter Eckpunkt, Ergebnis im Modellraum. */
void re15_teil_scherbe(const re15_teil_t *t, int32_t v1x, int32_t v1y, int32_t v1z, int32_t out[3]);

#endif
