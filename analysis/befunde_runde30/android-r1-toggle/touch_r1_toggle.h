/* =============================================================================================
 * touch_r1_toggle.h — R1 des Touch-Overlays als UMSCHALTER (Runde 30, Thema C).
 *
 * VORSCHLAG der Ermittlung; Zielort im Bau: re15_port/platform/pc/src/touch_r1_toggle_pc.h.
 * REINE LOGIK: kein SDL, kein Engine-Include, kein globaler Zustand — dadurch als Unit-Test
 * pruefbar (tests/unit/probe_r30_android_r1_toggle.c bindet GENAU diese Datei ein).
 *
 * PORT-KOMFORTFUNKTION auf Nutzerwunsch (AUFTRAG.md Abschnitt C), KEIN Original-Verhalten.
 * Sie erzeugt ausschliesslich das Pad-Bit, das das Original als PEGEL liest:
 *   Zieleintritt   lw v0,0(s0) [s0 = 0x800ac768] @0x80031ff4 / andi v0,v0,0x100 @0x80031ffc
 *   Halten/Senken  lw v1,0x800ac768 @0x800331e4  / andi v0,v1,0x100 @0x800331ec
 *                  -> sonst sh 3,0x800aca5a @0x80033200 (Senken)
 *   Feuern         lw v0,0x800ac768 @0x80033300  / andi v0,v0,0x40  @0x80033308
 * Ein dauerhaft gesetztes R1-Bit ist fuer die Spieler-FSM also dasselbe wie ein gehaltener
 * Knopf. Der Umschalter aendert NUR, wer das Bit haelt — nicht, was das Spiel damit tut.
 *
 * REGEL
 *   phase_live = 1 (freies Spiel, der Spieler-Dispatcher liest das Pad):
 *       jede DRUCK-FLANKE des R1-Fingers kippt die Raste; das R1-Bit folgt der RASTE.
 *   phase_live = 0 (Menue, Kiste, Karte, Text, Cutscene, Tuer-/Raumblende, Tod, Titel ...):
 *       die Raste FAELLT; das R1-Bit folgt dem FINGER (Halte-Taste wie bisher), damit die
 *       R1-Flanken der Menues (FILE-Sprung @0x80049834, Datei-Schirm @0x800c6df4,
 *       Kisten-Blaettern re15_itembox.c) unveraendert ankommen.
 *   Ein Finger, der beim Wechsel 0 -> 1 der Phase schon aufliegt, rastet NICHT (keine Flanke).
 * ============================================================================================= */
#ifndef RE15_TOUCH_R1_TOGGLE_H
#define RE15_TOUCH_R1_TOGGLE_H

typedef struct {
    unsigned char latched;    /* 1 = R1 gerastet (Kampfpose steht)                     */
    unsigned char prev_down;  /* R1-Finger lag im VORIGEN Eingabe-Tick auf             */
} re15_r1_toggle_t;

static inline void re15_r1_toggle_reset(re15_r1_toggle_t *t)
{
    t->latched = 0; t->prev_down = 0;
}

/* Ein Eingabe-Tick. finger_r1: liegt in diesem Tick ein Finger auf dem R1-Knopf (der Wert
 * NACH dem Ein-Tick-Latch des Overlays, ein Blitz-Tipp zaehlt also genau einen Tick).
 * Rueckgabe: 1 = das R1-Bit gehoert in diesem Tick ins Pad-Wort. */
static inline int re15_r1_toggle_step(re15_r1_toggle_t *t, int finger_r1, int phase_live)
{
    int down = finger_r1 ? 1 : 0;
    int edge = down && !t->prev_down;
    t->prev_down = (unsigned char)down;
    if (!phase_live) {
        t->latched = 0;
        return down;
    }
    if (edge) t->latched = (unsigned char)!t->latched;
    return t->latched;
}

#endif /* RE15_TOUCH_R1_TOGGLE_H */
