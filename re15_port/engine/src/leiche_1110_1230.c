/*
 * RE1.5 Rebuilt — Leichen ROOM1110 / ROOM1230: neuer Untersuchen-Text + einmal Handgun-Munition.
 *
 * Runde 34 Nacht, Spur F. Herleitung, Belege und alle Konstanten: include/re15_leiche.h und
 * analysis/befunde_runde34_nacht/F_leichen.md. Kein Asset-Patch: die RDTs bleiben byte-true, das
 * Original-Ereignis der Leiche (ROOM1110 sub02 / ROOM1230 sub21) laeuft unveraendert; der Port
 * tauscht an dessen Message_on nur die Nachricht und oeffnet danach das normale Aufnahme-Modal.
 */
#include "re15_leiche.h"

#include "re15_gameflow.h"     /* RE15_ROOM_BASE                                    */
#include "re15_room.h"         /* g_current_room_id, g_room_change                   */
#include "re15_scd.h"          /* g_scd, re15_game_flag_get, re15_pauseflags_belegt,
                                * scd_audio_queue_push                              */
#include "re15_msg.h"          /* re15_msg_install_text, re15_dialog_open_mask ...   */
#include "re15_item_modal.h"   /* re15_item_modal_start / _active                    */
#include <string.h>            /* memset                                            */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include "re15_inventory.h"    /* nur fuer die Logzeile: Munition nach "Yes"         */
#endif

/* ---------------------------------------------------------------------------------------------
 * DIE VIER TEXTE als .msg-Rohbytes. Glyphen = die des Auslieferungsstands (Tabelle msg_common.c
 * re15_msg_glyph: 0x00 Leerzeichen, 0x1D.. A-Z, 0x3D.. a-z, 0x18 ',', 0x3A ''', 0x57 '.'), JEDER
 * Baustein liegt woertlich so in einer ausgelieferten RDT (Fundstelle je Zeile, selbst gelesen
 * aus shared_assets/PSX/STAGE1 = info/Re1.5/PSX):
 *   Kopf `04 02`, Seitenumbruch `02 00`, Ende `01 00` = die der ersetzten Original-Nachricht
 *     (ROOM1110 msg 0: @0x0D68 / @0x0D8B / @0x0DD3; ROOM1230 msg 10: @0x16F4 / @0x170A / @0x1752).
 *     Der Seitenumbruch steht dort, wo ihn das Original zwischen denselben zwei Saetzen hat; der
 *     Nutzer schreibt beide Saetze in eine Zeile, weil er abtippt (Dossier L7).
 *   ⛔ Kleinschreibung "police"/"holding": die WORTGLEICHEN Teile sind Original-Bytes (klein), die
 *     Grossbuchstaben des Nutzers ("Police", "Holding") sind Tipp-Gewohnheit. Vorlauf Runde 33:
 *     Nutzer "I have to Report the situation ...", ausgeliefert "report" klein (tuer1120_1130.c
 *     k_meldung) ohne Einwand (Gegenpruefung Auflage 6c).
 *   "…" des Nutzers = drei Einzelpunkte `57 57 57`, wie RE1.5 "..." IMMER schreibt: 270 Folgen im
 *     Bestand, 0 Folgen mit vier Punkten, die Glyphe 0xF2 kommt in keiner der 1227 Nachrichten vor
 *     (Dossier 3.2). Der Punkt hinter "…" in "A miserable death…. He is Holding something." ist der
 *     Satztrenner des Nutzers beim Abtippen zweier Seiten, kein vierter Punkt (Dossier L9).
 *   Satzende "." (0x57) hinter "something" in BEIDEN Raeumen: 1230 schreibt der Nutzer ausdruecklich
 *     "He is Holding something." (Nutzer-Schreibung), 1110 im Gleichlauf dazu (Dossier L8; 1193 von
 *     1227 ausgelieferten Nachrichten enden auf Satzzeichen oder Auswahl).
 * Zeilenbreiten (Vorschub include/font_width.h = DEBUG.BIN[0x4416+code]): 1110 lang 210 / 160 px,
 * 1230 lang 129 / 160 px, kurz 210 bzw. 129 px — Bestand: 99 % der 2505 Zeilen <= 271 px, also
 * kein Umbruch noetig.
 * --------------------------------------------------------------------------------------------- */

/* "It's a police officer, he's dead." ▼ "He is holding something." */
static const uint8_t k_1110_lang[] = {
    0x04, 0x02,                                            /* Kopf       ROOM1110 @0x0D68  */
    /* I    t    '    s   _  a   _  p    o    l    i    c    e   _  o    f    f    i    c
     * e    r    ,   _  h    e    '    s   _  d    e    a    d    .                          */
    0x25,0x50,0x3a,0x4f,0x00,0x3d,0x00,0x4c,0x4b,0x48,0x45,0x3f,0x41,0x00,0x4b,0x42,0x42,0x45,0x3f,
    0x41,0x4e,0x18,0x00,0x44,0x41,0x3a,0x4f,0x00,0x40,0x41,0x3d,0x40,0x57, /* ROOM1110 @0x0D6A (msg 0 S.1) */
    0x02, 0x00,                                            /* Umbruch    ROOM1110 @0x0D8B  */
    /* H    e   _  i    s   _  h    o    l    d    i    n    g   _  s    o    m    e    t
     * h    i    n    g                                                                     */
    0x24,0x41,0x00,0x45,0x4f,0x00,0x44,0x4b,0x48,0x40,0x45,0x4a,0x43,0x00,0x4f,0x4b,0x49,0x41,0x50,
    0x44,0x45,0x4a,0x43,                                   /* ROOM1011 @0x012A3 (msg 19, am Stueck;
                                                            * einzeln: "He is holding" ROOM1110 @0x0D8D,
                                                            * " something" ROOM1110 @0x0EC4 msg 6)     */
    0x57,                                                  /* "."        ROOM1110 @0x0DA1  */
    0x01, 0x00,                                            /* Ende       ROOM1110 @0x0DD3  */
};

/* "It's a police officer, he's dead." — Seite 1 der Original-Nachricht, bytegleich. */
static const uint8_t k_1110_kurz[] = {
    0x04, 0x02,                                            /* Kopf       ROOM1110 @0x0D68  */
    0x25,0x50,0x3a,0x4f,0x00,0x3d,0x00,0x4c,0x4b,0x48,0x45,0x3f,0x41,0x00,0x4b,0x42,0x42,0x45,0x3f,
    0x41,0x4e,0x18,0x00,0x44,0x41,0x3a,0x4f,0x00,0x40,0x41,0x3d,0x40,0x57, /* ROOM1110 @0x0D6A */
    0x01, 0x00,                                            /* Ende       ROOM1110 @0x0DD3  */
};

/* "A miserable death..." ▼ "He is holding something." */
static const uint8_t k_1230_lang[] = {
    0x04, 0x02,                                            /* Kopf       ROOM1230 @0x16F4  */
    /* A   _  m    i    s    e    r    a    b    l    e   _  d    e    a    t    h    .    .    . */
    0x1d,0x00,0x49,0x45,0x4f,0x41,0x4e,0x3d,0x3e,0x48,0x41,0x00,0x40,0x41,0x3d,0x50,0x44,
    0x57,0x57,0x57,                                        /* ROOM1230 @0x16F6 (msg 10 S.1) */
    0x02, 0x00,                                            /* Umbruch    ROOM1230 @0x170A  */
    /* H    e   _  i    s   _  h    o    l    d    i    n    g   _  s    o    m    e    t
     * h    i    n    g                                                                     */
    0x24,0x41,0x00,0x45,0x4f,0x00,0x44,0x4b,0x48,0x40,0x45,0x4a,0x43,0x00,0x4f,0x4b,0x49,0x41,0x50,
    0x44,0x45,0x4a,0x43,                                   /* ROOM1011 @0x012A3 (msg 19; einzeln
                                                            * "He is holding" ROOM1230 @0x170C)       */
    0x57,                                                  /* "."        ROOM1230 @0x1720  */
    0x01, 0x00,                                            /* Ende       ROOM1230 @0x1752  */
};

/* "A miserable death..." — Seite 1 der Original-Nachricht, bytegleich. */
static const uint8_t k_1230_kurz[] = {
    0x04, 0x02,                                            /* Kopf       ROOM1230 @0x16F4  */
    0x1d,0x00,0x49,0x45,0x4f,0x41,0x4e,0x3d,0x3e,0x48,0x41,0x00,0x40,0x41,0x3d,0x50,0x44,
    0x57,0x57,0x57,                                        /* ROOM1230 @0x16F6 */
    0x01, 0x00,                                            /* Ende       ROOM1230 @0x1752  */
};

typedef struct {
    unsigned       raum;       /* RE15_ROOM_BASE */
    uint8_t        msg_orig;   /* Id am Message_on des Original-Ereignisses */
    uint8_t        msg_lang, msg_kurz;
    uint8_t        bit;        /* Bank 9 */
    const uint8_t *lang;  uint8_t lang_n;
    const uint8_t *kurz;  uint8_t kurz_n;
} leiche_t;

static const leiche_t k_leichen[2] = {
    { RE15_LEICHE_1110_RAUM, RE15_LEICHE_1110_MSG_ORIG, RE15_LEICHE_1110_MSG_LANG,
      RE15_LEICHE_1110_MSG_KURZ, RE15_LEICHE_1110_BIT,
      k_1110_lang, (uint8_t)sizeof k_1110_lang, k_1110_kurz, (uint8_t)sizeof k_1110_kurz },
    { RE15_LEICHE_1230_RAUM, RE15_LEICHE_1230_MSG_ORIG, RE15_LEICHE_1230_MSG_LANG,
      RE15_LEICHE_1230_MSG_KURZ, RE15_LEICHE_1230_BIT,
      k_1230_lang, (uint8_t)sizeof k_1230_lang, k_1230_kurz, (uint8_t)sizeof k_1230_kurz },
};

static const leiche_t *finde_raum(unsigned raum_basis)
{
    for (int i = 0; i < 2; i++)
        if (k_leichen[i].raum == raum_basis) return &k_leichen[i];
    return 0;
}

/* ANGEBOTS-LATCH: gesetzt, wenn der LANGE Text aufgeht; geloest, sobald er zu ist (dann Modal) oder
 * sobald er nicht mehr der eigene sein kann.
 * ⛔ PORT-WAHL (Robustheit, keine Original-Adresse — Gegenpruefung Auflage 5): das Modal oeffnet
 * NUR, wenn die zuletzt geoeffnete Nachricht noch die eigene lange Id ist. Damit faellt das Angebot
 * auch ohne eigenen Teardown-Haken weg, wenn der Raum neu aufgebaut wird: scd_room_reenter nullt
 * g_scd komplett (`memset(&g_scd, 0, ...)`, scd_room_setup.c) -> message_id 0 != 20/22. Ebenso bei
 * einem Wechsel der Raum-Basis und bei einer angemeldeten Raum-Anfrage (g_room_change.pending). */
static uint8_t  s_angebot;
static unsigned s_raum;
static uint8_t  s_bit;
static uint8_t  s_lang_id;
#ifdef RE15_PLATFORM_PC
static uint8_t  s_modal_offen;      /* nur Logzeile: unser Modal laeuft gerade */
static int      s_menge_vorher;
#endif

const uint8_t *re15_leiche_text(unsigned raum, int kurz, int *out_len)
{
    const leiche_t *l = finde_raum(RE15_ROOM_BASE(raum));
    if (!l) { if (out_len) *out_len = 0; return 0; }
    if (out_len) *out_len = kurz ? (int)l->kurz_n : (int)l->lang_n;
    return kurz ? l->kurz : l->lang;
}

int re15_leiche_angebot_offen(void) { return s_angebot; }

int re15_leiche_message_on(const uint8_t *pc, uint32_t pause_mask)
{
    if (!pc) return 0;
    const unsigned basis = RE15_ROOM_BASE(g_current_room_id);
    const leiche_t *l = finde_raum(basis);
    /* Schluessel (Raum, Nachricht) — ROOM1230 msg 0 = Tastenfeld bleibt unberuehrt. */
    if (!l || pc[1] != l->msg_orig) return 0;

    const int genommen = re15_game_flag_get(9, l->bit) ? 1 : 0;
    const uint8_t  id  = genommen ? l->msg_kurz : l->msg_lang;
    const uint8_t *txt = genommen ? l->kurz     : l->lang;
    const size_t   n   = genommen ? l->kurz_n   : l->lang_n;

    /* Text unter der Port-Id ablegen — erst HIER, deshalb braucht es keinen Raumstart-Haken: der
     * Tuer- und der Ladeweg sind automatisch abgedeckt (re15_msg_clear_room_block raeumt beim
     * Teardown, der naechste Druck legt neu ab). Dauer wie tuer1120_1130.c. */
    re15_msg_install_text(id, txt, n);
    {
        int d = re15_msg_compute_duration(txt, n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(id, d);
    }
    /* Stimme wie im Klartext-Zweig (scd_vm.c scd_queue_voice): Datei
     * synchro/STAGE1/room<Raum-Id>/main<Id>.wav — fehlt sie, bleibt es stumm mit Untertitel. */
    {
        scd_audio_event_t vev;
        memset(&vev, 0, sizeof vev);
        vev.kind      = (uint8_t)SCD_AUDIO_VOICE_ON;
        vev.sample_id = id;
        scd_audio_queue_push(&vev);
    }
    /* Oeffnen wie der Klartext-Zweig (nicht blockierend, Schreibmaschine) mit der Maske DIESES
     * Message_on (0xffff -> Freeze 0xFFFF0000, @0x80040508 / @0x8004051c). */
    re15_dialog_open_mask((int)id, 0, pause_mask);
    g_scd.message_arg2 = pc[2];
    g_scd.message_arg3 = pc[3];

    s_angebot = (uint8_t)!genommen;
    s_raum    = basis;
    s_bit     = l->bit;
    s_lang_id = l->msg_lang;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[leiche] ROOM%04X msg %d -> Port-Nachricht %d (Bit (9,%d)=%d)%s\n",
            (unsigned)g_current_room_id, (int)pc[1], (int)id, (int)l->bit, genommen,
            genommen ? "" : ", Angebot H. Gun Bullets nach dem Text");
#endif
    return 1;
}

int re15_leiche_tick(void)
{
#ifdef RE15_PLATFORM_PC
    /* Protokoll fuer den Nutzer (befund.log) und die Abnahme: wie ging unser Modal aus? */
    if (s_modal_offen && !re15_item_modal_active()) {
        s_modal_offen = 0;
        int nachher = 0;
        for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
            if (g_inv.slots[i].id == RE15_LEICHE_ITEM) nachher += g_inv.slots[i].qty;
        if (re15_game_flag_get(9, s_bit))
            fprintf(stderr, "[leiche] Yes: Bit (9,%d) gesetzt, H. Gun Bullets %d -> %d\n",
                    (int)s_bit, s_menge_vorher, nachher);
        else
            /* derselbe Zweig fuer "No" und "Inventar voll": @0x8001e054 `bltz` / @0x8001e06c `bne`
             * -> @0x8001e0ec (Zustand 8, nichts eingefuegt, kein Bit) */
            fprintf(stderr, "[leiche] No/voll: Munition bleibt bei der Leiche (Bit (9,%d)=0), "
                            "das naechste Untersuchen bietet sie wieder an\n", (int)s_bit);
    }
#endif
    if (!s_angebot) return 0;
    if (RE15_ROOM_BASE(g_current_room_id) != s_raum || g_room_change.pending) {
        s_angebot = 0;                            /* PORT-WAHL, s. s_angebot */
        return 0;
    }
    /* ⛔ EIN NACHRICHTENKANAL. Das Aufnahme-Modal oeffnet seine Frage mit DERSELBEN Routine wie der
     * Raumtext (Message_on `jal 0x80027e68` @0x80040518): Zustand 5 Ja/Nein `ori a1,zero,0x100`
     * @0x8001df6c -> `jal 0x80027e68` @0x8001dfe0, voll `ori a1,zero,0x100` @0x8001df88 ->
     * `jal 0x80027e68` @0x8001df90. Und die verwirft jedes Oeffnen, solange ein Text offen ist:
     *     80027e74  lbu  v0,0(v1)           ; 0x800b8520
     *     80027e7c  andi v0,v0,0x80         ; Belegt-Bit
     *     80027e80  beq  v0,zero,0x80027e90
     *     80027e88  j    0x800280ac         ; -> return -1, NICHTS geoeffnet
     * Also erst, wenn das Belegt-Bit frei ist — re15_pauseflags_belegt() IST dieses Bit im Port
     * (geloescht an den drei Dismiss-Stellen @0x80028598 / @0x800286c0 / @0x8002870c, zusammen mit
     * dem Freeze). NICHT g_scd.message_active: das bleibt im Untertitel-Nachhall (msg-FSM Zustand 7)
     * auf 1, obwohl der Freeze schon geloest ist (Herleitung game_state.c re15_pauseflags_belegt). */
    if (re15_pauseflags_belegt()) return 0;
    /* Laeuft schon ein Modal, warten — der Original-Handler startet nur aus Zustand 0
     * (@0x80043334 `bne v0,zero,0x80043368`). */
    if (re15_item_modal_active()) return 0;
    s_angebot = 0;
    if (g_scd.message_id != s_lang_id) return 0;  /* PORT-WAHL, s. s_angebot (Auflage 5) */
    if (re15_game_flag_get(9, s_bit)) return 0;

    /* Nachhall der Leichen-Zeile beenden: ein Kanal, eine Zeile (wie item_discard_common.c vor
     * seiner Frage; RE2 oeffnet dort mit demselben FUN_8002fe38). Nur wirksam im Nachhall-Zustand
     * 7 — gibt es keine Aufnahme main20..23.wav, ist das ein No-op. */
    re15_msg_nachhall_beenden();
    /* Das NORMALE Aufnahme-Modal (FUN_8001db28-Port), Argumente:
     *   aot_slot   -1   — keine Zone abschalten: die Zone ist das Leichen-EREIGNIS, das nach "Ja" fuer
     *                     den kurzen Text scharf bleiben MUSS ("Danach soll nur noch ... kommen, wenn
     *                     man ihn anklickt"); es legt sich selbst still und wieder scharf (@0x0CEE /
     *                     @0x0D18, @0x014A6 / @0x014D0). Muster sicherung_1150.c.
     *   taken_bit  61/62 — gesetzt NUR bei Ja (`jal 0x8004ef90` @0x8001e0d0, Bank 9 @0x8001e0d4).
     *   taken_prop 0xFF — kein Weltmodell (RE2 ROOM4050 @0x00F1A md1 = 0xFF; beide RE1.5-Leichen
     *                     haben kein Prop). */
    re15_item_modal_start(RE15_LEICHE_ITEM, RE15_LEICHE_MENGE, s_bit, -1, 0xFF);
#ifdef RE15_PLATFORM_PC
    s_modal_offen = 1;
    s_menge_vorher = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == RE15_LEICHE_ITEM) s_menge_vorher += g_inv.slots[i].qty;
    fprintf(stderr, "[leiche] ROOM%04X Text zu -> Aufnahme-Modal H. Gun Bullets x%d (Bit (9,%d))\n",
            (unsigned)g_current_room_id, RE15_LEICHE_MENGE, (int)s_bit);
#endif
    return 1;
}
