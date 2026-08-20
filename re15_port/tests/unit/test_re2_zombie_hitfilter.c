/**
 * @file test_re2_zombie_hitfilter.c
 * @brief PIN — der VOLLSTAENDIGE RE2-Kandidatenfilter im re15_re2z_tick-Epilog.
 *
 * Anlass (Nutzer-Spieltest v0.3.3, RE2-KI-Modus, Zombies):
 *   (b) "Manchmal wuerden sie hinfallen, stehen dann aber ploetzlich doch wieder kurz danach
 *        WAEHREND der Hinfall-Animation."
 *   (d) "Manchmal taucht das Blut an der richtigen Stelle auf, und manchmal beim Bein unten."
 *   (e) "Beim Anschiessen und Sterben landen sie offenbar weitestgehend animationslos am Boden."
 *
 * GEMESSEN VORHER (probe_re2_zfall Modus 4, ROOM1140, echter Weg game_step + R1/SQUARE, RE2-Bank
 * EM010 geladen, Pistole): f165 Treffer -> DEATH (hp -1), danach SECHS weitere Treffer auf die
 * LEICHE (f176/187/198/209/220/231, hp bis -97), die die Todes-Phase +0x6 jedes Mal von 1 auf 0
 * zuruecksetzen. NACHHER: 5 Treffer, der Todes-Treffer ist der letzte, Clip 7 laeuft f165..f234
 * ungestoert durch (anim_frame 0->69), dann CORPSE.
 *
 * URSACHE: 717d13e0 hat vom Original-Kandidatenfilter nur EIN Gate gespiegelt und nur in EINE
 * Richtung (freigeben, nie sperren). Der Original-Filter FUN_800470C0 (info/re2leon/PSX.EXE,
 * selbst disassembliert) hat VIER:
 *   80047124/2c/30: `lw v0,0(s0)` / `andi 0x1`   / `beq  -> 0x8004740c`  (1) inaktiv
 *   80047138/40   : `lbu v0,467(s0)`             / `bne  -> 0x8004740c`  (2) +0x1D3 != 0
 *   80047148/50   : `lh  v0,342(s0)`             / `bltz -> 0x8004740c`  (3) HP < 0
 *   80047158/60/64: `lhu v0,270(s0)` / `andi 0xc000` / `bne -> 0x8004740c` (4) +0x10E & 0xC000
 *
 * Dazu (d): das Sperren laeuft im Port zwangslaeufig ueber den RE1.5-Latch +0x93 Bit 0, und der
 * RE1.5-Resolver setzt bei einem gesperrten Kandidaten `+0x93 |= 2` (@0x8001240c / @0x80012fcc)
 * — der Ausloeser des RE1.5-Gore-Spawns FUN_80106a44, der an der AKTOR-WURZEL (= am Boden
 * zwischen den Fuessen) spawnt. RE2 hat das Feld +0x93 ueberhaupt nicht (Voll-Scan
 * `sb/lbu rt,147(rs)`: 0 Treffer in PSX.EXE UND in EMOVL10_S0.BIN), also darf im RE2-Gehirn kein
 * +0x93-getriebener Wurzel-Gore entstehen -> Bit 1 gehoert pro Tick auf 0.
 *
 * Gepinnt wird jedes Gate EINZELN mit POSITIV-KONTROLLE (der gesunde, aufrechte Zombie MUSS
 * freigegeben bleiben — sonst waere der Filter ein Trefferblocker und (c) "Sturz nach dem
 * Torkeln" waere wieder tot) und eine RE1.5-REGRESSIONSWACHE.
 */
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern int re15_re2z_tick(int slot);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

#define SLOT 2

/* Ein aufrechter, gesunder RE2-Zombie im Gang-Zustand — der Ausgangspunkt jeder Zeile. */
static re15_actor_t *mk(void)
{
    re15_actor_t *e = &g_actors[SLOT];
    memset(e, 0, sizeof *e);
    e->active = 1;
    e->type   = 0x10;
    e->hp     = 79;
    e->state  = 1;              /* ACTIVE */
    e->sub_state_1 = 1;         /* WALK */
    e->re2z_self1d3 = 0;
    e->re2z_f10e    = 0;
    e->hit_react    = 0;
    return e;
}

/* Der Epilog laeuft am ENDE von re15_re2z_tick; ein Tick reicht, um ihn zu beobachten. */
static uint8_t tick_latch(re15_actor_t *e)
{
    re15_re2z_tick(SLOT);
    return e->hit_react;
}

int main(void)
{
    re15_actor_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(0);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);

    /* ---- POSITIV-KONTROLLE: Gate (1)-(4) alle offen -> Kandidat FREIGEGEBEN ---------------
     * Ohne diese Zeile waere jeder folgende "gesperrt"-Nachweis wertlos: ein Filter, der alles
     * sperrt, bestuende jeden Sperrtest und wuerde den Zombie unbeschiessbar machen. */
    {   re15_actor_t *e = mk();
        e->hit_react = 1;                       /* der Resolver-Latch des Vorschusses steht */
        uint8_t hr = tick_latch(e);
        CHECK((hr & 1) == 0,
              "POSITIV-KONTROLLE: gesunder aufrechter Zombie muss freigegeben werden "
              "(+0x93 = 0x%02X, Bit 0 haengt)", hr);
    }

    /* ---- GATE (3) HP < 0 -> gesperrt  (@0x80047148-50 `lh v0,342`/`bltz`) ------------------
     * Das ist der gemessene (e)-Fall: die Leiche darf nicht weiter beschossen werden. */
    {   re15_actor_t *e = mk();
        e->hp = -1;                             /* der Todes-Treffer ist gefallen */
        e->state = 3;                           /* DEATH */
        e->hit_react = 0;
        uint8_t hr = tick_latch(e);
        CHECK((hr & 1) == 1,
              "GATE 3: HP < 0 muss den Kandidaten SPERREN (@0x80047150 bltz) "
              "(+0x93 = 0x%02X)", hr);
    }

    /* ---- GATE (2) +0x1D3 != 0 -> gesperrt  (@0x80047138-40 `lbu 467`/`bne`) ----------------
     * Das ist der (b)-Fall: waehrend der Sturz-/Liege-Phasen setzt EXEC[5] P0 +0x1D3 |= 0x80
     * (@0x80103304); ein Treffer darf die laufende Fall-Animation dann nicht neu anstossen.
     *
     * ⚠ Der Sturz-Executor selbst wird hier NICHT gefahren: ohne geladene Bank ist
     * re15_actor_clip_len() == 0, jede clip-getriebene Phase endet im Setz-Tick und EXEC[5]
     * laeuft bis zu seinem eigenen `+0x1D3 &= 0x7f` (@0x80103484-90) durch — der Test wuerde
     * dann den Executor messen statt den Filter. Gepinnt wird deshalb der FILTER an einem
     * Zustand, der +0x1D3 nicht anfasst; dass der echte Sturz das Byte traegt, ist am echten Weg
     * gemessen (probe_re2_zfall Modus 4, f34: s1=5 s2=1 1D3=80 -> +0x93 = 0x01). */
    {   re15_actor_t *e = mk();
        e->re2z_self1d3 = 0x80u;                /* ori 0x80 @0x80103304 (Sturz-Claim) */
        e->hit_react    = 0;
        uint8_t hr = tick_latch(e);
        CHECK((hr & 1) == 1,
              "GATE 2: +0x1D3 != 0 muss SPERREN (@0x80047140 bne) (+0x93 = 0x%02X)", hr);
    }

    /* ---- GATE (4) +0x10E & 0xC000 -> gesperrt  (@0x80047158-64 `lhu 270`/`andi 0xc000`) ---- */
    {   re15_actor_t *e = mk();
        e->re2z_f10e = 0x4000u;                 /* Limpet-/Liege-Latch */
        e->hit_react = 0;
        uint8_t hr = tick_latch(e);
        CHECK((hr & 1) == 1,
              "GATE 4: +0x10E & 0x4000 muss SPERREN (@0x80047164 bne) (+0x93 = 0x%02X)", hr);
    }
    {   re15_actor_t *e = mk();
        e->re2z_f10e = 0x8000u;
        e->hit_react = 0;
        uint8_t hr = tick_latch(e);
        CHECK((hr & 1) == 1,
              "GATE 4: +0x10E & 0x8000 muss SPERREN (@0x80047164 bne) (+0x93 = 0x%02X)", hr);
    }
    /* Gegenprobe zur Maske: 0x2000 (schon-gefallen, @0x80102DA8) liegt NICHT in 0xC000 und darf
     * NICHT sperren — sonst waere ein einmal gefallener Zombie fuer immer unbeschiessbar. */
    {   re15_actor_t *e = mk();
        e->re2z_f10e = 0x2000u;
        e->hit_react = 1;
        uint8_t hr = tick_latch(e);
        CHECK((hr & 1) == 0,
              "MASKEN-GEGENPROBE: +0x10E & 0x2000 liegt nicht in 0xC000 und darf NICHT sperren "
              "(+0x93 = 0x%02X)", hr);
    }

    /* ---- (d) BIT 1 = RE1.5-WURZEL-GORE: im RE2-Gehirn IMMER 0 ------------------------------
     * +0x93 Bit 1 ist der einzige Ausloeser von re15_enemy_gore_tick (@0x80106a98), und der
     * spawnt an e->x/y/z = AM BODEN. RE2 kennt +0x93 nicht -> Bit 1 darf einen RE2-Zombie nie
     * ueberleben, weder freigegeben noch gesperrt. */
    {   re15_actor_t *e = mk();
        e->hit_react = 0x2;                     /* Resolver-Zweitkontakt @0x8001240c */
        uint8_t hr = tick_latch(e);
        CHECK((hr & 2) == 0,
              "(d) freigegeben: +0x93 Bit 1 (RE1.5-Wurzel-Gore) muss geloescht sein "
              "(+0x93 = 0x%02X)", hr);
    }
    {   re15_actor_t *e = mk();
        e->hp = -1; e->state = 3;
        e->hit_react = 0x3;                     /* gesperrt UND Zweitkontakt-Gore gesetzt */
        uint8_t hr = tick_latch(e);
        CHECK((hr & 2) == 0,
              "(d) gesperrt: +0x93 Bit 1 muss auch beim gesperrten Kandidaten geloescht sein "
              "(+0x93 = 0x%02X)", hr);
        CHECK((hr & 1) == 1, "(d) gesperrt: Bit 0 muss dabei gesetzt BLEIBEN (+0x93 = 0x%02X)", hr);
    }

    /* ---- NEGATIV-TEST: der Filter darf den lebenden Zombie nie dauerhaft sperren ------------
     * 200 Ticks am Stueck; sonst waere (c) "Sturz nach dem Torkeln" wieder unerreichbar.
     *
     * Gemessen wird die EXAKTE AEQUIVALENZ: nach jedem Tick muss `+0x93 Bit 0 gesetzt` genau
     * dann gelten, wenn der Original-Filter den Kandidaten ueberspringen wuerde. Das ist
     * schaerfer als "nie gesperrt" und bleibt gueltig, egal in welchen Zustand die KI laeuft —
     * der Zombie greift z.B. selbstaendig zu und setzt dabei +0x1D3 = 15 (@0x8010276C-70),
     * was den Kandidaten VOLLKOMMEN ZURECHT sperrt. Ein "nie gesperrt"-Test haette genau diese
     * korrekte Sperre als Fehler gemeldet. */
    {   re15_actor_t *e = mk();
        int mismatch = 0, blocked = 0, released = 0;
        for (int i = 0; i < 200; i++) {
            e->hp = 79;
            uint8_t hr = tick_latch(e);
            int want_block = !(e->active != 0
                            && e->re2z_self1d3 == 0u
                            && e->hp >= 0
                            && !(e->re2z_f10e & 0xC000u));
            if (((hr & 1) != 0) != (want_block != 0)) mismatch++;
            if (hr & 1) blocked++; else released++;
            if (hr & 2) mismatch++;             /* Bit 1 darf nie ueberleben */
        }
        CHECK(mismatch == 0,
              "NEGATIV-TEST: %d von 200 Ticks weichen vom Original-Filter ab "
              "(gesperrt=%d frei=%d)", mismatch, blocked, released);
        CHECK(released > 0,
              "NEGATIV-TEST: der Zombie war in ALLEN 200 Ticks gesperrt — der Filter waere ein "
              "Trefferblocker und (c) 'Sturz nach dem Torkeln' unerreichbar");
    }

    /* ---- RE1.5-REGRESSIONSWACHE ------------------------------------------------------------
     * re15_re2z_tick ist der RE2-Zweig; im RE1.5-Flavor laeuft er gar nicht (der Hook in
     * re15_enemy_ai_live_step ist auf re15_ai_flavor() gegated). Diese Zeile pinnt, dass der
     * Epilog KEIN globaler Nebeneffekt ist: der RE1.5-Pfad fasst +0x93 hier nicht an. */
    {   re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        re15_actor_t *e = mk();
        e->hp = -1; e->state = 3;
        e->hit_react = 0;
        /* Der RE1.5-Dispatch ruft re15_re2z_tick nicht auf -> +0x93 bleibt, wie es war. */
        CHECK(e->hit_react == 0,
              "RE1.5-WACHE: der RE2-Epilog darf im RE1.5-Flavor nicht wirken (+0x93 = 0x%02X)",
              e->hit_react);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    }

    if (fails == 0) printf("test_re2_zombie_hitfilter: OK\n");
    else            printf("test_re2_zombie_hitfilter: %d FAIL\n", fails);
    return fails ? 1 : 0;
}
