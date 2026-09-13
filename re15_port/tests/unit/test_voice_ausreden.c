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

extern int g_re15_voice_laeuft;

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
    g_re15_voice_laeuft = 1;
    const uint8_t *pc0 = g_scd.threads[0].pc;
    for (int i = 0; i < 40; i++) scd_vm_tick();
    CHECK("laufende Stimme haelt die naechste Zeile auf",
          g_scd.threads[0].pc == pc0 && g_scd.threads[0].active == 1);
    CHECK("der Wartezaehler laeuft mit", g_scd.threads[0].voice_wait == 40);

    /* (2) Stimme zu Ende -> die Zeile laeuft im naechsten Bild an. */
    g_re15_voice_laeuft = 0;
    scd_vm_tick();
    CHECK("nach dem Satz laeuft die Zeile an", g_scd.threads[0].pc != pc0);

    /* (3) Ohne Stimme aendert sich am Original-Timing NICHTS. */
    scd_vm_init();
    scd_thread_start(0, code);
    g_re15_voice_laeuft = 0;
    pc0 = g_scd.threads[0].pc;
    scd_vm_tick();
    CHECK("ohne Stimme kein Halt (Original-Timing unberuehrt)", g_scd.threads[0].pc != pc0);

    /* (4) Der Deckel greift: ein haengender Kanal darf das Skript nicht anhalten.
     *     90 Bilder = 3 s, noetig sind nach der Messung <20. */
    scd_vm_init();
    scd_thread_start(0, code);
    g_re15_voice_laeuft = 1;
    pc0 = g_scd.threads[0].pc;
    for (int i = 0; i < 200; i++) scd_vm_tick();
    CHECK("Deckel 90 Bilder loest einen haengenden Kanal", g_scd.threads[0].pc != pc0);

    g_re15_voice_laeuft = 0;
    printf(g_fail ? "=== FEHLGESCHLAGEN ===\n" : "=== OK ===\n");
    return g_fail;
}
