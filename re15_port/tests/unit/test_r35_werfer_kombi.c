/* test_r35_werfer_kombi.c — Runde 35 Spur B: Inventar-Kombination der Werfer-Klasse und der Python.
 *
 * Faehrt die ECHTE Menue-Maschine (re15_menu_fsm_tick: GRID -> Kommando EXCHANGE -> Zustand 7 ->
 * Matcher/Executor -> Kompaktierung -> Ergebnis-Animation), wie der Nutzer sie bedient — Muster
 * tests/unit/test_inv_fsm.c (Welle 5). Belege: Dossier analysis/befunde_runde35/B_werfer.md §2.5,
 * Code engine/src/werfer_r35.c (re15_werfer_paar / re15_werfer_gl_tausch), Haken in menu_common.c.
 *
 *  K1  GL Explosiv 15 + EXPLOSIVE RND 0x19  -> Nachladen (RE1.5-Aktion 2, Satz `19 0f 02 00`
 *      @0x80074cb4), Magazin 6 (@0x80074dd4), Rest bleibt in der Runde.
 *  K2  GL Explosiv 15 + ACID ROUNDS 0x1a    -> RE2-Zustand 7 (@0x8006bc18): GL wird 16 (Saeure) mit
 *      dem GANZEN Stapel, die geladenen Explosiv-Runden kommen als 0x19 zurueck (MIXITEM-Bild 0x0c),
 *      Schwanzzelle der breiten Waffe zieht mit.
 *  K3  leerer GL 16 + INCEND. ROUNDS 0x1b   -> GL 17, die Runden-Zelle wird geloescht (Menge 0,
 *      `beq` @0x8006bd30) und kompaktiert.
 *  K4  Cursor auf der Runde (0x19) + GL 17  -> RE2-Zustand 8 (@0x8006bd98), Spiegel von K2.
 *  K5  Cursor auf der Runde (0x1a) + SCHWANZZELLE des GL 16 -> Normalisierung auf die Kopfzelle
 *      (@0x8004e948-7c), Nachladen in B (Aktion 3), VOLL-Zweig: Runde geloescht.
 *  K6  COLT PYTHON 20 + MAGNUM BULLETS 0x17 -> Nachladen (Aktion 2), Magazin 6 (@0x80074e98).
 *  K7  MAGNUM BULLETS + COLT PYTHON         -> Nachladen in B (Aktion 3).
 *  K8  SUPER REDHAWK 7 + MAGNUM (RE1.5-Satz `17 07 02 00` @0x80074c9c) laeuft unveraendert ueber
 *      die eingebettete Tabelle (der Haken greift nur fuer 15..17/20).
 *  K9  ROCKET LAUNCHER 18 + EXPLOSIVE RND   -> KEIN Paar (RE1.5 Zeile 18 ohne Satz, RE2 Id 17
 *      `04 00 00 00` + NULL-Satz @0x800a9ea4): Zustand 6, nichts aendert sich.
 *  K10 GEFUEHRTER GL 15 + ACID -> nach dem Schliessen des Menues ist die gefuehrte Waffe 16
 *      (RE2 @0x8006bc70-98 zieht die gefuehrte Id nach; Port: Schliess-Commit equip_id_now).
 */
#include <stdio.h>
#include <string.h>
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_inventory.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_werfer.h"

static int s_fails = 0;
#define CHECK(nr, c, ...) do { if (!(c)) { printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); s_fails++; } \
                               else { printf("ok   %d\n", (nr)); } } while (0)

static void frame(uint16_t pressed, uint16_t held)
{
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen())
        re15_menu_fsm_tick(pressed, held);
    if (re15_menu_is_open())
        re15_inv_screen_ecg_tick();
    re15_fade_tick();
}
static void idle(int n) { while (n-- > 0) frame(0, 0); }
static void druck(uint16_t b) { frame(b, b); }

static void leer(void)
{
    re15_inv_init();
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) { g_inv.slots[i].id = 0; g_inv.slots[i].qty = 0; g_inv.slots[i].flags = 0; }
    re15_inv_icon_reset();
}
static void breit(int kopf, uint8_t id, uint8_t menge)
{
    g_inv.slots[kopf].id = id;     g_inv.slots[kopf].qty = menge;     g_inv.slots[kopf].flags = 1;
    g_inv.slots[kopf + 1].id = id; g_inv.slots[kopf + 1].qty = menge; g_inv.slots[kopf + 1].flags = 2;
}
static void stueck(int zelle, uint8_t id, uint8_t menge)
{
    g_inv.slots[zelle].id = id; g_inv.slots[zelle].qty = menge; g_inv.slots[zelle].flags = 0;
}
/* Menue auf -> ITEM -> GRID (Cursor 0); gefuehrt = Zelle `eq` (0x80 = nichts). */
static void auf(int eq)
{
    re15_inv_set_equipped_slot(eq);
    re15_inv_set_prev_equip_slot(0x80);
    re15_menu_toggle();
    druck(RE15_PAD_BIT_SQUARE);
    idle(8);
}
/* GRID-Bestaetigung -> Kommandoleiste -> RIGHT (EXCHANGE) -> Bestaetigung = Zustand 7. */
static void tausch_modus(int nr)
{
    druck(RE15_PAD_BIT_SQUARE);
    idle(8);
    druck(RE15_PAD_BIT_RIGHT);
    druck(RE15_PAD_BIT_SQUARE);
    CHECK(nr, g_inv_screen.item_state == 7, "EXCHANGE -> Zustand 7, ist %d", g_inv_screen.item_state);
}
static void zu(int nr, int erfolg)
{
    if (erfolg) {
        CHECK(nr, re15_menu_item_c3() == 1, "Erfolg -> Ergebnis-Animation (25c3 = 1), ist %d", re15_menu_item_c3());
        idle(17);
    } else {
        CHECK(nr, g_inv_screen.item_state == 6 && re15_menu_item_c3() == 0,
              "kein Paar -> Zustand 6 ohne Animation, ist st=%d c3=%d", g_inv_screen.item_state, re15_menu_item_c3());
    }
    idle(8);
    re15_menu_toggle();
}

int main(void)
{
    uint8_t res = 0, pic = 0;

    /* ---- Paar-Tabelle (re15_werfer_paar) ---------------------------------------------------- */
    CHECK(1, re15_werfer_paar(15, 0x19, &res, &pic) == 2 && res == 15 && pic == 0, "15 + 0x19 = Nachladen in A (2), Ergebnis 15");
    CHECK(2, re15_werfer_paar(16, 0x1a, &res, &pic) == 2 && res == 16, "16 + 0x1a = Nachladen");
    CHECK(3, re15_werfer_paar(17, 0x1b, &res, &pic) == 2 && res == 17, "17 + 0x1b = Nachladen");
    CHECK(4, re15_werfer_paar(15, 0x1a, &res, &pic) == 7 && res == 15 && pic == 0x0c, "15 + ACID = Tausch (7), Bild 0x0c (EXPLOSIVE RND), res=%d pic=%d", res, pic);
    CHECK(5, re15_werfer_paar(16, 0x1b, &res, &pic) == 7 && pic == 0x0d, "16 + INCEND = Tausch, Bild 0x0d (ACID)");
    CHECK(6, re15_werfer_paar(17, 0x19, &res, &pic) == 7 && pic == 0x0e, "17 + EXPLOSIVE = Tausch, Bild 0x0e (INCEND.)");
    CHECK(7, re15_werfer_paar(0x19, 15, &res, &pic) == 3 && res == 15, "0x19 + 15 = Nachladen in B (3)");
    CHECK(8, re15_werfer_paar(0x19, 17, &res, &pic) == 8 && res == 15 && pic == 0x0e, "0x19 + 17 = Tausch (8), Ergebnis GL 15, Bild 0x0e; res=%d pic=%d", res, pic);
    CHECK(9, re15_werfer_paar(20, 0x17, &res, &pic) == 2 && res == 20, "Python + MAGNUM = Nachladen");
    CHECK(10, re15_werfer_paar(0x17, 20, &res, &pic) == 3 && res == 20, "MAGNUM + Python = Nachladen in B");
    CHECK(11, re15_werfer_paar(18, 0x19, &res, &pic) == 0 && re15_werfer_paar(7, 0x17, &res, &pic) == 0 &&
              re15_werfer_paar(14, 0x18, &res, &pic) == 0 && re15_werfer_paar(15, 0x17, &res, &pic) == 0 &&
              re15_werfer_paar(20, 0x19, &res, &pic) == 0, "Rakete/Redhawk/Flamme/Fremdmunition: kein Paar dieser Klasse");

    /* ---- K1 Nachladen GL Explosiv ------------------------------------------------------------ */
    leer(); breit(0, 15, 2); stueck(2, 0x19, 10);
    auf(0x80); tausch_modus(20);
    druck(RE15_PAD_BIT_DOWN);
    CHECK(21, g_inv_screen.second_cursor == 2, "K1 zweiter Cursor auf der Runde, ist %d", g_inv_screen.second_cursor);
    druck(RE15_PAD_BIT_SQUARE);
    CHECK(22, g_inv.slots[0].id == 15 && g_inv.slots[0].qty == 6 && g_inv.slots[2].id == 0x19 && g_inv.slots[2].qty == 6,
          "K1 GL 2 + 10 Runden -> Magazin 6 (@0x80074dd4), Rest 6; ist GL %02x q%d, Runde %02x q%d",
          g_inv.slots[0].id, g_inv.slots[0].qty, g_inv.slots[2].id, g_inv.slots[2].qty);
    zu(23, 1);

    /* ---- K2 Munitionswechsel Explosiv -> Saeure ---------------------------------------------- */
    leer(); breit(0, 15, 3); stueck(2, 0x1a, 5);
    auf(0x80); tausch_modus(30);
    druck(RE15_PAD_BIT_DOWN);
    druck(RE15_PAD_BIT_SQUARE);
    CHECK(31, g_inv.slots[0].id == 16 && g_inv.slots[0].qty == 5 && g_inv.slots[0].flags == 1,
          "K2 GL wird 16 (Saeure) mit dem ganzen Stapel 5; ist %02x q%d f%d", g_inv.slots[0].id, g_inv.slots[0].qty, g_inv.slots[0].flags);
    CHECK(32, g_inv.slots[1].id == 16 && g_inv.slots[1].flags == 2, "K2 Schwanzzelle zieht mit: %02x f%d", g_inv.slots[1].id, g_inv.slots[1].flags);
    CHECK(33, g_inv.slots[2].id == 0x19 && g_inv.slots[2].qty == 3,
          "K2 die 3 geladenen Explosiv-Runden kommen als 0x19 zurueck; ist %02x q%d", g_inv.slots[2].id, g_inv.slots[2].qty);
    CHECK(34, re15_inv_screen_cache_mix_pic(2) == 0x0c, "K2 MIXITEM-Bild 0x0c in der Runden-Zelle, ist %d", re15_inv_screen_cache_mix_pic(2));
    zu(35, 1);

    /* ---- K3 leerer GL Saeure + Brand: Runden-Zelle wird geloescht ----------------------------- */
    leer(); breit(0, 16, 0); stueck(2, 0x1b, 4); stueck(3, 0x01, 0);
    auf(0x80); tausch_modus(40);
    druck(RE15_PAD_BIT_DOWN);
    druck(RE15_PAD_BIT_SQUARE);
    CHECK(41, g_inv.slots[0].id == 17 && g_inv.slots[0].qty == 4, "K3 GL wird 17 (Brand) mit 4; ist %02x q%d", g_inv.slots[0].id, g_inv.slots[0].qty);
    CHECK(42, g_inv.slots[2].id == 0x01 && g_inv.slots[3].id == 0,
          "K3 leere Runde geloescht (`beq` @0x8006bd30) und kompaktiert (Messer rueckt nach): Zelle2 %02x Zelle3 %02x",
          g_inv.slots[2].id, g_inv.slots[3].id);
    zu(43, 1);

    /* ---- K4 Cursor auf der Runde, Partner = GL (RE2-Zustand 8) -------------------------------- */
    leer(); breit(0, 17, 1); stueck(2, 0x19, 7);
    auf(0x80);
    druck(RE15_PAD_BIT_DOWN); idle(1);
    CHECK(50, g_inv_screen.item_cursor == 2, "K4 Cursor auf der Runde (Zelle 2), ist %d", g_inv_screen.item_cursor);
    tausch_modus(51);
    druck(RE15_PAD_BIT_UP);
    CHECK(52, g_inv_screen.second_cursor == 0, "K4 zweiter Cursor auf dem GL, ist %d", g_inv_screen.second_cursor);
    druck(RE15_PAD_BIT_SQUARE);
    CHECK(53, g_inv.slots[0].id == 15 && g_inv.slots[0].qty == 7 && g_inv.slots[1].id == 15,
          "K4 GL wird 15 (Explosiv) mit 7; ist %02x q%d / %02x", g_inv.slots[0].id, g_inv.slots[0].qty, g_inv.slots[1].id);
    CHECK(54, g_inv.slots[2].id == 0x1b && g_inv.slots[2].qty == 1 && re15_inv_screen_cache_mix_pic(2) == 0x0e,
          "K4 Brand-Runde 0x1b q1 zurueck, Bild 0x0e; ist %02x q%d pic %d", g_inv.slots[2].id, g_inv.slots[2].qty, re15_inv_screen_cache_mix_pic(2));
    zu(55, 1);

    /* ---- K5 Runde + SCHWANZZELLE des passenden GL: Nachladen in B, VOLL-Zweig ------------------ */
    leer(); breit(0, 16, 2); stueck(2, 0x1a, 3); stueck(3, 0x01, 0);
    auf(0x80);
    druck(RE15_PAD_BIT_DOWN); idle(1);
    tausch_modus(60);
    druck(RE15_PAD_BIT_UP);
    druck(RE15_PAD_BIT_RIGHT);
    CHECK(61, g_inv_screen.second_cursor == 1, "K5 zweiter Cursor auf der Schwanzzelle, ist %d", g_inv_screen.second_cursor);
    druck(RE15_PAD_BIT_SQUARE);
    CHECK(62, g_inv.slots[0].id == 16 && g_inv.slots[0].qty == 5, "K5 2 + 3 = 5 <= 6 in die Kopfzelle; ist %02x q%d", g_inv.slots[0].id, g_inv.slots[0].qty);
    CHECK(63, g_inv.slots[2].id == 0x01, "K5 Runde verbraucht, Messer rueckt nach; Zelle2 = %02x", g_inv.slots[2].id);
    zu(64, 1);

    /* ---- K6 Python + MAGNUM ------------------------------------------------------------------ */
    leer(); stueck(0, 20, 1); stueck(1, 0x17, 10);
    auf(0x80); tausch_modus(70);
    druck(RE15_PAD_BIT_RIGHT);
    druck(RE15_PAD_BIT_SQUARE);
    CHECK(71, g_inv.slots[0].id == 20 && g_inv.slots[0].qty == 6 && g_inv.slots[1].id == 0x17 && g_inv.slots[1].qty == 5,
          "K6 Python 1 + 10 MAGNUM -> Magazin 6 (@0x80074e98), Rest 5; ist %02x q%d / %02x q%d",
          g_inv.slots[0].id, g_inv.slots[0].qty, g_inv.slots[1].id, g_inv.slots[1].qty);
    zu(72, 1);

    /* ---- K7 MAGNUM + Python ------------------------------------------------------------------ */
    leer(); stueck(0, 0x17, 4); stueck(1, 20, 1);
    auf(0x80); tausch_modus(80);
    druck(RE15_PAD_BIT_RIGHT);
    druck(RE15_PAD_BIT_SQUARE);
    CHECK(81, g_inv.slots[0].id == 20 && g_inv.slots[0].qty == 5 && g_inv.slots[1].id == 0,
          "K7 4 + 1 = 5 <= 6: Python q5, MAGNUM geloescht + kompaktiert; ist %02x q%d / %02x",
          g_inv.slots[0].id, g_inv.slots[0].qty, g_inv.slots[1].id);
    zu(82, 1);

    /* ---- K8 Redhawk + MAGNUM laeuft weiter ueber die RE1.5-Tabelle ----------------------------- */
    leer(); stueck(0, 7, 1); stueck(1, 0x17, 10);
    auf(0x80); tausch_modus(90);
    druck(RE15_PAD_BIT_RIGHT);
    druck(RE15_PAD_BIT_SQUARE);
    CHECK(91, g_inv.slots[0].id == 7 && g_inv.slots[0].qty == 6 && g_inv.slots[1].qty == 5,
          "K8 Redhawk 6 / MAGNUM 5 (Satz `17 07 02 00` @0x80074c9c); ist %02x q%d / q%d",
          g_inv.slots[0].id, g_inv.slots[0].qty, g_inv.slots[1].qty);
    zu(92, 1);

    /* ---- K9 Raketenwerfer: kein Paar ---------------------------------------------------------- */
    leer(); breit(0, 18, 2); stueck(2, 0x19, 10);
    auf(0x80); tausch_modus(100);
    druck(RE15_PAD_BIT_DOWN);
    druck(RE15_PAD_BIT_SQUARE);
    CHECK(101, g_inv.slots[0].id == 18 && g_inv.slots[0].qty == 2 && g_inv.slots[2].id == 0x19 && g_inv.slots[2].qty == 10,
          "K9 Rakete + Runde: nichts aendert sich; ist %02x q%d / %02x q%d",
          g_inv.slots[0].id, g_inv.slots[0].qty, g_inv.slots[2].id, g_inv.slots[2].qty);
    zu(102, 0);

    /* ---- K10 gefuehrter GL: nach dem Schliessen ist die gefuehrte Waffe die neue Art ----------- */
    leer(); breit(0, 15, 3); stueck(2, 0x1a, 5);
    re15_player_set_equipped_weapon(15);
    auf(0); tausch_modus(110);
    druck(RE15_PAD_BIT_DOWN);
    druck(RE15_PAD_BIT_SQUARE);
    CHECK(111, g_inv.slots[0].id == 16 && re15_inv_equipped_slot() == 0, "K10 gefuehrte Zelle 0 traegt jetzt 16; ist %02x, Zelle %d",
          g_inv.slots[0].id, re15_inv_equipped_slot());
    zu(112, 1);
    CHECK(113, re15_player_equipped_weapon() == 16, "K10 nach dem Schliessen ist die gefuehrte Waffe 16 (RE2 @0x8006bc70-98), ist %d",
          re15_player_equipped_weapon());

    if (s_fails) { printf("test_r35_werfer_kombi: %d FAILURES\n", s_fails); return 1; }
    printf("test_r35_werfer_kombi: OK\n");
    return 0;
}
