/*
 * RE1.5 Rebuilt — Runde 41 Spur S "spinnengore" (Kopf und Belege: include/re15_spinnengore_r41.h,
 * Dossier analysis/befunde_runde41/S_spinnengore.md).
 *
 * Alle RE1.5-Adressen STAGE2.BIN (Overlay @0x80100000), selbst disassembliert in BEIDEN Staenden (info/Re1.5 und
 * Version_40%, Ausgaben gleich). RE2-Adressen EMS25.BIN (CDEMD0.EMS kind 0x25, Slot-0-Record @0x80100000).
 */
#include "re15_spinnengore_r41.h"
#include "re15_teile_r15.h"
#include "re15_damage.h"      /* re15_enemy_fx19700 (FUN_80019700 mit a3-Versatz), re15_engine_rand8 (FUN_8001af20) */
#include <stdlib.h>
#include <string.h>

static int s_test_schalter = -1;

void re15_spinnengore_test_schalter(int an) { s_test_schalter = an; }

/* PORT-WAHL-Schalter wie re15_zgore_80prozent (zombiegore_r15.c): dieselbe Umgebungsvariable. Gelesen wird nur an
 * Treffer- und Todes-Ereignissen, nicht je Bild. */
int re15_spinnengore_an(void)
{
    if (s_test_schalter >= 0) return s_test_schalter;
    return getenv("RE15_KEIN_80PROZENT_GORE") ? 0 : 1;
}

/* Tabelle @0x80118f38 (24 Worte, `addiu at,at,-28872` @0x80113fd0, Index `lbu v0,5(v0)` @0x80113fc0):
 * Eintrag == 0x80113ff8 fuer 7,8,9,10,11,13,16,17,18,21, sonst 0x8011461c. */
int re15_spinnengore_platzwaffe(int w)
{
    switch (w) {
    case 7: case 8: case 9: case 10: case 11: case 13: case 16: case 17: case 18: case 21: return 1;
    default: return 0;
    }
}

/* Ein Teil gruen zerplatzen lassen (@0x80113cc8-cfc, @0x801140cc-10c, @0x801146bc-6f0):
 *   `lui t0,0x20` / `ori t0,t0,0x7f60` / `sw t0,104(v1)`        -> Farbwort +0x68 = 0x207f60
 *   `sh zero,148` / `sh a3(-50),150` / `sh zero,152` / `sh a2(3),154` / `sh zero,156` / `sh zero,158`
 *                                                               -> +0x94..+0x9e = {0,-50,0,3,0,0}
 *   `ori v0,v0,0x4a` / `sw v0,0(v1)`                             -> Wort |= 0x4a (0x40 eigene Matrix, 0x08 Scherben) */
void re15_spinnengore_teil_platzt(int slot, int k)
{
    re15_teil_t *t = re15_teile(slot);
    if (!t || k < 0 || k >= RE15_TEILE_MAX) return;
    t[k].col = 0x207f60u;
    memset(t[k].h94, 0, sizeof t[k].h94);
    t[k].h94[1] = (uint16_t)(int16_t)-50;
    t[k].h94[3] = 3;
    t[k].w |= 0x4a;
    re15_teile_markieren(slot);
}

/* Alle noch dranhaengenden Beine abwerfen — Schleife @0x8011419c-52c (Zwilling @0x80114778-b0c):
 *   +0x1e0 = 9; `addiu v0,v0,-1` / `beq v0,zero` -> i = 8..1
 *   A = pool + 344*i + 172 = Teil 2i+1, B = A + 172 = Teil 2i+2
 *   `andi v1,a0,0x41` / `bne v1,v0(1)` @0x80114218-21c    -> nur sichtbar und nicht schon abgeloest
 *   `ori v0,a0,0x1062` @0x80114220 (A) / @0x80114264 (B)    -> 0x1000 Lichtbezug, 0x40 Matrix, 0x20 Flug, 0x02
 *   `sb t2(20),289` (+0x75) / `sh t1(-128),302` (+0x82) / `sb a3(8),290` (+0x76)
 *   `sh zero,228` / `sh a2(-100),230` / `sh zero,232` (+0x38/+0x3a/+0x3c); fuer B dieselben Felder +344. */
int re15_spinnengore_beine_fliegen(int slot)
{
    re15_teil_t *t = re15_teile(slot);
    if (!t) return 0;
    int n = 0;
    for (int i = 8; i >= 1; i--) {
        re15_teil_t *a = &t[2 * i + 1], *b = &t[2 * i + 2];
        if ((a->w & 0x41u) != 1u) continue;
        re15_teil_t *p[2] = { a, b };
        for (int j = 0; j < 2; j++) {
            p[j]->w        = (uint16_t)(p[j]->w | 0x1062u);
            p[j]->grav     = 20;
            p[j]->spin     = -128;
            p[j]->spin_inc = 8;
            p[j]->v[0] = 0; p[j]->v[1] = -100; p[j]->v[2] = 0;
        }
        n++;
    }
    if (n) re15_teile_markieren(slot);
    return n;
}

/* Komplett-Platzer = 0x80113ff8 Phase 0 @0x8011403c (gleiche Folge im Krit-Zweig von 0x8011461c @0x801146b0):
 *   Blut 0x82000 an Teil 0 (`jal 0x80019700` @0x801140c4, a2 = +0x188 + 64, a3 = 0x80118f18 = Nullen)
 *   Teil 0 und Teil 19 (+3268) platzen (@0x801140cc-14c)
 *   Blut an Teil 0 (@0x80114164) und Teil 19 (`addiu a2,a2,3332` @0x8011418c)
 *   alle Beine fliegen (@0x8011419c-52c). SE 5 (@0x80114190) und Clip/Zustand bleiben beim Gehirn. */
void re15_spinnengore_platzen(re15_actor_t *e)
{
    static const int32_t null3[3] = { 0, 0, 0 };
    if (!e) return;
    int slot = (int)(e - g_actors);
    if (slot <= 0 || slot >= RE15_ACTOR_MAX) return;
    re15_enemy_fx19700(e, 0x00082000u, 0, null3);
    re15_spinnengore_teil_platzt(slot, 0);
    re15_spinnengore_teil_platzt(slot, 19);
    re15_enemy_fx19700(e, 0x00082000u, 0, null3);
    re15_enemy_fx19700(e, 0x00082000u, 19, null3);
    (void)re15_spinnengore_beine_fliegen(slot);
}

/* Beinpaar leg/leg+1 platzt, Blut zweimal an Teil leg+2 (a2 = pool + 172*leg + 408 @0x80113dc8 / @0x80113e20). */
void re15_spinnengore_bein_ab(re15_actor_t *e, int leg)
{
    static const int32_t null3[3] = { 0, 0, 0 };
    if (!e || leg < 3 || leg > 17) return;
    int slot = (int)(e - g_actors);
    if (slot <= 0 || slot >= RE15_ACTOR_MAX) return;
    re15_spinnengore_teil_platzt(slot, leg);
    re15_spinnengore_teil_platzt(slot, leg + 1);
    re15_enemy_fx19700(e, 0x00082000u, leg + 2, null3);
    re15_enemy_fx19700(e, 0x00082000u, leg + 2, null3);
}

/* RE2-Gehirn, HURT (nach dem Zeilen-Handler, jeden Takt):
 *   (1) jedes NEU abgetrennte RE2-Bein k (+0x220 |= 1<<k @0x80105c64 bzw. @0x80103A18; Teile 2k+3/2k+4
 *       @0x80105c44-48 `sll a0,a1,1` / `addiu a3,a0,3`) wird gezeichnet — PORT-WAHL: das RE1.5-Platzen statt der
 *       RE2-Drift (dunkel 0x2f2f2f, @0x80105c88-ce0);
 *   (2) Krit (+0x93 & 0x40, gesetzt @0x800123b4-b8) im frischen Takt, ohne RE2-Abtrennung: das RE1.5-Krit-Bein
 *       {3,..,17}[rng&7] (@0x80113b74-94, Tabelle @0x80118f28). Gefuehrt in der RE2-Beinmaske des Tiers (+0x220/+0x221)
 *       statt in der gemeinsamen RE1.5-Liste @0x80118f30 — PORT-WAHL "Maximum": die Liste gilt fuer ALLE Spinnen eines
 *       Raums, nach 8 Beinen fiele dort keines mehr. Die RE2-Untergrenze +0x221 < 3 (@0x80105c20) gilt hier nicht. */
void re15_spinnengore_re2_treffer(re15_actor_t *e, uint8_t beine_vor, int frisch)
{
    if (!e || !re15_spinnengore_an()) return;
    uint8_t neu = (uint8_t)(e->re2s_legs220 & (uint8_t)~beine_vor);
    for (int k = 0; k < 8; k++)
        if ((neu >> k) & 1u) re15_spinnengore_bein_ab(e, 2 * k + 3);
    if (neu || !frisch || !(e->hit_react & 0x40)) return;
    int j = (int)(re15_engine_rand8() & 7u);                 /* `jal 0x8001af20` / `andi v0,v0,0x7` @0x80113b80-90 */
    if ((e->re2s_legs220 >> j) & 1u) return;                 /* Bein schon ab (RE1.5: Listentreffer @0x80113c14-1c) */
    e->re2s_legs220 = (uint8_t)(e->re2s_legs220 | (1u << j));
    if (e->re2s_legn221 > 0) e->re2s_legn221--;
    re15_spinnengore_bein_ab(e, 2 * j + 3);
}

/* RE2-Gehirn, DEATH (frischer Takt +0x6 == 0): Platzer, wenn
 *   - die RE1.5-Waffentabelle @0x80118f38 auf 0x80113ff8 zeigt (IMMER-Platzer), oder
 *   - Krit (+0x93 & 0x40; RE1.5 0x8011461c @0x801146b0), oder
 *   - die RE2-Zeile ihren eigenen Hinterleib-Platzer faehrt: FUN_80104890 aus 0x801044D0 (Zeilen 5,6,7,8,17,
 *     `jal 0x80104890` @0x80104588) und ueber Zeile 9 (@0x80104A2C), Saeure-Zeile 11 wirft Teil 19 (@0x80104bcc-bf4).
 * Sonst das RE1.5-Todesblut an Teil 0 (0x8011461c @0x80114654). */
void re15_spinnengore_re2_tod(re15_actor_t *e, int weapon_id, int row)
{
    static const int32_t null3[3] = { 0, 0, 0 };
    if (!e || !re15_spinnengore_an()) return;
    int re2_platzt = (row == 5 || row == 6 || row == 7 || row == 8 || row == 9 || row == 11 || row == 17);
    if (re15_spinnengore_platzwaffe(weapon_id) || (e->hit_react & 0x40) || re2_platzt)
        re15_spinnengore_platzen(e);
    else
        re15_enemy_fx19700(e, 0x00082000u, 0, null3);
}
