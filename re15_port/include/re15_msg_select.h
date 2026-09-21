#ifndef RE15_MSG_SELECT_H
#define RE15_MSG_SELECT_H
/*
 * re15_msg_select.h — die EINE Ja/Nein-Auswahl des Spiels.
 *
 * RE1.5 hat GENAU EINEN Zeichner fuer "Yes / No": den Zustand 4 der Nachrichten-FSM,
 * LAB_80028564 (Sprungtabelle @0x8001095c). Jede Abfrage des Spiels — die Skript-Abfrage
 * (Message_on mit Ja/Nein-Seite), der Aufnahme-Prompt ("WILL YOU TAKE THE <X>.") und die
 * Wegwerf-Abfrage ("You don't need this key any more. Discard it?") — laeuft durch
 * DIESELBE Box und DIESELBEN Instruktionen. Belegt ueber den Oeffner FUN_80027e68:
 *
 *     @0x80027ee8  beq   a1,0x100 -> LAB_80027f0c
 *     @0x80027eec  ori   v0,zero,0x22       (Delay-Slot)  Box-X   = 0x22 = 34
 *     @0x80027f10  sh    v0,DAT_800b8534                  ^
 *     @0x80027f14  ori   v0,zero,0xb4                     Box-Y   = 0xb4 = 180
 *     @0x80027f1c  sh    v0,DAT_800b8536                  ^
 *     @0x80027f38  lhu   v0,DAT_800c4fc6[a2*2]            Skript-Tabelle
 *
 * a1 = 0x100 ist der Modus, mit dem der Aufnahme-Prompt oeffnet (@0x8001df6c-94) UND aus
 * dem die Wegwerf-Zeile stammt (Skript [6] der Tabelle @0x800C4FC6). Beide landen also auf
 * Box (34,180) und damit zwangslaeufig auf denselben Auswahl-Koordinaten.
 *
 * ---------------------------------------------------------------------------------------
 * DIE ZAHLEN, jede mit ihrer Instruktion (LAB_80028564, Zustand 4):
 *
 *   Zeile:   @0x80028624  lhu   a0,DAT_800b8536      a0 = Box-Y = 0xb4 = 180
 *            @0x80028630  addiu a0,a0,0x10           +0x10  -> 196            (Cursor)
 *            @0x8002866c  lhu   a0,DAT_800b8536
 *            @0x80028674  addiu a0,a0,0x10           +0x10  -> 196            (Text)
 *
 *   Text-X:  @0x80028680  ori   a0,a0,0xae           = 174  (Anfang von "Yes")
 *
 *   Cursor-X (aus der Wahl gerechnet):
 *            @0x8002862c  lbu   v1,DAT_800b8520
 *            @0x80028638  andi  v1,v1,0x1            v1 = Wahl (0 = Yes, 1 = No)
 *            @0x8002863c  sll   v0,v1,0x3            v1*8
 *            @0x80028640  addu  v0,v0,v1             v1*9
 *            @0x80028644  sll   v0,v0,0x2            v1*36
 *            @0x80028648  subu  v0,v0,v1             v1*35
 *            @0x8002864c  sll   v0,v0,0x1            v1*70   = v1*0x46
 *            @0x80028650  addiu v0,v0,0xa0           +0xa0   = 160 / 230
 *
 *   No-X:    Die Option steht als EIN String "Yes\x01.No" @0x800C4954 (DEBUG.BIN laedt RAW
 *            nach 0x800C0000, Datei == RAM; Datei-Offset 0x4954 = 59 65 73 01 2e 4e 6f 00).
 *            Der Port zeichnet die beiden Woerter einzeln und braucht dafuer die Spalte von
 *            "No". Sie folgt aus denselben zwei gemessenen Konstanten wie die Cursor-Zellen:
 *            der Abstand der beiden Zellen ist 0x46 (@0x8002864c), der Abstand Cursor->Text
 *            ist 174-160 = 14 (@0x80028680 gegen @0x80028650) — also No-Text = 230+14 = 244
 *            = 174 + 0x46. Genau diese Herleitung tragen die beiden bereits ausgelieferten
 *            Stellen (platform/pc/main.c, platform/psx/main.c) seit ihrer Abnahme.
 *
 *   BLINKEN: der Cursor wird NUR gezeichnet, wenn das Blink-Gatter offen ist:
 *            @0x800285e8  lbu   v1,DAT_800b8525      Blink-Zaehler
 *            @0x800285f0  addiu v1,v1,-0x1           jedes Bild -1 (0 -> 0xFF)
 *            @0x800285f4  sb    v1,DAT_800b8525
 *            @0x800285f8  sll   v1,v1,0x18
 *            @0x800285fc  sra   v1,v1,0x18           Vorzeichen auf 8 Bit
 *            @0x80028600  ori   v0,zero,0x18         >>> MASKE 0x18 <<<
 *            @0x80028604  sllv  v0,v0,s1             s1 = 0 im Port (wie beide Altstellen)
 *            @0x80028608  and   v1,v1,v0
 *            @0x8002860c  beq   v1,zero,LAB_8002865c -> Cursor NICHT zeichnen
 *            Die Vorzeichen-Erweiterung aendert die Bits 3/4 nicht, `zaehler & 0x18` auf
 *            dem rohen Byte ist also dasselbe Gatter.
 *
 *   Beim Umschalten wird der Zaehler ZUERST genullt und DANN heruntergezaehlt:
 *            @0x800285bc  beq   v0,zero,LAB_800285e0  kein Links/Rechts -> ueberspringen
 *            @0x800285d4  sb    zero,DAT_800b8525     Zaehler := 0
 *            @0x800285d8  xori  v0,v0,0x1             Wahl umschalten
 *            ... und erst danach @0x800285f0 `-1` -> 0xFF -> 0xFF & 0x18 != 0.
 *            Der Cursor ist nach einem Tastendruck also SOFORT sichtbar, nicht ein Bild
 *            spaeter. Wer erst dekrementiert und dann nullt, verliert genau dieses Bild.
 * ---------------------------------------------------------------------------------------
 */
#include <stdint.h>

/* Die Glyphen-Codes der .msg-Schrift (A-Z ab 0x1D, a-z ab 0x3D): "Yes" / "No". */
extern const unsigned char re15_msg_select_yes[3];
extern const unsigned char re15_msg_select_no[2];

typedef struct {
    int x, y;                      /* linke obere Ecke des Wortes            */
    const unsigned char *glyphs;   /* .msg-Glyphencodes                      */
    int len;
} re15_msg_select_opt_t;

typedef struct {
    re15_msg_select_opt_t opt[2];  /* [0] = "Yes", [1] = "No"                */
    int cursor_x, cursor_y;        /* Zelle der AKTUELLEN Wahl               */
    int cursor_visible;            /* Blink-Gatter (0 = Cursor auslassen)    */
} re15_msg_select_t;

/* Fuellt `out` mit den byte-true Koordinaten. `choice`: 0 = Yes, 1 = No.
 * `blink` ist der Zaehler DAT_800b8525 des jeweiligen Prompts. */
void re15_msg_select_layout(int choice, uint8_t blink, re15_msg_select_t *out);

/* Ein Bild des Blink-Zaehlers (@0x800285d4 vor @0x800285f0). `lr_edge` != 0, wenn in
 * diesem Bild Links/Rechts gedrueckt wurde. */
uint8_t re15_msg_select_blink_tick(uint8_t blink, int lr_edge);

#endif /* RE15_MSG_SELECT_H */
