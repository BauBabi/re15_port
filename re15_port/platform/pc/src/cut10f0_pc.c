/* cut10f0_pc.c — PC-Seite der Szene ROOM10F0 (Runde 35, Spur K): die Gestenblock-Leihe.
 *
 * Herleitung und Belege: include/re15_cut10f0.h, analysis/befunde_runde35/K_cut10f0.md. Zustand, Programm
 * und Weichen liegen plattformfrei in engine/src/cut_10f0.c; hier nur das Lesen der fremden RDT.
 * (Nachbesserung 1: aus platform/pc/main.c hierher verlegt — VERTRAG §1.4, in main.c bleiben Haken.)
 *
 * ROOM10F0 hat keinen Animationsblock (RDT+0x5C = 0). Solange die Szene aussteht, liefert
 * re15_cut10f0_rbj_quelle den Raum, dessen Block (RDT @0x5C) zu leihen ist: ROOM11B0 (RDT @0x1CB0,
 * 48168 B; Record 0 = Leons Gestenbibliothek, Record 1 = die NPC-Bibliothek, Clips 15..24).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "re15_cut10f0.h"
#include "re15_rdt.h"
#include "re15_audio.h"      /* re15_audio_re2_tuer_se */
#include "asset_root_pc.h"   /* re15_pc_read_any: dieselbe Wurzelliste wie main.c pc_read_shared */

/* Haken audio_pc.c, Se_on-Zweig (Nachbesserung 1: aus audio_pc.c hierher verlegt, dort bleibt eine Zeile).
 * Port-Bank 0x0E (RE15_CUT10F0_SE_BANK) = die geladene RE2-Tuerbank: der Tuerknall der Szene in der Se_on-Form
 * von ROOM10D0 sub21 @0x01A02. Die Original-Bank-Weiche FUN_80045024 kennt nur Bank 0..5, deshalb VOR ihr und
 * vor deren Debug-Zeile. Rueckgabe 1 = behandelt. */
int re15_cut10f0_pc_se_on(unsigned bank, int sample_id)
{
    if (bank != RE15_CUT10F0_SE_BANK) return 0;
    if (getenv("RE15_SE_DEBUG"))
        fprintf(stderr, "[se] Se_on bank=%u id=%d -> RE2-Tuerbank (Runde 35 Spur K)\n", bank, sample_id);
    re15_audio_re2_tuer_se(sample_id);
    return 1;
}

/* Haken audio_pc.c, SEQ_CTL-Zweig (Nachbesserung 1, Mangel 1). Hat die Weiche der Audio-Schicht MAIN01
 * geliefert (re15_cut10f0_bgm_haelt_main), meint ein Skript-Befehl an Slot 0 die nicht geladene TABELLEN-Musik
 * des Raums (ROOM11D0 sub01 @0x01710 `54 00 02 00 00 00` gilt MAIN3B, Tabelle @0x80074828[0x1D] = 0xFF7B).
 * Angewandt stoppte er MAIN01 (FUN_80044da4 op 2 @0x80044e50 -> SsSeqStop), und weil jeder Raum des Fensters
 * denselben MAIN traegt, startete ihn nichts neu (FUN_80044210 Gleich-Zweig @0x80044280; SsSeqReplay kehrt bei
 * gestoppter Sequenz um, @0x8005ac94); die Nutzlast @0x80044f50/@0x80044f6c schriebe in die MAIN01-Bank.
 * Rueckgabe 1 = nicht anwenden. Slots 1/2 (SUB) bleiben unberuehrt. */
int re15_cut10f0_pc_main_gesperrt(unsigned slot, int op, long cap_tick)
{
    if (slot != 0 || !re15_cut10f0_bgm_haelt_main()) return 0;
    if (getenv("RE15_BGM_CTL_DEBUG"))
        fprintf(stderr, "[bgm] Sce_bgm_control slot=0 op=%d im MAIN01-Fenster NICHT angewandt "
                        "(Runde 35 Spur K) capTick=%ld\n", op, cap_tick);
    return 1;
}

/* Den Animationsblock leihen, den die ausstehende Szene im Raum room_id braucht. Rueckgabe: Zeiger IN
 * den residenten Dateipuffer der Quell-RDT (bleibt bis zur naechsten Leihe gueltig; der Aufrufer behandelt
 * ihn wie einen RDT-Alias — rbj_borrowed = 1, nie free) und *size, oder NULL (keine Leihe noetig/moeglich). */
uint8_t *re15_cut10f0_pc_rbj_leihen(unsigned room_id, int *size)
{
    static uint8_t   *s_leih_buf = NULL;
    static re15_rdt_t s_leih_rdt;
    const unsigned quelle = re15_cut10f0_rbj_quelle(room_id);
    char rel[48];
    int n = 0;
    if (!quelle || !size) return NULL;                 /* *size bleibt dann unberuehrt */
    snprintf(rel, sizeof rel, "STAGE%u/ROOM%04X.RDT", (quelle >> 12) & 0xFu, quelle);
    uint8_t *b = re15_pc_read_any(rel, &n);
    if (!b) return NULL;
    if (re15_rdt_parse(b, (size_t)n, &s_leih_rdt) != 0 || !s_leih_rdt.animation ||
        s_leih_rdt.animation_size <= 0) { free(b); return NULL; }
    free(s_leih_buf);
    s_leih_buf = b;
    *size = s_leih_rdt.animation_size;
    fprintf(stderr, "[rbj] Animationsblock von ROOM%04X geliehen (%d B, Runde 35 Spur K)\n", quelle, *size);
    return (uint8_t *)s_leih_rdt.animation;
}
