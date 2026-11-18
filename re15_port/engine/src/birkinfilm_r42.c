/*
 * birkinfilm_r42.c — Runde 42 Strang "birkinfilm": Leons erstes Treffen mit Annette (ROOM6000 sub06) mit Rahmenzeilen und dem
 * RE2-Film "Birkins Mutation". Herleitung, Ablauf und Belege: include/re15_birkinfilm_r42.h und der Kopf von
 * tools/r42_birkinfilm/szene_bauen.py (erzeugt gen/birkinfilm_r42.inc). Film und Blenden: engine/src/zws_r41.c (Tabellenzeile
 * "birk6000", Blendenart RE15_ZWS_BLENDART_4090).
 *
 * Stufe 2: Elzas Fassung ROOM5011 sub02 (Film zwischen msg 3 @0x00640 und msg 4 @0x00690, keine neuen Saetze; Herleitung im
 * Header, Abschnitt "Stufe 2"), Tabellenzeile "birk5011" in zws_r41.c.
 *
 * PORT-STORY. Kein Asset-Patch: der Port tauscht nur das Ziel des Evt_exec sub06 (6000 sub00 @0x00F58) bzw. sub02 (5011 sub00
 * @0x0053E), wenn das Sub im geladenen RDT bytegleich der Signatur ist (Form story6_r42.c umleiten, hier die ganzen Subs).
 */
#include <stdint.h>
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "re15_birkinfilm_r42.h"
#include "re15_scd.h"
#include "re15_msg.h"
#include "re15_engine.h"

#include "gen/birkinfilm_r42.inc"

extern unsigned g_current_room_id;

#ifdef RE15_PLATFORM_PC
#define BF_LOG(...) do { fprintf(stderr, "[birkinfilm] F%u ", (unsigned)g_engine.frame_count); fprintf(stderr, __VA_ARGS__); \
                         fputc('\n', stderr); } while (0)
#else
#define BF_LOG(...) do { } while (0)
#endif

unsigned g_re15_bf_umleitungen = 0;
unsigned g_re15_bf_warten = 0;
static unsigned s_warte = 0;

/* ==== Texte (Haken in re15_msg_load_room_block nach den RDT-Texten, Form story6_r42.c) ==================================== */
const uint8_t *re15_birkinfilm_r42_text(uint8_t id, int *out_len)
{
    for (int i = 0; i < BF_TEXTE_N; i++)
        if (k_bf_texte[i].id == id) {
            if (out_len) *out_len = k_bf_texte[i].n;
            return k_bf_texte[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

void re15_birkinfilm_r42_texte(uint16_t room_id)
{
    s_warte = 0;                           /* Warte-Zeile 43 (6000) / 46 (5011): je Raumladen neu */
    if (room_id != RE15_BF_RAUM) return;   /* nur Leons ROOM6000; ROOM6001 (Elza) hat keine Annette-Szene, 5011 keine neuen Saetze */
    for (int i = 0; i < BF_TEXTE_N; i++) {
        re15_msg_install_text(k_bf_texte[i].id, k_bf_texte[i].b, (size_t)k_bf_texte[i].n);
        const int d = re15_msg_compute_duration(k_bf_texte[i].b, k_bf_texte[i].n, 0);   /* wie re15_msg_load_room_block */
        if (d > 0 && d < 65535) re15_msg_install_durations(k_bf_texte[i].id, d);
    }
    BF_LOG("ROOM6000: %d Rahmenzeilen eingesetzt (msg 40..42 Satz 1, 48..50 Satz 2)", BF_TEXTE_N);
}

/* ==== Programm ============================================================================================================ */
const uint8_t *re15_birkinfilm_r42_programm(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_bf_6000_prog;
    return k_bf_6000_prog;
}

const uint8_t *re15_birkinfilm_r42_programm_raum(unsigned room_id, int *out_len)
{
    if ((room_id & 0xFFFFu) == RE15_BF_RAUM_ELZA) {
        if (out_len) *out_len = (int)sizeof k_bf_5011_prog;
        return k_bf_5011_prog;
    }
    if ((room_id & 0xFFFFu) == RE15_BF_RAUM) return re15_birkinfilm_r42_programm(out_len);
    if (out_len) *out_len = 0;
    return NULL;
}

const uint8_t *re15_birkinfilm_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc)
{
    if (room_id == RE15_BF_RAUM_ELZA && sub == RE15_BF_ELZA_SUB && pc) {   /* Stufe 2: 5011 sub00 @0x0053E Evt_exec sub02 */
        if (memcmp(pc, k_bf_5011_sub02, sizeof k_bf_5011_sub02) != 0) {
            BF_LOG("ROOM5011 sub02 weicht von der Signatur ab - Original-Ablauf ohne Film");
            return pc;
        }
        g_re15_bf_umleitungen++;
        BF_LOG("ROOM5011 Evt_exec sub02 -> Programm (%u B): Film R409BIRK zwischen msg 3 und msg 4 (Elza)",
               (unsigned)sizeof k_bf_5011_prog);
        return k_bf_5011_prog;
    }
    if (room_id != RE15_BF_RAUM || sub != RE15_BF_SUB || !pc) return pc;
    if (memcmp(pc, k_bf_6000_sub06, sizeof k_bf_6000_sub06) != 0) {
        BF_LOG("ROOM6000 sub06 weicht von der Signatur ab - Original-Ablauf ohne Film");
        return pc;
    }
    g_re15_bf_umleitungen++;
    BF_LOG("ROOM6000 Evt_exec sub06 -> Programm (%u B): Rahmenzeilen 40/41, Film R409BIRK, Rahmenzeile 42 vor msg 3",
           (unsigned)sizeof k_bf_6000_prog);
    return k_bf_6000_prog;
}

/* ==== Warte-Zeile 43 (RE2 Wsleep @0x027E2) ================================================================================ */
int re15_birkinfilm_r42_message_on(const uint8_t *pc)
{
    if (!pc) return 0;
    const uint16_t raum = (uint16_t)g_current_room_id;
    if (!((raum == RE15_BF_RAUM && pc[1] == RE15_BF_MSG_WARTEN) ||
          (raum == RE15_BF_RAUM_ELZA && pc[1] == RE15_BF_ELZA_MSG_WARTEN))) return 0;
    /* Der Stimmen-Riegel davor (scd_vm.c) hat das Ende der Stimme abgewartet (6000: Xa 0x11; 5011: synchro main03 = msg 3).
     * PORT-WAHL: auch den Untertitel abwarten. */
    if (g_scd.message_active && s_warte < RE15_BF_WARTEN_NOTBREMSE) { s_warte++; return 2; }
    g_re15_bf_warten = s_warte;
    BF_LOG("ROOM%04X Warte-Zeile %d: Stimme und Untertitel zu Ende (%u Takte auf den Untertitel)%s", (unsigned)raum, pc[1],
           s_warte, s_warte >= RE15_BF_WARTEN_NOTBREMSE ? " NOTBREMSE" : "");
    s_warte = 0;
    return 1;
}

/* ==== Satz 2 der Rahmenzeilen (Runde 42 Strang namen) ===================================================================== */
/* RE2 ROOM4090 sub02 zeigt jede Rahmenzeile als ZWEI Nachrichten unter EINER Stimme: msg 0x0A @0x02718, Xa_on 0x09 @0x0271E,
 * Sleep 41 @0x02722, msg 0x0B @0x02726 (ebenso 0x14/0x15 um Sleep 81 @0x027D8, 0x17/0x18 um Sleep 81 @0x02850). Das Programm
 * (gen/birkinfilm_r42.inc) bildet das ab: msg 40/41/42 = Satz 1 mit der RE2-Stimme, Sleep, msg 48/49/50 = Satz 2.
 * PORT-WAHL: Satz 2 laeuft am Stimmen-Riegel (scd_vm.c op_message_on, "DEN VORIGEN SATZ AUSREDEN LASSEN") vorbei und reiht keine
 * eigene Stimme ein - er IST der zweite Teil der laufenden Aufnahme; der Riegel wuerde ihn bis zu deren Ende zurueckhalten.
 * Geoeffnet wird er wie jede Untertitelzeile (re15_dialog_open_mask, nicht blockierend, Maske aus dem Opcode). */
int re15_birkinfilm_r42_folgeseite(const uint8_t *pc, uint32_t pause_mask)
{
    if (!pc || (uint16_t)g_current_room_id != RE15_BF_RAUM) return 0;
    if (pc[1] != RE15_BF_MSG_XA09_S2 && pc[1] != RE15_BF_MSG_XA11_S2 && pc[1] != RE15_BF_MSG_XA14_S2) return 0;
    re15_dialog_open_mask((int)pc[1], 0, pause_mask);
    g_scd.message_arg2 = pc[2];
    g_scd.message_arg3 = pc[3];
    BF_LOG("ROOM6000 msg %d: Satz 2 unter der laufenden RE2-Stimme (kein Riegel, keine eigene Stimme)", pc[1]);
    return 1;
}

/* ==== Stimmen ============================================================================================================= */
const char *re15_birkinfilm_r42_stimme(unsigned room_id, int voice_id, int *lese)
{
    if ((room_id & 0xFFFFu) != RE15_BF_RAUM) return NULL;
    const char *d = NULL;
    int l = 0;
    switch (voice_id) {
        case RE15_BF_MSG_XA09: d = RE15_BF_XA09_DATEI; l = RE15_BF_XA09_LESE; break;
        case RE15_BF_MSG_XA11: d = RE15_BF_XA11_DATEI; l = RE15_BF_XA11_LESE; break;
        case RE15_BF_MSG_XA14: d = RE15_BF_XA14_DATEI; l = RE15_BF_XA14_LESE; break;
        default: return NULL;
    }
    if (lese) *lese = l;
    return d;
}
