/* test_voice_ausreden.c — der vorige Satz darf ausreden, bevor die naechste
 * Dialogzeile oeffnet.
 *
 * BEFUND (Nutzer 2026-09-13, ROOM1170-Intro): "bricht ein voiceover zu frueh ab
 * 'They almost Caught...' dort kommt das 'me' nicht mehr durch, da dann schon
 * der naechste Dialog kommt."
 *
 * GEMESSEN: ROOM1170 sub02 Nachricht 3 = "They almost caught me" (Skript-Offset
 * +0x00BE; der MSG-Text ist um 4 verschoben kodiert: "PDAU =HIKOP ?=QCDP IA").
 * Das Skript gibt ihr Sleep 25+25+51 = 101 Bilder, die Aufnahme
 * synchro/STAGE1/room1170/main03.wav ist 373376 B @48 kHz = 3,89 s = 117 Bilder.
 * 16 Bilder fehlen - genau das "me". Das naechste Message_on ruft
 * re15_voice_play -> re15_xa_read_s, und das setzt pos=0: der Satz bricht ab.
 *
 * Der Riegel: op_message_on parkt, solange g_re15_voice_laeuft steht (die
 * Plattform stampt das je Bild aus s_xa.active), gedeckelt auf 90 Bilder.
 *
 * KEINE byte-true-Frage - RE1.5 hat keine englische Sprachausgabe, synchro/ ist
 * eigene Produktion. Die Sleep-Werte des Skripts bleiben unangetastet.
 */
#include <stdio.h>
#include <stdint.h>
#include "re15_scd.h"
#include "re15_msg.h"
#include "re15_room.h"

extern int g_re15_voice_laeuft;
extern int g_re15_voice_restbilder;

/* Beides zusammen ist der Zustand "eine Aufnahme laeuft noch N Bilder" - die Plattform
 * stempelt die beiden im selben Block (audio_pc.c). Der Riegel wartet gegen die echte
 * Restlaenge, nicht gegen eine geschaetzte Zahl. */
static void stimme(int restbilder)
{
    g_re15_voice_laeuft     = (restbilder > 0);
    g_re15_voice_restbilder = restbilder;
}

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { fprintf(stderr, "FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

int main(void)
{
    printf("=== der vorige Satz darf ausreden (Nutzer 2026-09-13, ROOM1170 msg 3) ===\n");

    /* Message_on ist 4 Byte: [0x2B][id][maske_lo][maske_hi] */
    uint8_t code[] = { SCD_OP_MESSAGE_ON, 3, 0x00, 0x00, SCD_OP_EVT_END };

    /* (1) Stimme laeuft -> die Zeile wird NICHT geoeffnet, der PC bleibt stehen. */
    scd_vm_init();
    scd_thread_start(0, code);
    stimme(200);                           /* laeuft noch 200 Bilder */
    const uint8_t *pc0 = g_scd.threads[0].pc;
    for (int i = 0; i < 40; i++) scd_vm_tick();
    CHECK("laufende Stimme haelt die naechste Zeile auf",
          g_scd.threads[0].pc == pc0 && g_scd.threads[0].active == 1);
    CHECK("der Wartezaehler laeuft mit", g_scd.threads[0].voice_wait == 40);

    /* (2) Stimme zu Ende -> die Zeile laeuft im naechsten Bild an. */
    stimme(0);
    scd_vm_tick();
    CHECK("nach dem Satz laeuft die Zeile an", g_scd.threads[0].pc != pc0);

    /* (3) Ohne Stimme aendert sich am Original-Timing NICHTS. */
    scd_vm_init();
    scd_thread_start(0, code);
    stimme(0);
    pc0 = g_scd.threads[0].pc;
    scd_vm_tick();
    CHECK("ohne Stimme kein Halt (Original-Timing unberuehrt)", g_scd.threads[0].pc != pc0);

    /* (4) DIE LANGEN FAELLE. Groesster gemessener Fehlbetrag im ganzen Bestand: ROOM1150
     *     main09 mit 92,5 Bildern (242,5 Bilder Aufnahme gegen 150 Bilder Skript-Zeit ab
     *     sub08 @0x11FA). Der erste Riegel hatte einen festen Deckel von 90 und haette
     *     genau diesen Fall um 2,5 Bilder abgeschnitten - deshalb zaehlt jetzt die echte
     *     Restlaenge. */
    scd_vm_init();
    scd_thread_start(0, code);
    stimme(93);                            /* der ROOM1150-main09-Fall */
    pc0 = g_scd.threads[0].pc;
    for (int i = 0; i < 92; i++) scd_vm_tick();
    CHECK("auch 93 Bilder werden abgewartet (der alte Deckel 90 schnitt hier ab)",
          g_scd.threads[0].pc == pc0);
    stimme(0);
    scd_vm_tick();
    CHECK("und danach laeuft die Zeile an", g_scd.threads[0].pc != pc0);

    /* (5) Die NOTBREMSE greift: ein haengender Kanal (laeuft, aber die Restlaenge sinkt
     *     nicht) darf das Skript nicht unbegrenzt anhalten. 300 Bilder = 10 s, also mehr
     *     als die laengste Aufnahme des Bestands (242,5 Bilder). */
    scd_vm_init();
    scd_thread_start(0, code);
    stimme(9999);                          /* Kanal haengt */
    pc0 = g_scd.threads[0].pc;
    for (int i = 0; i < 400; i++) scd_vm_tick();
    CHECK("die Notbremse loest einen haengenden Kanal", g_scd.threads[0].pc != pc0);

    /* (6) DER RIEGEL DARF NUR DIALOGZEILEN AUFHALTEN. Bis Runde 11 sass er am
     *     Funktionsanfang und parkte damit auch, was gar keine Zeile ist: das SAVE-TELEFON
     *     (ROOM1150 Nachricht 1) und die ITEM-BOX (Nachricht 3) werden im Port abgefangen
     *     und springen mit pc+=4 heraus - wer sie examinierte, waehrend noch eine Irons-Zeile
     *     klang, bekam bis zu 3 s lang gar keine Rueckmeldung: kein Menue, kein Text.
     *     Jetzt sitzt der Riegel hinter diesen Abfangungen. */
    {
        uint8_t tel[] = { SCD_OP_MESSAGE_ON, 1, 0x00, 0x00, SCD_OP_EVT_END };
        scd_vm_init();
        g_current_room_id = 0x1150;            /* ROOM1150: Nachricht 1 = Save-Telefon */
        scd_thread_start(0, tel);
        stimme(200);                           /* eine Aufnahme laeuft */
        const uint8_t *pcT = g_scd.threads[0].pc;
        scd_vm_tick();
        CHECK("das Save-Telefon wartet NICHT auf die laufende Stimme",
              g_scd.threads[0].pc != pcT);
        g_current_room_id = 0;
    }

    /* (7) Dasselbe fuer den AUSWAHL-Dialog: er hat seinen eigenen Park-Zweig ueber dem
     *     Riegel. Der Spieler drueckt, und das Menue muss sofort kommen - nicht erst,
     *     wenn ein Satz aus einer anderen Szene fertig ist. */
    {
        uint8_t frage[] = { SCD_OP_MESSAGE_ON, 7, 0x00, 0x00, SCD_OP_EVT_END };
        uint8_t blob[]  = { 0x30, 0x03, 0x01, 0x00 };   /* ein Glyph, dann 0x03 = Auswahl */
        scd_vm_init();
        re15_msg_install_text(7, blob, sizeof blob);
        scd_thread_start(0, frage);
        stimme(200);
        int offen = 0;
        for (int i = 0; i < 3 && !offen; i++) { scd_vm_tick(); offen = g_scd.message_fsm_active; }
        CHECK("der Auswahl-Dialog oeffnet sofort, trotz laufender Stimme", offen);
    }

    stimme(0);
    printf(g_fail ? "=== FEHLGESCHLAGEN ===\n" : "=== OK ===\n");
    return g_fail;
}
