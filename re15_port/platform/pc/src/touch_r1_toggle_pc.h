/* =============================================================================================
 * touch_r1_toggle_pc.h — R1 des Touch-Overlays als UMSCHALTER (Runde 30, Thema C).
 *
 * Nutzerwunsch (analysis/befunde_runde30/AUFTRAG.md, Abschnitt C): auf dem Touchscreen soll R1
 * nicht gehalten werden muessen — ein Tipp hebt die Waffe und laesst die Kampfpose stehen, ein
 * zweiter Tipp senkt sie wieder. Dossier: analysis/befunde_runde30/android-r1-toggle.md.
 *
 * PORT-KOMFORTFUNKTION, KEIN Original-Verhalten — nur fuer das Touch-Overlay
 * (touch_overlay_pc.c). Tastatur und Gamepad (input_pc.c) laufen NICHT hierdurch und bleiben
 * Halte-Tasten, auch ein Bluetooth-Pad am Android-Geraet.
 *
 * Sie erzeugt ausschliesslich das Pad-Bit, das das Original als PEGEL liest (selbst
 * disassembliert, info/Re1.5/PSX.EXE; 0x800ac768 = virtuelles HELD-Wort, R1 = virt. 0x100):
 *   Zieleintritt   lui s0,0x800b / addiu s0,s0,-14488 @0x80031f40/44 (s0 = 0x800ac768)
 *                  lw v0,0(s0) @0x80031ff4 / andi v0,v0,0x100 @0x80031ffc / beq @0x80032000
 *                  -> sw 0x701,0x800aca58 @0x80032020 (cmd 1, Aktion 7 = Zielen)
 *   Halten/Senken  lw v1,0x800ac768 @0x800331e4 / andi v0,v1,0x100 @0x800331ec / bne @0x800331f0
 *                  -> sonst sh 3,0x800aca5a @0x80033200 (Senken)
 *   Feuern         lw v0,0x800ac768 @0x80033300 / andi v0,v0,0x40 @0x80033308 (Viereck, Pegel)
 * Keiner dieser Leser nimmt das Flankenwort 0x800ac76c. Ein dauerhaft gesetztes R1-Bit ist fuer
 * die Spieler-FSM also dasselbe wie ein gehaltener Knopf — der Umschalter aendert NUR, wer das
 * Bit haelt, nicht, was das Spiel damit tut.
 *
 * REGEL
 *   phase_live = 1 (freies Spiel, der Spieler-Dispatcher liest das Pad; re15_player_pad_live):
 *       jede DRUCK-FLANKE des R1-Fingers kippt die Raste; das R1-Bit folgt der RASTE.
 *   phase_live = 0 (Menue, Kiste, Text, Cutscene, Tuer-/Raumblende, Tod, Titel/Front-End ...):
 *       die Raste FAELLT; das R1-Bit folgt dem FINGER (Halte-Taste wie bisher), damit die
 *       R1-Flanken der Menues (FILE-Sprung @0x80049814, Datei-Schirm DEBUG.BIN @0x800c6df4,
 *       Kisten-Blaettern re15_itembox.c) unveraendert ankommen.
 *   Ein Finger, der beim Wechsel 0 -> 1 der Phase schon aufliegt, rastet NICHT (keine Flanke).
 *
 * REINE LOGIK: kein SDL, kein Engine-Include, kein globaler Zustand — dadurch im Unit-Riegel
 * pruefbar (tests/unit/probe_r30_android_r1_toggle.c bindet GENAU diese Datei ein).
 * ============================================================================================= */
#ifndef RE15_TOUCH_R1_TOGGLE_PC_H
#define RE15_TOUCH_R1_TOGGLE_PC_H

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

#endif /* RE15_TOUCH_R1_TOGGLE_PC_H */
