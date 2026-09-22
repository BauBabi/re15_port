/*
 * item_discard_common.c — "You don't need this key any more. Discard it?"
 *
 * Die vollstaendige Herleitung (Wortlaut @0x800C508B, RE2-Regel LAB_80051718,
 * Benutzungsstellen-Zensus, Sackgassen-Beweis) steht im Kopf von
 * include/re15_item_discard.h. Hier nur noch, was der Code TUT, mit den Adressen
 * an den einzelnen Zeilen.
 */
#include "re15_item_discard.h"
#include "re15_inventory.h"    /* g_inv, re15_inv_find_item, re15_inv_remove_slot        */
#include "re15_scd.h"          /* re15_pauseflags_belegt — RE2s Belegt-Bit @0x800517f0   */
#include "re15_item_prompt.h"  /* re15_item_prompt_walk — Glyphenzahl des Skripts        */
#include "re15_msg.h"          /* re15_msg_nachhall_beenden — @0x80051834 = @0x8005164C  */
#include "re15_msg_select.h"   /* re15_msg_select_blink_tick — Blink-Gatter @0x80028600  */

#include "gen/discard_sites.inc"

/* Port-Prompt-Schluessel 8 -> BSS-Skript [6] (item_prompt_common.c:60). */
#define RE15_DISCARD_PROMPT_KEY 8

/* Schreibmaschinen-Kadenz: identisch zum Aufnahme-Prompt, byte-true FUN_80028134
 * (@0x800281b0-c4 `DAT_800b8524 = 2 << s1`, s1 = 0 -> 1 Glyphe je 2 Bilder). Der
 * Schnellvorlauf ist derselbe: HELD virtuell 0x4000 -> Zaehler -= 4 (@0x80028228),
 * 2 Glyphen je Ablauf (@0x8002822c `sltiu 4` -> `ori 2`). Uebernommen aus
 * item_modal_common.c, damit beide Prompts identisch tippen. */
#define DISCARD_MSG_RELOAD 2

enum { D_AUS = 0, D_WARTET, D_FRAGT };

static int     s_zustand   = D_AUS;
static uint8_t s_item      = 0;
static int     s_slot      = -1;
static int     s_choice    = 0;    /* 0 = Yes (vorbelegt), 1 = No                       */
static int     s_reveal    = 0;
static int     s_reveal_total = 0;
static int     s_reveal_timer = 0;
static uint8_t s_blink     = 0;    /* DAT_800b8525 dieser Abfrage (@0x800285e8)         */
static int     s_gefragt   = 0;
static int     s_weggeworfen = 0;

/* DER VORENTSCHEID = RE2s @0x80051628 `jal FUN_800696cc` + @0x80051634 `bltz s1`.
 * Er faellt VOR dem Oeffnen der Nachricht; re15_discard_notice_message merkt danach nur
 * noch vor, wenn er fuer DIESELBE (Raum, Nachricht) mit Treffer gefallen ist. */
static struct { unsigned room; uint8_t msg; uint8_t item; int slot; int gilt; } s_vor;
static int s_vorentscheide = 0, s_vorentscheide_besitz = 0, s_vorentscheid_belegt = 0;

/* Anzahl 0 zurueckdrehen — dasselbe, was RE2 in JEDEM Zweig tut, der NICHT wegwirft
 * (@0x800517C4 `sb v1,count` mit v1 = 1). Seit die Anzahl erst beim Fragen faellt (siehe
 * re15_discard_tick), kann das nur noch den Zustand D_FRAGT betreffen; in D_WARTET ist die
 * Anzahl unberuehrt (@0x800517f4 verlaesst die Routine VOR dem Dekrement @0x80051810).
 *
 * ⛔ DER PLATZ WIRD NACHGEZOGEN, DIE ID ENTSCHEIDET. Dasselbe Argument wie an den zwei
 * anderen Stellen dieser Datei (Einhaengen und Fragen): re15_inv_remove_slot kompaktiert
 * das Inventar (FUN_8004dadc), ein gemerkter INDEX kann danach auf einen fremden Platz
 * zeigen. Ohne das Nachziehen blieb genau in diesem Fall ein Platz mit Anzahl 0 stehen —
 * ein Gegenstand, den der Spieler sieht und nicht mehr benutzen kann. Riegel TEIL Q faehrt
 * den Fall samt Gegenprobe. */
static void anzahl_zurueck(void)
{
    if (s_zustand == D_AUS || s_item == 0) return;
    int slot = s_slot;
    if (slot < 0 || slot >= RE15_INV_MAX_SLOTS || g_inv.slots[slot].id != s_item)
        slot = re15_inv_find_item(s_item);
    if (slot >= 0 && slot < RE15_INV_MAX_SLOTS
        && g_inv.slots[slot].id == s_item && g_inv.slots[slot].qty == 0)
        g_inv.slots[slot].qty = 1;
}

void re15_discard_reset(void)
{
    /* HART: Raumwechsel, neues Spiel, Spielstand laden, Game-Over. Alles faellt weg.
     * ⛔ DASS DAS BEIM RAUMWECHSEL GEFAHRLOS IST, IST GEMESSEN, nicht gewaehlt: die
     * Spanne "vorgemerkt" liegt jetzt vollstaendig im Freeze der ausloesenden Nachricht,
     * und der haelt AUCH den Skript-Runner (RE15_PAUSE_SCD 0x02000000, Gate @0x8003f044
     * `lui v1,0x200` / @0x8003f04c). Alle 16 ausgelieferten Benutzungsstellen oeffnen
     * ihre Nachricht mit Maske 0xFFFF0000 (Zensus: `Message_on <id> ff ff`, pc[2..3]<<16,
     * scd_vm.c op_message_on) — in dieser Spanne kann WEDER der Spieler (Pad auf 0xf000,
     * @0x800304f4-@0x8003051c) NOCH das Skript einen Raumwechsel ausloesen. Die frueher
     * hier noetige Port-Entscheidung "darf die Vormerkung den Raumwechsel ueberleben?"
     * entfaellt damit ersatzlos; der Riegel TEIL I misst die Eigenschaft je Stelle. */
    anzahl_zurueck();
    s_zustand = D_AUS; s_item = 0; s_slot = -1; s_choice = 0;
    s_reveal = s_reveal_total = s_reveal_timer = 0; s_blink = 0;
    s_vor.gilt = 0;                /* ein halb gefallener Vorentscheid gilt nicht weiter */
}

uint8_t re15_discard_pending_item(void) { return (uint8_t)(s_zustand == D_AUS ? 0 : s_item); }

/* ⛔ HARTER RESET. `item` wird ABSICHTLICH nicht gelesen: ein Load darf keine Abfrage
 * des vorigen Laufs erben. Herleitung (RE2s Fortsetzungs-Zeiger liegt im RAM und in
 * keinem Speicherformat; der Auslieferungsstand kann nicht speichern) im Header. */
void re15_discard_restore(uint8_t item)
{
    (void)item;
    re15_discard_reset();
}

/* (1)+(2) von RE2s Reihenfolge: Platz suchen (@0x80051628 `jal FUN_800696cc`,
 * @0x80051630 `move s1,v0`) und ohne Treffer abzweigen (@0x80051634 `bltz s1`).
 * Gerufen von op_message_on UNMITTELBAR VOR dem Oeffnen der Nachricht. */
int re15_discard_besitz_vor_nachricht(unsigned room_id, uint8_t msg_id)
{
    s_vor.gilt = 0;
    if (s_zustand != D_AUS) return 0;          /* eine Abfrage laeuft schon              */
    uint8_t item = 0;
    const re15_discard_site_t *s = NULL;
    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) {
        if (re15_discard_sites[i].room == (uint16_t)room_id
            && re15_discard_sites[i].msg == msg_id) { s = &re15_discard_sites[i]; break; }
    }
    if (!s) return 0;                          /* keine Benutzungsstelle                 */

    /* ⛔ DAS CODE-PANEL-GATE, und zwar VOR der Besitzpruefung. Es entscheidet nicht, OB
     * gefragt werden darf, sondern ob (Raum, Nachricht) ueberhaupt eine Wegwerf-Stelle
     * IST: an einem Tor mit Zifferncode ist der Schluessel erst erledigt, wenn der Code
     * einmal richtig eingegeben wurde. Die Stelle haengt deshalb an der Erfolgs-Nachricht
     * des Panels ("You've opened the lock."), und diese Zeile steht in 22 Raeumen — das
     * Gate-Bit trennt die vier gemeinten von den uebrigen. Fail-closed: kein Flag, keine
     * Stelle. Bit und Ausloeser sind GEMESSEN (Generator-Bedingung E, gen_discard_sites.py):
     *   ROOM10D0 sub01 @0x01512 Ck(3,50,0) + @0x01516 Ck(5,13..16,1) (die vier Ziffern)
     *                  @0x01526 Evt_exec sub19  -> @0x0152A Set(3,50,1) = ERFOLG
     *   sub19          @0x0199E Message_on 5    = der neue Ausloeser
     * `Evt_exec` gibt die Kontrolle nicht ab (op_evt_exec, byte-true @0x8003f2b8), das
     * Bit steht also bereits, wenn sub19 seine Nachricht oeffnet.
     * Die Besitzpruefung darunter bleibt unveraendert an der RE2-Kettenposition
     * (@0x80051628 suchen / @0x80051634 verzweigen, beide VOR @0x8005164C). */
    if (s->gate_zone && !re15_game_flag_get(s->gate_zone, s->gate_bit)) return 0;
    item = s->item;
    s_vorentscheide++;
    /* MESSGROESSE FUER DIE KETTENPOSITION: RE2 entscheidet VOR @0x8005164C, die Nachricht
     * ist zu diesem Zeitpunkt also noch nicht offen. Jeder Vorentscheid, der das
     * Belegt-Bit schon gesetzt findet (@0x800517f0 `andi v0,v0,0x80`), saesse hinter dem
     * Oeffnen also an der falschen Stelle der Kette. Der Riegel TEIL L verlangt 0. */
    if (re15_pauseflags_belegt()) s_vorentscheid_belegt++;

    /* @0x80051628 — den Platz SUCHEN. re15_inv_find_item ist der Port-Gegenwert zu
     * FUN_800696cc (lineare Slot-Suche nach der Id, inventory_common.c:144-149). */
    int slot = re15_inv_find_item(item);
    /* @0x80051634 `bltz s1,LAB_800516a0` — KEIN Treffer, also der andere Zweig. Dort
     * haengt RE2 keine Fortsetzung ein (@0x800516C0 `j LAB_800516f8`), es kann also nie
     * eine Abfrage folgen. Im Port heisst dieser Zweig: Vorentscheid ungueltig. */
    if (slot < 0 || slot >= RE15_INV_MAX_SLOTS) return 0;
    if (g_inv.slots[slot].qty == 0) return 0;  /* schon verbraucht -> nichts zu tun      */

    s_vor.room = room_id; s_vor.msg = msg_id;
    s_vor.item = item;    s_vor.slot = slot;   s_vor.gilt = 1;
    s_vorentscheide_besitz++;
    return 1;
}

int re15_discard_vorentscheide(void)             { return s_vorentscheide; }
int re15_discard_vorentscheid_belegt(void)       { return s_vorentscheid_belegt; }
int re15_discard_vorentscheide_mit_besitz(void)  { return s_vorentscheide_besitz; }

/* (4) von RE2s Reihenfolge: die Fortsetzung EINHAENGEN (@0x80051670
 * `sw v0=>LAB_80051718,-0x7d50(at)`), unmittelbar nachdem die Nachricht offen ist. */
void re15_discard_notice_message(unsigned room_id, uint8_t msg_id)
{
    if (s_zustand != D_AUS) { s_vor.gilt = 0; return; }   /* eine Abfrage laeuft schon   */

    /* ⛔ FAIL-CLOSED. Vorgemerkt wird NUR, was der Vorentscheid von @0x80051628/
     * @0x80051634 fuer GENAU DIESE (Raum, Nachricht) mit Treffer freigegeben hat. Ohne
     * ihn passiert nichts — ein neuer Oeffnungsweg fuer Nachrichten kann die Abfrage
     * also nicht ohne Besitzpruefung armieren. */
    if (!s_vor.gilt || s_vor.room != room_id || s_vor.msg != msg_id) { s_vor.gilt = 0; return; }
    uint8_t item = s_vor.item;
    int     slot = s_vor.slot;
    s_vor.gilt = 0;

    /* Den Platz nachziehen: zwischen Vorentscheid und Einhaengen liegt das Oeffnen der
     * Nachricht; ein Nachruecken (re15_inv_remove_slot, FUN_8004dadc) kann den Platz
     * verschoben haben. Die Id entscheidet, nicht der Index. */
    if (slot < 0 || slot >= RE15_INV_MAX_SLOTS || g_inv.slots[slot].id != item)
        slot = re15_inv_find_item(item);
    if (slot < 0 || slot >= RE15_INV_MAX_SLOTS || g_inv.slots[slot].qty == 0) return;

    /* ⛔ HIER wird NICHT dekrementiert. RE2 haengt an dieser Stelle nur die Fortsetzung
     * LAB_80051718 ein (`sw v0,[0x800D4498]` @0x80051670); der Zaehler faellt erst in der
     * Fortsetzung, und zwar NACH der Warte-Schranke — siehe re15_discard_tick. Wer schon
     * beim Einhaengen dekrementiert, zeigt dem Spieler waehrend der ganzen Wartezeit einen
     * Schluessel mit Anzahl 0 im Inventar. */
    s_item    = item;
    s_slot    = slot;
    s_choice  = 0;                             /* Yes vorbelegt — RE2 setzt die Auswahl-
                                                * zelle beim Nachrichtenstart auf exakt
                                                * 0x80, unteres Nibble 0 = Yes
                                                * (@0x8002FE88); der Aufnahme-Prompt des
                                                * Ports startet ebenso auf 0.            */
    s_zustand = D_WARTET;                      /* erst die Raum-Nachricht ausreden lassen */
}

void re15_discard_tick(uint16_t pad_edge, uint16_t pad_held)
{
    if (s_zustand == D_AUS) return;

    if (s_zustand == D_WARTET) {
        /* ⛔ DIE EINZIGE SCHRANKE — und sie ist RE2 woertlich, nicht nachempfunden:
         *     800517e0  lbu  v0,-0x78c4(v0)         DAT_800e873c
         *     800517f0  andi v0,v0,0x80             das BELEGT-Bit des Nachrichtensystems
         *     800517f4  bne  v0,zero,LAB_80051870   belegt -> return, NICHTS angefasst
         * re15_pauseflags_belegt() IST dieses Bit im Port (DAT_800b8520 & 0x80,
         * Open-Guard @0x80027e7c, geloescht @0x80028598/@0x800286c0/@0x8002870c —
         * dieselben drei Stellen, an denen RE2 den Freeze zuruecknimmt, LAB_800307e0
         * @0x800307e8/@0x800307f4). Beleg an der Definition in game_state.c.
         *
         * ⛔ HIER STANDEN BIS RUNDE 22 ZWEI WEITERE SCHRANKEN (Faden-Ende und
         * Szenen-Fenster). Sie sind ERSATZLOS WEG, weil GENAU SIE das Loch aufgerissen
         * haben: sie verlaengerten die Spanne "vorgemerkt" ueber das Ende des
         * Nachrichten-Freezes hinaus, und in dem Ueberhang lief der Spieler frei
         * (ROOM1100 sub02 msg 4: 80 Bilder, ROOM4000 sub02 msg 2: 29 Bilder; Zerlegung
         * im Riegel TEIL I). Mit der Belegt-Bit-Schranke allein deckt sich die Spanne
         * mit dem Freeze, so wie in RE2.
         *
         * ⛔ ABER KEIN ABSOLUTSATZ: das Fenster ist ABGESICHERT, nicht verschwunden.
         * ⛔ BERICHTIGT: die Zahl 161 war festgetippt. Gemessen sind 887 Bilder
         * Ueberhang ueber alle gefahrenen Stellen, groesstes Einzelfenster 182.
         * Daraus ist GENAU
         * 1 BILD JE STELLE geworden — das Schliess-Bild, in dem re15_msg_tick den
         * Freeze schon geloest und re15_discard_tick die Abfrage noch nicht aufgemacht
         * hat. Gehalten wird es von der PORT-Zeile re15_discard_frozen() (s. deren
         * Definition weiter unten), gemessen je Stelle vom Riegel TEIL I
         * (`ohne_bruecke == 1` an 10 von 10 gefahrenen Stellen) und in seiner Wirkung
         * von TEIL J (Rueckbau dieses einen Bildes: 680 Einheiten SUMME ueber die
         * Stellen, je Stelle 75..101 — nicht 680 je Stelle).
         * RE2 hat hier 0 solche Bilder, weil Dekrement und neuer Freeze im SELBEN
         * Aufruf liegen: @0x80051810 `addiu v0,v0,-1`, @0x80051834 `jal FUN_8002fe38`,
         * @0x80051844 `lui v1,0xff00` / @0x80051850 `sw v0,DAT_800cfbdc`.
         *
         * Dass ein SICHTBARER Prompt das ausloesende Unterprogramm anhaelt, ist dabei
         * KEIN Defekt, sondern byte-true: RE2 legt beim Fragen sofort wieder die ganze
         * Maske an (@0x80051844 `lui v1,0xff00` / @0x80051850 `sw v0,DAT_800cfbdc`), und
         * 0xFF000000 enthaelt das Skript-Bit 0x02000000. Das Unterprogramm parkt also
         * genau an der Anweisung, an der die Nachricht es gelassen hat, und laeuft nach
         * der Antwort weiter (Riegel TEIL J faehrt das mit ROOM1090 sub03 durch, dem
         * Fall, an dem Runde 21 den Gegenbeweis zu sehen glaubte — dort wurde die Frage
         * nie beantwortet, weil der Pruefstand keine Taste drueckte). */
        if (re15_pauseflags_belegt()) return;
        /* Den Platz nachpruefen: das Inventar kann zwischen Einhaengen und Fragen
         * nachgerueckt sein (re15_inv_remove_slot kompaktiert, FUN_8004dadc). Ist der
         * Gegenstand ganz weg (Itembox o.ae.), gibt es nichts mehr wegzuwerfen. */
        if (s_slot < 0 || s_slot >= RE15_INV_MAX_SLOTS || g_inv.slots[s_slot].id != s_item)
            s_slot = re15_inv_find_item(s_item);
        if (s_slot < 0 || s_slot >= RE15_INV_MAX_SLOTS || g_inv.slots[s_slot].id != s_item
            || g_inv.slots[s_slot].qty == 0) {
            re15_discard_reset(); return;
        }
        /* JETZT erst faellt der Zaehler — byte-true die Reihenfolge von LAB_80051718:
         * die Warte-Schranke @0x800517F4 `bne v0,zero,0x80051870` verlaesst die Routine,
         * OHNE die Anzahl anzufassen; erst dahinter steht
         *   80051808 lbu   v0,count
         *   80051810 addiu v0,v0,-1      ; Zaehler -= 1
         *   8005181C sb    v0,count
         *   80051824 bne   v0,zero,...   ; > 0 -> KEINE Abfrage
         *   80051834 jal   FUN_8002FE38  ; == 0 -> FRAGEN (a1=0x100 @0x8005182C, a2=9)
         * Gemessen (tools/gen_discard_sites.py): alle neun Gegenstaende werden mit Anzahl 1
         * ausgegeben und haben genau EINE Benutzungsstelle — die Anzahl faellt also von 1
         * auf 0 und es wird gefragt. Die Regel steht trotzdem als Zaehler da: ein Gegenstand
         * mit mehreren Benutzungsstellen wuerde sonst zu frueh gemeldet. */
        uint8_t n = (uint8_t)(g_inv.slots[s_slot].qty - 1);   /* @0x80051810 */
        g_inv.slots[s_slot].qty = n;                          /* @0x8005181C */
        if (n != 0) { re15_discard_reset(); return; }         /* @0x80051824 */
        /* ⛔ DER NACHHALL DER AUSLOESENDEN ZEILE ENDET HIER. RE2 oeffnet die Abfrage mit
         * DERSELBEN Nachrichten-Routine, mit der es die Zeile "You have used the <Name>."
         * geoeffnet hat:
         *     8005182C  li   a1,0x100
         *     80051830  li   a2,0x9          ; Prompt-Skript 9
         *     80051834  jal  FUN_8002fe38    ; = derselbe Aufruf wie @0x8005164C
         * Ein Kanal, eine Zeile: die vorige kann den Prompt nicht ueberleben. Der
         * UNTERTITEL-NACHHALL des Ports (msg-FSM Zustand 7, Nutzer-Entscheidung
         * 2026-09-20, msg_common.c:565-575) ist der einzige Zustand, in dem Text steht,
         * waehrend der Freeze schon geloest ist — in RE2 gibt es den nicht, dort sind
         * @0x800307e8 `andi v0,v0,0x7f` (Belegt-Bit loeschen) und @0x800307f4
         * `sw v1,DAT_800cfbdc` (Pause-Schnappschuss zuruecklegen) EIN Paar im selben
         * Block. Der Nachhall bleibt ueberall sonst erhalten (er ist die
         * Nutzer-Entscheidung), aber er ueberlappt die Abfrage nicht mehr. */
        re15_msg_nachhall_beenden();
        s_reveal       = 0;
        s_reveal_total = re15_item_prompt_walk(RE15_DISCARD_PROMPT_KEY, s_item,
                                               0, 0, 0);   /* nur zaehlen */
        s_reveal_timer = 1;                    /* Startwert 1 << s1, s1 = 0 (@0x800281a0-ac) */
        s_blink        = 0;                    /* Blink-Zaehler beim Oeffnen (@0x80027eb0)   */
        s_zustand      = D_FRAGT;
        s_gefragt++;
        return;
    }

    /* --- D_FRAGT: tippen, dann Yes/No ------------------------------------------------ */
    if (s_reveal < s_reveal_total) {
        int budget = 1;                                   /* s2 = 1            @0x800281d8 */
        int t0     = s_reveal_timer;
        s_reveal_timer = t0 - 1;                          /* @0x800281f0/f8               */
        if (((s_reveal_timer & 0xff) != 0) && (pad_held & 0x4000)) {
            s_reveal_timer = t0 - 4;                      /* @0x80028228                  */
            if (DISCARD_MSG_RELOAD < 4) budget = 2;       /* @0x8002822c/38               */
        }
        if (s_reveal_timer > 0) return;                   /* @0x8002823c-48               */
        s_reveal += budget;
        if (s_reveal > s_reveal_total) s_reveal = s_reveal_total;
        s_reveal_timer = DISCARD_MSG_RELOAD;
        return;                                           /* vor dem Ende keine Eingabe   */
    }

    /* Auswahl + Bestaetigen — dieselben virtuellen Bits wie der Aufnahme-Prompt
     * (item_modal_common.c Zustand 6): 0x3000 = Menue links/rechts (roh Steuerkreuz),
     * 0x4000 = BESTAETIGEN (roh SQUARE, Preset-Tabelle @0x80073dbc[14]). */
    /* Blink-Zaehler des Cursors — ein Bild, byte-true @0x800285d4 vor @0x800285f0
     * (Herleitung in include/re15_msg_select.h). Erst nullen, dann dekrementieren:
     * ein Tastendruck macht den Cursor im SELBEN Bild wieder sichtbar. */
    s_blink = re15_msg_select_blink_tick(s_blink, (pad_edge & 0x3000) != 0);
    if (pad_edge & 0x3000) s_choice ^= 1;      /* @0x800285d8 xori v0,v0,0x1 */
    /* ABBRECHEN mit CROSS (virtuell 0x8000) = "No". Das ist die Konvention des Ports
     * (Bestaetigen SQUARE, Abbrechen CROSS) und KEINE byte-true Regel: RE2s Abfrage
     * kennt nur den Bestaetigen-Knopf (FUN_80030844, DAT_800ce310 & 0x1000), die
     * physische Taste dahinter ist in Untersuchung A ausdruecklich NICHT aufgeloest
     * worden. Wer abbricht, behaelt den Gegenstand — der sichere Ausgang. */
    if (pad_edge & 0x8000) { s_choice = 1; }
    else if (!(pad_edge & 0x4000)) return;

    if (s_choice == 0) {
        /* JA: Slot leeren + nachruecken. RE2: `sb zero,id` @0x80051774,
         * `sb zero,flags` @0x80051794, `jal FUN_80069714` @0x80051798.
         * Der Port hat dafuer re15_inv_remove_slot (Consume @0x8004aef0-af28) samt
         * anschliessender Kompaktierung (FUN_8004dadc). */
        if (s_slot >= 0 && s_slot < RE15_INV_MAX_SLOTS && g_inv.slots[s_slot].id == s_item) {
            re15_inv_remove_slot(s_slot);
            s_weggeworfen++;
            s_zustand = D_AUS; s_item = 0; s_slot = -1;
            s_reveal = s_reveal_total = s_reveal_timer = 0; s_blink = 0;
            return;
        }
        /* ⛔ DER PLATZ TRAEGT DEN GEGENSTAND NICHT MEHR — also wird NICHTS weggeworfen,
         * und dann muss die schon gefallene Anzahl zurueck. RE2 schreibt die 1 in JEDEM
         * Zweig zurueck, der nicht wegwirft (@0x800517C4 `sb v1,count`, v1 = 1); bis
         * Runde 24 fiel dieser Zweig hier ohne anzahl_zurueck() durch und liess einen
         * Platz mit Anzahl 0 stehen. Dass der Fall im Spiel nicht auftreten SOLL (der
         * sichtbare Prompt friert die Welt ein, @0x80051844/@0x80051850 mit 0xFF000000),
         * ist kein Grund, ihn offen zu lassen: der Freeze ist eine PORT-Zeile
         * (re15_discard_frozen), also wird hier fail-closed ausgestiegen.
         * Riegel TEIL Q faehrt den Fall mit Gegenprobe. */
        re15_discard_reset();
        return;
    }
    /* NEIN: Anzahl auf 1 zurueck (@0x800517C4) — sonst liefe sie beim naechsten
     * Gebrauch auf 0xFF unter, und es wuerde nie wieder gefragt. Die Abfrage kommt
     * beim naechsten Gebrauch erneut, genau wie in RE2. */
    re15_discard_reset();
}

int re15_discard_active(void) { return s_zustand != D_AUS; }

int re15_discard_frozen(void)
{
    if (s_zustand == D_FRAGT) return 1;
    /* ⛔ DAS SCHLIESS-BILD GEHOERT DAZU. RE2 laesst zwischen "Belegt-Bit frei" und
     * "Abfrage eingefroren" KEIN Bild: dieselbe Fortsetzung dekrementiert
     * (@0x80051810), fragt (@0x80051834) und legt die Maske im SELBEN Aufruf wieder an
     * (@0x80051844 `lui v1,0xff00` / @0x80051850 `sw v0,DAT_800cfbdc`).
     * Der Port tickt die Abfrage-FSM erst NACH re15_game_step (pc- und psx-main, damit
     * die Bestaetigungs-Flanke nicht zusaetzlich den AOT-Scan feuert). Ohne diese Zeile
     * waere genau das eine Bild frei, in dem re15_msg_tick den Freeze loest und
     * re15_discard_tick die Abfrage noch nicht aufgemacht hat. */
    return s_zustand == D_WARTET && !re15_pauseflags_belegt();
}

int re15_discard_prompt(uint8_t *out_item, int *out_choice)
{
    if (s_zustand != D_FRAGT) return 0;
    if (out_item)   *out_item   = s_item;
    if (out_choice) *out_choice = s_choice;
    return RE15_DISCARD_PROMPT_KEY;
}

uint8_t re15_discard_blink(void)    { return s_blink; }
int re15_discard_reveal(void)       { return s_reveal; }
int re15_discard_reveal_total(void) { return s_reveal_total; }
int re15_discard_ready(void)        { return s_zustand == D_FRAGT && s_reveal >= s_reveal_total; }
int re15_discard_gefragt(void)      { return s_gefragt; }
int re15_discard_weggeworfen(void)  { return s_weggeworfen; }
