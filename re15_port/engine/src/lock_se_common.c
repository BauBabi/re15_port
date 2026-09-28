/* lock_se_common.c — "Tuer verschlossen"-Ton an verschlossenen Tueren.
 *
 * ⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL. Alle Belege (RE1.5 stumm @0x80043084 /
 * @0x800430bc; RE2 @0x80051610 / @0x800516a4 jal 0x8005ba28 mit a0 = 0x02160000,
 * a1 = 0; ROOM2110 sub09 @Datei 0x01BBC) stehen im Kopf von include/re15_lock_se.h.
 * Dossier: analysis/befunde_runde30/tuer-verschlossen.md.
 *
 * Ablauf (RE2 @0x800516a0..@0x800516b8): Satz waehlen, SE-Spieler rufen (nicht
 * positional, @0x800516a8 `addu a1,zero,zero`), im selben Bild die Nachricht oeffnen.
 * Der Port ruft re15_lock_se_notice unmittelbar vor re15_dialog_open_mask — also im
 * selben Bild wie der Text, genau wie RE2.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "re15_lock_se.h"
#include "re15_audio.h"
#include "re15_engine.h"           /* g_engine.frame_count — nur fuer das Datei-Log */

#include "gen/re2_door_bank.inc"   /* RE2_DOOR_EDT_* / RE2_DOOR_VBD_* / RE2_DOOR_SE_ZU_{A,B,E,P} */
#include "gen/lock_se_sites.inc"   /* re15_lock_se_sites[], RE15_LOCK_ART_K/_M/_S, *_SITE_COUNT */

#if RE15_LOCK_ART_K != RE15_LOCK_SE_ART_K || RE15_LOCK_ART_M != RE15_LOCK_SE_ART_M || \
    RE15_LOCK_ART_S != RE15_LOCK_SE_ART_S
#error "gen/lock_se_sites.inc und include/re15_lock_se.h nummerieren die Arten verschieden"
#endif

/* ============================================================================
 * ⛔ DIE ZUORDNUNG ART -> WELLE — EINE Stelle, zum Tauschen nach dem Anhoeren.
 * ============================================================================
 * RE2 legt die Welle je RAUM fest (Raumbank-Satz 0x16, Bank 2 = [RDT+0x08] via
 * FUN_80059e54 @0x80059f44), NICHT je Tuerart: die Tuer ROOM2000<->ROOM2020 klingt von
 * der einen Seite nach A, von der anderen nach B. Eine Original-Regel "Tuerart -> Welle"
 * gibt es deshalb nicht; beide Zeilen unten sind Zuordnungen des Ports.
 * Gemessen (Huellkurven, analysis/befunde_runde30/tools/r30_re2_wellen_familien.py):
 *   Familie A 0,77-0,82 s in 9 Revier-Raeumen (1010 10C0 1100 1140 2000 2010 20A0 20B0 20F0)
 *   Familie B 0,51 s      in 6 Revier-Raeumen (1050 1160 2020 2040 2070 20C0)
 *   Familie E 1,10 s      in 12 Raeumen inkl. ROOM2110 (Kartenleser-Tuer) und Stage 4-7
 *
 * Art K (elektronisch / Kartenleser / Pincode) -> ZU_E.
 *   Port-Wahl, keine Original-Adresse fuer die ZUORDNUNG (Nutzer-Vorgabe Runde 30:
 *   Kartenleser- und Pincode-Tueren). Vorbild der Welle ist die einzige Kartenleser-Tuer
 *   in RE2 Leon A: ROOM2110 sub09 @Datei 0x01BBC Se_on(2,0x16); Raumbank-Satz 0x16
 *   EDT @Datei 0x02EFC `00 00 74 00`, VAG @Datei 0x07054 (10272 B, 1,095 s).
 * Art M (mechanisch / andere Seite)           -> ZU_A.
 *   Port-Wahl, keine Original-Adresse. RE2 ROOM1140 Satz 0x16, EDT @Datei 0x01448
 *   `00 00 74 16`, VAG @Datei 0x02870 (7184 B, 0,822 s) — Familie A, die RE2 in den
 *   meisten Revier-Raeumen (9 gegen 6) nimmt. Alternative ZU_B (ROOM1050 Satz 0x16,
 *   VAG @Datei 0x074A4, 3232 B, 0,510 s) liegt in derselben Bank: Tausch = diese Zeile. */
#define RE15_LOCK_SE_SATZ_K  RE2_DOOR_SE_ZU_E
#define RE15_LOCK_SE_SATZ_M  RE2_DOOR_SE_ZU_A

/* Art S (ohne Strom)                          -> ZU_P.  KEINE Port-Wahl, sondern BELEGT:
 *   RE1.5 ROOM5080/5081 und RE2 ROOM7020 sind derselbe Raum (Generator, dieselben vier Texte).
 *   RE1.5 legt "The door won't open until the power is restored!" (msg 2 @Datei 0x00996 /
 *   0x0098E) per Aot_reset sce 1 auf den Tuer-Platz 0 (sub02 @Datei 0x007C2 / 0x007BA
 *   `46 00 01 31 02 00 ff ff 00 00`; Platz 0 = Door_aot_set main00 @Datei 0x006FA) und ist
 *   dort stumm wie an jeder Tuer (@0x80043084). RE2 spielt beim woertlich gleichen Text
 *   (ROOM7020 msg 2 @Datei 0x01F69) in sub06 @Datei 0x01598 Message_on 2 und direkt danach
 *   @Datei 0x0159E `36 02 16 00 00 00 00 00 00 00 00 00` Se_on(2,0x16). Raumbank-Satz 0x16 von
 *   ROOM7020: EDT @Datei 0x05CE8 `00 00 e3 00`, Tone @Datei 0x06730, VAG @Datei 0x1DD50
 *   (6144 B, sha1 ea19d086e3cb) = Satz 3 der Mini-Bank. RE1.5 ist hier nicht massgeblich,
 *   weil es an Tueren gar keinen Ton hat (Kopf von include/re15_lock_se.h). */
#define RE15_LOCK_SE_SATZ_S  RE2_DOOR_SE_ZU_P

unsigned g_re15_lock_se_zaehler = 0;
int      g_re15_lock_se_letzter = -1;

int re15_lock_se_satz_fuer_art(int art)
{
    if (art == RE15_LOCK_SE_ART_K) return RE15_LOCK_SE_SATZ_K;
    if (art == RE15_LOCK_SE_ART_M) return RE15_LOCK_SE_SATZ_M;
    if (art == RE15_LOCK_SE_ART_S) return RE15_LOCK_SE_SATZ_S;
    return -1;
}

int re15_lock_se_site_count(void) { return RE15_LOCK_SE_SITE_COUNT; }

int re15_lock_se_site(int i, unsigned *out_room, int *out_msg, int *out_art, int *out_wege)
{
    if (i < 0 || i >= RE15_LOCK_SE_SITE_COUNT) return -1;
    const re15_lock_se_site_t *s = &re15_lock_se_sites[i];
    if (out_room) *out_room = s->room;
    if (out_msg)  *out_msg  = s->msg;
    if (out_art)  *out_art  = s->art;
    if (out_wege) *out_wege = s->wege;
    return 0;
}

/* Messschiene fuer den Echtlauf (die GUI-exe hat kein stderr): RE15_TUERSE_LOG=<pfad>
 * schreibt je Ausloesung eine Zeile (Bildnummer, Raum, Nachricht, Art, Weg, Satz). Ohne die
 * Variable passiert nichts. Die Bildnummer erlaubt, den Ton in einer RE15_AUDIO_CAP_SYNC-
 * Aufnahme wiederzufinden (Offset = Bild * 1470 Stereo-Frames, audio_pc.c). */
static void tuerse_log(unsigned room, int msg, int art, int weg, int satz)
{
    static const char *s_path = NULL; static int s_init = 0;
    if (!s_init) { s_init = 1; s_path = getenv("RE15_TUERSE_LOG"); if (s_path && !*s_path) s_path = NULL; }
    if (!s_path) return;
    FILE *f = fopen(s_path, "ab");
    if (!f) return;
    fprintf(f, "F%u raum=%04X nachricht=%d art=%c weg=%s satz=%d(%s) nr=%u\n",
            (unsigned)g_engine.frame_count,
            room, msg, art == RE15_LOCK_SE_ART_K ? 'K' : art == RE15_LOCK_SE_ART_M ? 'M' : 'S',
            weg == RE15_LOCK_WEG_AOT ? "AOT" : "SKRIPT", satz,
            satz == RE2_DOOR_SE_ZU_A ? "ZU_A" : satz == RE2_DOOR_SE_ZU_B ? "ZU_B"
                                     : satz == RE2_DOOR_SE_ZU_E ? "ZU_E"
                                     : satz == RE2_DOOR_SE_ZU_P ? "ZU_P" : "?",
            g_re15_lock_se_zaehler);
    fclose(f);
}

int re15_lock_se_notice(unsigned room_id, uint8_t msg_id, int weg)
{
    for (int i = 0; i < RE15_LOCK_SE_SITE_COUNT; i++) {
        const re15_lock_se_site_t *s = &re15_lock_se_sites[i];
        if (s->room != (uint16_t)room_id || s->msg != msg_id) continue;
        if (!(s->wege & weg)) return -1;       /* dieser Weg ist fuer die Stelle nicht belegt */
        int satz = re15_lock_se_satz_fuer_art(s->art);
        if (satz < 0) return -1;
        g_re15_lock_se_zaehler++;
        g_re15_lock_se_letzter = satz;
        tuerse_log(room_id, msg_id, s->art, weg, satz);
        /* RE2 @0x800516a4 `jal 0x8005ba28` mit a1 = 0 (@0x800516a8): nicht positional. */
        re15_audio_re2_door_se(satz);
        return satz;
    }
    return -1;
}

/* Satz-TOC der Mini-Bank (gen/re2_door_bank.inc) — Muster re15_elev_bank_rec. */
void re15_door_bank_rec(re15_door_bank_rec_t *out)
{
    if (!out) return;
    out->edt_off  = RE2_DOOR_EDT_OFF;
    out->edt_size = RE2_DOOR_EDT_SIZE;
    out->vbd_off  = RE2_DOOR_VBD_OFF;
    out->vbd_size = RE2_DOOR_VBD_SIZE;
    out->se_count = RE2_DOOR_SE_COUNT;
    out->se_zu_a  = RE2_DOOR_SE_ZU_A;
    out->se_zu_b  = RE2_DOOR_SE_ZU_B;
    out->se_zu_e  = RE2_DOOR_SE_ZU_E;
    out->se_zu_p  = RE2_DOOR_SE_ZU_P;
}
