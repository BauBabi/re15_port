/* =============================================================================
 * DAUERFEUER-KADENZ-PIN (Ingram M10 = Waffen-Id 12; identische Mechanik 14/19)
 * =============================================================================
 * Byte-true-Quelle: eigene Disassemblierung 2026-09-12, Dossiers in
 * analysis/waffen_fsm_2026-09-12/ (fsm-sub1/sub2, entlade-ingram, SPEC.md).
 *
 * Gepinnt wird:
 *  (A) Hold-Clips der Dauerfeuer-Klasse: 9/12/15 = 9+3*hoch+6*tief
 *      (@0x800342bc-e8) - NICHT 8/10/12 wie die Standard-Waffen.
 *  (B) Die Schuss-Kadenz der Feuerschleife. Der Entlade-Handler laeuft in
 *      Sub 2 JEDEN Frame (jalr @0x800345ec); darin:
 *        SCHUSS  (acae9 & 4) == 0     @0x800349bc
 *        MUENDUNG acae9 % 3 == 0      @0x8003483c
 *        HUELSE  (s8)acaf2 % 7 == 0   @0x80034948 (signiert!)
 *      Der Feuer-Clip von PL00W0C hat 9 Bilder -> Schuesse auf den Bildern
 *      0,1,2,3,8 = FUENF je 9-Frame-Zyklus; der Clip LOOPT (f314-Wrap
 *      ignoriert @0x80034664).
 *  (C) Abzug loslassen -> Abkling-Clip 8/11/14 (@0x80034794) mit dem EINZIGEN
 *      Frame-Reset der Schleife (@0x8003477c); Clip-Ende -> Hold (sh 1
 *      @0x800347e4).
 *  (D) Der u8-Ueberlauf des Huelsen-Zaehlers acaf2 ist byte-true SIGNIERT
 *      dividiert: (int8_t)128 = -128, -128 % 7 = -2 != 0 -> nach Takt 126
 *      faellt die naechste Huelse erst auf Takt 130 ((int8_t)130 = -126,
 *      -126 % 7 == 0) - die 7er-Phase verschiebt sich um 4.
 * ============================================================================= */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_inventory.h"

extern re15_actor_t g_actors[];

extern int  re15_player_aim_clip(void);
extern int  re15_player_aim_ready(void);
extern void re15_player_fire_start(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_player_autofire_active(void);
extern int  re15_player_autofire_tick(int held, int *schuss, int *muendung, int *rauch, int *huelse);
extern void re15_player_autofire_empty(void);

static int fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fail = 1; } } while (0)

int main(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->hp = 100;
    g_scd.player_mode = 0;
    g_scd.message_display_frames = 0;

    if (re15_inv_find_item(12) < 0) (void)re15_inv_grant(12, 99);
    re15_player_set_equipped_weapon(12);            /* Ingram M10 -> 0x80034014 */
    re15_player_tick(NULL, 0);                      /* Aim-FSM -> NONE */
    pl->motion = 200; pl->anim_frame = 0;
    re15_player_set_aim_clip_len(9);                /* Mock: W0C-Feuerclip = 9 Bilder */

    /* ---- (A) RAISE -> HOLD 9; Elevation 12/15 ------------------------------ */
    for (int f = 0; f < 20 && !re15_player_aim_ready(); f++)
        re15_player_tick(NULL, RE15_PAD_BIT_R1);
    CHECK(re15_player_aim_ready(), "(A) nach 20 Ticks R1 nicht aim-ready");
    CHECK(re15_player_aim_clip() == 9,
          "(A) Dauerfeuer-Hold = Clip 9 (@0x800342bc-e8), ist %d", re15_player_aim_clip());
    re15_player_tick(NULL, RE15_PAD_BIT_R1 | RE15_PAD_BIT_UP);
    CHECK(re15_player_aim_clip() == 12, "(A) Hold HOCH = Clip 12, ist %d", re15_player_aim_clip());
    re15_player_tick(NULL, RE15_PAD_BIT_R1 | RE15_PAD_BIT_DOWN);
    CHECK(re15_player_aim_clip() == 15, "(A) Hold TIEF = Clip 15, ist %d", re15_player_aim_clip());
    re15_player_tick(NULL, RE15_PAD_BIT_R1);
    CHECK(re15_player_aim_clip() == 9,  "(A) Hold MITTE = Clip 9, ist %d", re15_player_aim_clip());
    /* der Elevationswechsel hat das Frueh-Exit-Gate gesetzt - fuer den Feuer-
     * Einstieg unten verbrauchen (game_step taete dasselbe) */
    { extern int re15_player_elev_gate_consume(void); (void)re15_player_elev_gate_consume(); }

    /* ---- (B) Feuerschleife: 27 Ticks, Muster exakt -------------------------- */
    re15_player_fire_start();
    CHECK(re15_player_aim_clip() == 7, "(B) Feuer-Clip MITTE = 7 (@0x80034588-a8), ist %d",
          re15_player_aim_clip());
    CHECK(re15_player_autofire_active(), "(B) autofire_active nach fire_start");
    {
        /* Reihenfolge wie im Spiel: fire_start lief NACH player_tick dieses
         * Ticks -> der Entlade-Tick desselben Frames liest Bild 0; danach je
         * Tick erst Advance (player_tick), dann Entlade-Tick. */
        int schuesse = 0, muendungen = 0, huelsen = 0;
        int folge_ok = 1;
        for (int t = 0; t < 27; t++) {
            int d = 0, m = 0, r = 0, h = 0;
            if (t > 0) re15_player_tick(NULL, RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE);
            CHECK(re15_player_autofire_tick(1, &d, &m, &r, &h) == 1,
                  "(B) Tick %d: Schleife unerwartet beendet", t);
            int f9 = t % 9;                        /* Bild im 9er-Zyklus */
            int soll_d = ((f9 & 4) == 0);          /* 0,1,2,3,8 */
            int soll_m = (f9 % 3 == 0);            /* 0,3,6 */
            int soll_h = (((int8_t)t % 7) == 0);   /* Takt == Tick hier */
            if (d != soll_d || m != soll_m || h != soll_h) {
                folge_ok = 0;
                fprintf(stderr, "FAIL: (B) Tick %d (Bild %d): schuss=%d/%d muendung=%d/%d huelse=%d/%d\n",
                        t, f9, d, soll_d, m, soll_m, h, soll_h);
                fail = 1;
            }
            schuesse += d; muendungen += m; huelsen += h;
        }
        CHECK(folge_ok && schuesse == 15 && muendungen == 9 && huelsen == 4,
              "(B) Summen 27 Ticks: schuesse=%d/15 muendungen=%d/9 huelsen=%d/4",
              schuesse, muendungen, huelsen);
        CHECK(re15_player_aim_clip() == 7, "(B) Clip loopt auf 7, ist %d", re15_player_aim_clip());
    }

    /* ---- (C) Abzug loslassen -> Abklingen 8 -> Hold 9 ----------------------- */
    {
        int d, m, r, h;
        CHECK(re15_player_autofire_tick(0, &d, &m, &r, &h) == 0,
              "(C) Loslassen muss die Schleife beenden");
        CHECK(re15_player_aim_clip() == 8,
              "(C) Abkling-Clip MITTE = 8 (@0x80034794), ist %d", re15_player_aim_clip());
        CHECK(pl->anim_frame == 0, "(C) EINZIGER Frame-Reset (@0x8003477c), ist %d",
              (int)pl->anim_frame);
        for (int t = 0; t < 12; t++) re15_player_tick(NULL, RE15_PAD_BIT_R1);
        CHECK(re15_player_aim_clip() == 9 && re15_player_aim_ready(),
              "(C) Abkling-Ende -> Hold 9 + aim-ready (sh 1 @0x800347e4), clip=%d ready=%d",
              re15_player_aim_clip(), re15_player_aim_ready());
    }

    /* ---- (D) s8-Ueberlauf des Huelsen-Takts --------------------------------- */
    {
        re15_player_fire_start();
        int letzte = -1, bei_126 = 0, in_127_129 = 0, bei_130 = 0;
        for (int t = 0; t < 132; t++) {
            int d = 0, m = 0, r = 0, h = 0;
            if (t > 0) re15_player_tick(NULL, RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE);
            re15_player_autofire_tick(1, &d, &m, &r, &h);
            if (h) letzte = t;
            if (t == 126 && h) bei_126 = 1;
            if (t >= 127 && t <= 129 && h) in_127_129 = 1;
            if (t == 130 && h) bei_130 = 1;
        }
        CHECK(bei_126 && !in_127_129 && bei_130,
              "(D) s8-Ueberlauf: Huelse bei 126 (%d), KEINE bei 127-129 (%d), wieder bei 130 (%d)"
              " - (int8_t)128=-128, -128%%7=-2 (@0x80034948 signierte Division)",
              bei_126, in_127_129, bei_130);
        (void)letzte;
    }

    if (fail) { fprintf(stderr, "test_autofire_pin: ROT\n"); return 1; }
    printf("test_autofire_pin: alle Pins gruen (Hold 9/12/15, Kadenz 5/9, s8-Huelsentakt)\n");
    return 0;
}
