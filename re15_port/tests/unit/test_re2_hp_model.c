/**
 * @file test_re2_hp_model.c
 * @brief PIN — das VOLLSTAENDIGE RE2-HP-/Schadensmodell im RE2-Modus, und der Beweis, dass
 *        der RE1.5-Pfad byte-identisch bleibt.
 *
 * NUTZER-BEFUND (echter Spieltest, RE2-KI-Modus, Dinner Room): "Die Zombies stecken viel zu
 * viel ein. Viel mehr als im Original." URSACHE: der RE2-Modus fuhr nur das VERHALTEN; HP und
 * Schaden blieben RE1.5 (Pistole 5 Schaden auf ~95 HP = ~20 Treffer). Das Modell ist jetzt
 * komplett portiert — die Belege stehen an den Tabellen in re15_damage.c, hier die Kurzform:
 *
 *   SCHADEN: RE2-Applier FUN_800470C0 (info/re2leon/PSX.EXE)
 *     80047218 lbu v0,8(s1)      ; Typ
 *     8004722c lw  a1,27272(at)  ; a1 = *(u32*)(0x800A6A88 + typ*4)
 *     8004723c addiu v0,v0,-20   ; Zeile = 20*(w-1),  w = RE2-Item-Zeile
 *     8004724c lw  v1,0(a1)      ; Wort0 = 3x 10 Bit
 *     80047254 srlv v1,v1,v0     ; Schiebeweite = 10*Bracket (Bracket 0 = direkter Einschlag)
 *     8004725c andi v1,v1,0x3ff  ; 80047260 subu v0,v0,v1 ; 80047268 sh v0,342(s1)
 *   HP: die vier HP-Tabellen je EM-Modul haengen an DAT_800CFB74 Bit 0x40/0x20. Bit 0x40 wird
 *     im Boot-Systeminit gesetzt (`DAT_800cfb74 |= 0x40` @0x8002B4B4) und von keinem der 9
 *     Schreiber je geloescht (der einzige maskierende, `&= 0x10008D9` @0x8002C698, laesst 0x40
 *     stehen); 0x20 setzt niemand. Damit ist genau EIN Zweig je Modul ausgeliefert:
 *       Zombie EMOVL10_S0.BIN @0x80100708 -> 0x8010C670, +15 wenn Gegnerzahl < 4 (@0x801007FC),
 *                              Typ 0x11 fest 250 (@0x801008C8-CC)
 *       Hund   EMD0G_MOD0.BIN @0x801001C4 -> 0x801053B0 + (rand&3)
 *       Kraehe EMOVL21_S0.BIN @0x80100324 -> 10 fest
 *       Spinne EMS25.BIN      @0x80100298 -> 0x80106334 + (rand&3)
 *
 * Der Test prueft DREI Konfigurationen an denselben Aktoren:
 *   (A) RE1.5                 — MUSS byte-identisch zur veroeffentlichten RE1.5-Zeile bleiben
 *   (B) RE2 + Modell AUS      — der Negativ-Test: wieder die RE1.5-Zahlen
 *   (C) RE2 + Modell AN       — die RE2-Zahlen
 *
 * 2026-08-20 — KIND 0x11 IST SCHARF, DER SCHALTER IST WEG. `RE15_RE2_ZOMBIE11_250` /
 * re15_re2_kind11_250() existieren nicht mehr; die 250 gelten unbedingt. Belegkette (voll in
 * re15_damage.c ueber re15_re2_init_hp): beide Spiele lesen den Typ aus ENTITY+0x8 (RE1.5
 * @0x801007d8, RE2 @0x80100894) — es gibt keine Uebersetzungsschicht; der Port indiziert den
 * RE2-Asset-TOC @0x8009ADF4 mit genau diesem Typ (re2_ems.c `((kind-0x10)*4+rec)*2`) und laedt
 * fuer 0x11 eine EIGENE EM011-TIM (313 von 66592 Bytes gleich zu EM010) — RE2-EM011 ist Brad
 * Vickers (BioModels.h:186); und die zweite Haelfte derselben bne-Verzweigung (+0x21A |= 0x8000,
 * @0x801008D0-D4) war in enemy_ai_re2_zombie.c immer schon ungegatet scharf.
 * Gemessen (probe_re2_hp_model, ROOM1140, echter game_step-Weg), Pistolentreffer bis zum Tod:
 * 0x10 -> 5, 0x16 -> 6, 0x11 -> 16. Abschnitt 6 haelt beide Gegenrichtungen fest.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

/* ---- die ERWARTUNGEN ------------------------------------------------------------------- */
/* RE1.5, byte-true @0x8006e0d0 + typ*0x58 (unveraendert aus re15_damage.c uebernommen) */
static const int E15_ZOMBIE[22] = { 0,6,24,5,5,15,15,200,40,100,200,100,10,100,10,100,200,100,400,20,0,100 };
static const int E15_DOG[22]    = { 0,5,20,10,10,24,24,200,40,100,100,200,30,100,10,100,100,200,400,20,0,100 };
static const int E15_CROW[22]   = { 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,100 };
static const int E15_SPIDER[22] = { 0,6,24,10,10,24,24,200,36,100,50,200,10,100,10,100,50,200,400,20,0,100 };
/* RE2, Zone 0 von 0x800A6A88[typ] + 20*(Zeile-1) (Zeilen-Karte re2z_row_from_weapon) */
static const int E2_ZOMBIE[22]  = { 3,3,3,16,16,16,16,900,200,200,200,200,4,300,15,200,200,200,900,8,16,3 };
static const int E2_ZOMBIE16[22]= { 3,3,3,11,11,11,11,900,60,80,200,80,3,80,12,80,200,80,900,6,11,3 };
static const int E2_DOG[22]     = { 10,10,10,18,18,18,18,200,59,300,300,300,10,300,12,300,300,300,300,18,18,10 };
static const int E2_CROW[22]    = { 15,15,15,15,15,15,15,200,60,60,60,60,15,120,15,60,60,60,300,30,15,15 };
static const int E2_SPIDER[22]  = { 15,15,15,17,17,17,17,130,55,60,90,130,10,130,10,60,90,130,200,18,17,15 };

/* Waffen, die den unbegrenzten Schuss-Streifen benutzen (Tester-Tabelle @0x8006E548). */
static const uint8_t GUN_STRIP[22] = { 1,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,1,0,1 };

/* Die vier RE2-INIT-HP-Tabellen des ausgelieferten Zweigs (selbst gedumpt, s. Kopf). */
static const int HP_ZOMBIE[16] = { 80, 94, 128, 75, 60, 95, 58, 75, 50, 83, 79, 66, 80, 65, 82, 65 };
static const int HP_DOG[16]    = { 129, 95, 95, 95, 129, 80, 95, 95, 80, 95, 69, 80, 69, 95, 69, 80 };
static const int HP_SPIDER[16] = { 109,109,109,109,129,109,109,129,109,109,109,129,109,99,109,109 };

static uint8_t *s_buf = NULL;
static long     s_sz  = 0;

static int load_room(void)
{
    FILE *f = fopen(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END); s_sz = ftell(f); fseek(f, 0, SEEK_SET);
    s_buf = (uint8_t *)malloc((size_t)s_sz);
    if (!s_buf || fread(s_buf, 1, (size_t)s_sz, f) != (size_t)s_sz) { fclose(f); return 0; }
    fclose(f);
    return 1;
}

static int bringup(int flavor, int model_on)
{
    re15_rdt_t rdt;
    if (re15_rdt_parse(s_buf, (size_t)s_sz, &rdt) != 0) return -1;
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    re15_re2_damage_model_set(model_on);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    if (rdt.main_scd)   scd_thread_start(0, rdt.main_scd);
    if (rdt.sub_scd[0]) scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->x = 0; pl->y = 0; pl->z = 0;
    for (int i = 0; i < 30; i++) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type < 0x40) return s;
    return -1;
}

/* EIN Ziel scharfmachen. re2_hp_stamped = 1: der HP-Stempel darf die gesetzten Test-HP nicht
 * ueberschreiben — hier wird der SCHADEN gemessen, die HP-Seite hat ihren eigenen Block. */
static void arm(int slot, uint8_t type, int hp, int dist)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    e->active = 1; e->type = type;
    re15_enemy_apply_hitbox(e, type);
    e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->hp = (int16_t)hp; e->hit_react = 0; e->hit_stun = 0;
    e->grid_id = (uint8_t)(e->grid_id & 0x7f);   /* aufrecht -> LEVEL-Band @0x801015F4-FC */
    e->aim_band = 2;                             /* Kraehe: LEVEL (@0x80112560-C8)          */
    e->anim_flags = 0;
    e->re2_hp_stamped = 1;
    pl->x = e->x - dist; pl->z = e->z; pl->y = e->y; pl->rot_y = 0; pl->hit_react = 0;
}

/* Eine ganze Zeile pruefen: 22 Waffen gegen die erwarteten Schadenswerte. */
static void check_row(const char *tag, int flavor, int model_on, uint8_t type, const int *exp)
{
    int bad = 0;
    for (int w = 0; w < 22; w++) {
        int slot = bringup(flavor, model_on);
        if (slot < 0) { CHECK(0, "%s: kein Gegner in ROOM1140", tag); return; }
        /* 4000 statt 3000 fuer die Schuss-Streifen-Waffen: der Schrot-Crit haengt an
         * `best_dist < 3000` (@0x800124FC-51C) und best_dist kommt aus der BIOS-Naeherung
         * SquareRoot0 — bei nominal 3000 liefert sie 2999, der Crit feuerte also und
         * verdeckte den Schadenswert. 4000 ist sicher ausserhalb; die Schuss-Streifen sind
         * reichweiten-UNbegrenzt (re15_gun_wedge_inside, Beleg in re15_damage.c). */
        arm(slot, type, 2000, GUN_STRIP[w] ? 4000 : 700);
        re15_player_set_equipped_weapon(w);
        int fired = re15_player_weapon_fire(w);
        re15_actor_t *e = &g_actors[slot];
        if (!fired) { CHECK(0, "%s typ=0x%02X w%d: Schuss kam nicht an", tag, type, w); bad++; continue; }
        /* RE1.5-Crit (@0x800124FC-51C, in BEIDEN Modi aktiv — er sitzt im RE1.5-Hitscan, nicht
         * im Schadensmodell): Waffe 7 toetet Typen < 0x20 sofort (Waffe 8 nur unter 3000). */
        int crit = (w == 7 && type < 0x20);
        int want = crit ? -1 : (2000 - exp[w]);
        if (e->hp != (int16_t)want) {
            CHECK(0, "%s typ=0x%02X w%d: hp %d, erwartet %d (Schaden %d%s)",
                  tag, type, w, e->hp, want, exp[w], crit ? ", Crit" : "");
            bad++;
        }
    }
    if (!bad) printf("  %-22s typ=0x%02X: 22/22 Waffen exakt\n", tag, type);
}

int main(void)
{
    if (!load_room()) { printf("SKIP: ROOM1140.RDT fehlt\n"); return 0; }

    /* ================= 1) RE1.5 — REGRESSIONSWACHE (byte-identisch) ======================= */
    printf("=== 1) RE1.5-Pfad unveraendert (Zeilen @0x8006e0d0 + typ*0x58) ===\n");
    check_row("RE1.5", RE15_AI_FLAVOR_RE15, 1, 0x10, E15_ZOMBIE);
    check_row("RE1.5", RE15_AI_FLAVOR_RE15, 1, 0x11, E15_ZOMBIE);
    check_row("RE1.5", RE15_AI_FLAVOR_RE15, 1, 0x16, E15_ZOMBIE);
    check_row("RE1.5", RE15_AI_FLAVOR_RE15, 1, 0x20, E15_DOG);
    check_row("RE1.5", RE15_AI_FLAVOR_RE15, 1, 0x21, E15_CROW);
    check_row("RE1.5", RE15_AI_FLAVOR_RE15, 1, 0x25, E15_SPIDER);

    /* ================= 2) RE2 + Modell AN — die RE2-Zeilen ================================ */
    printf("\n=== 2) RE2-Modell AN: Zone 0 aus 0x800A6A88[typ] ===\n");
    check_row("RE2 AN", RE15_AI_FLAVOR_RE2, 1, 0x10, E2_ZOMBIE);    /* 0x800A412C */
    check_row("RE2 AN", RE15_AI_FLAVOR_RE2, 1, 0x11, E2_ZOMBIE);    /* 0x800A412C */
    check_row("RE2 AN", RE15_AI_FLAVOR_RE2, 1, 0x16, E2_ZOMBIE16);  /* 0x800A42A8 */
    check_row("RE2 AN", RE15_AI_FLAVOR_RE2, 1, 0x20, E2_DOG);       /* 0x800A4424 */
    check_row("RE2 AN", RE15_AI_FLAVOR_RE2, 1, 0x21, E2_CROW);      /* 0x800A45A0 */
    check_row("RE2 AN", RE15_AI_FLAVOR_RE2, 1, 0x25, E2_SPIDER);    /* 0x800A4B90 */

    /* ================= 3) NEGATIV-TEST: Modell AUS -> die RE1.5-Zahlen sind zurueck ======= */
    printf("\n=== 3) NEGATIV-TEST: RE2-Modus mit Modell AUS = alte Zahlen ===\n");
    check_row("RE2 AUS", RE15_AI_FLAVOR_RE2, 0, 0x10, E15_ZOMBIE);
    check_row("RE2 AUS", RE15_AI_FLAVOR_RE2, 0, 0x16, E15_ZOMBIE);
    check_row("RE2 AUS", RE15_AI_FLAVOR_RE2, 0, 0x20, E15_DOG);
    check_row("RE2 AUS", RE15_AI_FLAVOR_RE2, 0, 0x21, E15_CROW);
    check_row("RE2 AUS", RE15_AI_FLAVOR_RE2, 0, 0x25, E15_SPIDER);
    /* ⚠ 0x26 ist BEWUSST ausgenommen: ihre RE2-Zeile ist kein Balance-Thema, sondern der Fix
     * gegen den permanenten Einfrierer (49de51f3) und bleibt auch bei Modell AUS aktiv. */

    /* ================= 4) DIE HP-SEITE ==================================================== */
    printf("\n=== 4) RE2-INIT-HP je Typ (re15_re2_init_hp) ===\n");
    {
        int slot = bringup(RE15_AI_FLAVOR_RE2, 1);
        CHECK(slot > 0, "kein Gegner fuer den HP-Block");
        if (slot > 0) {
            re15_actor_t *e = &g_actors[slot];
            struct { uint8_t type; const int *tbl; int fixed; const char *name; } cases[] = {
                { 0x10, HP_ZOMBIE, -1,  "Zombie 0x10 @0x8010C670" },
                { 0x12, HP_ZOMBIE, -1,  "Zombie 0x12 @0x8010C670" },
                { 0x16, HP_ZOMBIE, -1,  "Zombie 0x16 @0x8010C670" },
                { 0x11, NULL,     250,  "Zombie 0x11 fest @0x801008C8-CC (EM011 Brad)" },
                { 0x21, NULL,      10,  "Kraehe 0x21 fest @0x80100324" },
                { 0x20, HP_DOG,    -1,  "Hund   0x20 @0x801053B0" },
                { 0x25, HP_SPIDER, -1,  "Spinne 0x25 @0x80106334" },
            };
            for (unsigned c = 0; c < sizeof cases / sizeof cases[0]; c++) {
                e->type = cases[c].type;
                int lo = 9999, hi = -9999, bad = 0;
                for (int i = 0; i < 400; i++) {
                    int hp = re15_re2_init_hp(e);
                    if (hp < lo) lo = hp;
                    if (hp > hi) hi = hp;
                    if (cases[c].fixed >= 0) { if (hp != cases[c].fixed) bad++; continue; }
                    /* Wuerfel-Tabellen: Zombie exakt aus der Tabelle (+0/+15),
                     * Hund/Spinne Tabelle + (rand & 3). */
                    int ok = 0, slack = (cases[c].tbl == HP_ZOMBIE) ? 0 : 3;
                    for (int k = 0; k < 16 && !ok; k++)
                        for (int d = 0; d <= slack && !ok; d++)
                            if (hp == cases[c].tbl[k] + d ||
                                (slack == 0 && hp == cases[c].tbl[k] + 15)) ok = 1;
                    if (!ok) bad++;
                }
                printf("  %-30s %3d..%3d  (%d Ausreisser von 400)\n", cases[c].name, lo, hi, bad);
                CHECK(bad == 0, "HP-Zug fuer %s liefert Werte ausserhalb der Tabelle", cases[c].name);
            }
            /* ⛔ KIND 0x11 = 250, OHNE SCHALTER (2026-08-20). Der frueher hier gepinnte Hebel
             * RE15_RE2_ZOMBIE11_250 / re15_re2_kind11_250() ist ERSATZLOS entfallen — dass
             * RE1.5-Typ 0x11 == RE2-kind 0x11 ist, ist belegt (ENTITY+0x8 in BEIDEN Spielen:
             * RE1.5 @0x801007d8 / RE2 @0x80100894; der Port indiziert den RE2-Asset-TOC
             * @0x8009ADF4 mit genau diesem Typ und laedt fuer 0x11 eine eigene EM011-Textur;
             * RE2-EM011 = Brad Vickers, BioModels.h:186). Volle Herleitung in re15_damage.c.
             * Der Test pinnt jetzt DIE UNBEDINGTHEIT: keine Umgebung, kein Setter, kein Wurf
             * kann 0x11 von 250 wegbringen — und NUR 0x11 bekommt sie. */
            e->type = 0x11;
            {
                int all250 = 1;
                for (int i = 0; i < 400; i++) if (re15_re2_init_hp(e) != 250) all250 = 0;
                CHECK(all250, "Kind 0x11 muss IMMER 250 liefern (@0x801008BC/C8/CC) — "
                              "unbedingt, ohne Schalter");
                printf("  Zombie 0x11 fest                     250      (400/400, kein Schalter)\n");
            }
            {   /* GEGENPROBE: 250 ist EXAKT ein Sonderfall — kein anderes Kind faellt darauf.
                 * `bne v1,v0,0x801008d8` @0x801008BC ueberspringt jedes andere Kind. */
                static const uint8_t others[] = { 0x10, 0x12, 0x13, 0x16, 0x18 };
                int any250 = 0;
                for (unsigned k = 0; k < sizeof others / sizeof others[0]; k++) {
                    e->type = others[k];
                    for (int i = 0; i < 400; i++)
                        if (re15_re2_init_hp(e) == 250) any250 = 1;
                }
                CHECK(!any250, "NUR Kind 0x11 darf 250 bekommen — ein anderer Zombie-Typ "
                               "hat den Sonderfall getroffen");
                printf("  0x10/0x12/0x13/0x16/0x18             nie 250  (Gegenprobe @0x801008BC)\n");
                e->type = 0x11;
            }

            /* Baby 0x26: KEIN Modell hier — HP 1 / -1 kommt byte-true aus seinem eigenen Brain. */
            e->type = 0x26;
            CHECK(re15_re2_init_hp(e) < 0, "0x26 darf KEIN HP-Modell aus re15_damage.c bekommen "
                  "(HP 1 @0x801000F8 / -1 @0x80100204 gehoeren dem Baby-Brain)");
        }
    }

    /* ================= 5) DER STEMPEL: RE1.5 unberuehrt, RE2 genau einmal ================= */
    printf("\n=== 5) re15_re2_hp_sync: No-op in RE1.5, genau EIN Stempel pro Spawn in RE2 ===\n");
    {
        int slot = bringup(RE15_AI_FLAVOR_RE15, 1);
        if (slot > 0) {
            re15_actor_t *e = &g_actors[slot];
            e->hp = 4242; e->re2_hp_stamped = 0;
            re15_re2_hp_sync();
            CHECK(e->hp == 4242, "RE1.5: re15_re2_hp_sync hat HP veraendert (%d)", e->hp);
            CHECK(e->re2_hp_stamped == 0, "RE1.5: re15_re2_hp_sync hat das Port-Feld angefasst");
            printf("  RE1.5: hp bleibt %d, Stempel bleibt %d\n", e->hp, e->re2_hp_stamped);
        }
        slot = bringup(RE15_AI_FLAVOR_RE2, 1);
        if (slot > 0) {
            re15_actor_t *e = &g_actors[slot];
            /* bringup() fahrt die KI ueber re15_enemy_ai_run_all (nicht ueber game_step), also
             * steht der Stempel hier noch aus — genau der Zustand direkt nach dem INIT-Tick. */
            CHECK(e->state != 0, "RE2: der INIT ist nicht gelaufen (state 0)");
            int16_t hp_before = e->hp;
            re15_re2_hp_sync();
            CHECK(e->re2_hp_stamped == 1, "RE2: der Stempel wurde nach dem INIT nicht gesetzt");
            CHECK(e->hp != hp_before || e->type == 0x21,
                  "RE2: der Stempel hat die RE1.5-HP nicht ersetzt (hp blieb %d)", e->hp);
            int16_t hp_after_init = e->hp;
            for (int i = 0; i < 5; i++) re15_re2_hp_sync();
            CHECK(e->hp == hp_after_init, "RE2: wiederholtes re15_re2_hp_sync wuerfelt neu "
                  "(%d -> %d) — der Stempel muss GENAU EINMAL pro Spawn feuern",
                  hp_after_init, e->hp);
            printf("  RE2: typ=0x%02X hp=%d, 5x sync ohne Aenderung\n", e->type, e->hp);
            /* Modell AUS -> kein Stempel */
            slot = bringup(RE15_AI_FLAVOR_RE2, 0);
            if (slot > 0) {
                re15_actor_t *e2 = &g_actors[slot];
                re15_re2_hp_sync();
                CHECK(e2->re2_hp_stamped == 0, "RE2 mit Modell AUS: es wurde trotzdem gestempelt");
                printf("  RE2 Modell AUS: Stempel=%d hp=%d (RE1.5-Zug)\n",
                       e2->re2_hp_stamped, e2->hp);
            }
        }
    }

    /* ================= 6) TYP 0x11 KOMPLETT: RE1.5-Wache + Negativ-Test ==================== */
    /* Der Sonderfall ist die einzige Stelle des Modells, an der die Identitaet RE1.5-Typ ==
     * RE2-kind kampfrelevant wird — er bekommt deshalb seinen eigenen Block mit BEIDEN
     * Gegenrichtungen. */
    printf("\n=== 6) Typ 0x11: RE1.5-Pfad byte-identisch, Modell AUS = kein 250 ===\n");
    {
        /* RE1.5-eigene HP-Zeile fuer Typ 0x11: `lbu v1,8(a1)` @0x801007d8 -> `sll v1,v1,5`
         * @0x801007e0 -> Tabelle 0x8011f034 + 0x11*0x20 = @0x8011f254, Spalte (rng&0xf)*2
         * (STAGE1.BIN selbst gedumpt). Kein Wert dieser Zeile ist 250 — RE1.5 kennt keinen
         * zaehen Zombie-Typ. */
        static const int HP15_Z11[16] = { 71,85,103,73,87,105,75,107,89,77,93,79,95,81,98,83 };

        {   /* Statische Gegenprobe: 250 kommt in der RE1.5-Zeile ueberhaupt nicht vor. */
            int has250 = 0;
            for (int k = 0; k < 16; k++) if (HP15_Z11[k] == 250) has250 = 1;
            CHECK(!has250, "RE1.5-Zeile @0x8011f254 enthaelt 250 — dann waere die Gegenprobe hin");
        }

        /* (a) RE1.5-WACHE am ECHTEN 0x11-Spawn von ROOM1140: seine HP muessen aus der
         *     RE1.5-Zeile @0x8011f254 kommen, der RE2-Stempel muss ein No-op sein. */
        int slot = bringup(RE15_AI_FLAVOR_RE15, 1);
        int z11 = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x11) { z11 = s; break; }
        CHECK(z11 > 0, "ROOM1140 hat keinen Typ-0x11-Spawn — die 0x11-Wache misst nichts");
        if (z11 > 0) {
            re15_actor_t *e = &g_actors[z11];
            int in_row = 0;
            for (int k = 0; k < 16; k++) if (e->hp == HP15_Z11[k]) in_row = 1;
            int16_t hp_before = e->hp;
            re15_re2_hp_sync();
            CHECK(e->hp == hp_before, "RE1.5/0x11: der RE2-Stempel hat die HP angefasst (%d -> %d)",
                  hp_before, e->hp);
            CHECK(e->re2_hp_stamped == 0, "RE1.5/0x11: der RE2-Stempel hat das Port-Feld gesetzt");
            CHECK(e->hp != 250, "RE1.5/0x11: 250 im RE1.5-Pfad — der Sonderfall leckt");
            CHECK(in_row, "RE1.5/0x11: hp=%d steht NICHT in der Zeile @0x8011f254", hp_before);
            printf("  RE1.5 0x11 (Slot %d): hp=%d aus Zeile @0x8011f254, Stempel=%d, kein 250\n",
                   z11, hp_before, e->re2_hp_stamped);
        }
        (void)slot;

        /* (b) NEGATIV-TEST: RE2-Modus mit RE15_RE2_DMG_MODEL=0 -> gar kein Stempel, also auch
         *     keine 250. Das ist der EINZIGE verbliebene Hebel; einen typ-lokalen gibt es nicht
         *     mehr. */
        bringup(RE15_AI_FLAVOR_RE2, 0);
        z11 = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x11) { z11 = s; break; }
        if (z11 > 0) {
            re15_actor_t *e = &g_actors[z11];
            int16_t hp_before = e->hp;
            re15_re2_hp_sync();
            CHECK(e->re2_hp_stamped == 0 && e->hp == hp_before,
                  "RE2/Modell AUS/0x11: es wurde gestempelt (Stempel=%d, hp %d -> %d)",
                  e->re2_hp_stamped, hp_before, e->hp);
            CHECK(e->hp != 250, "RE2/Modell AUS/0x11: 250 trotz abgeschaltetem Modell");
            printf("  RE2 Modell AUS 0x11: hp bleibt %d, Stempel=%d\n", e->hp, e->re2_hp_stamped);
        }

        /* (c) POSITIV ueber den Stempel-Weg: RE2 + Modell AN -> genau 250 auf dem echten Spawn. */
        bringup(RE15_AI_FLAVOR_RE2, 1);
        z11 = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x11) { z11 = s; break; }
        if (z11 > 0) {
            re15_actor_t *e = &g_actors[z11];
            CHECK(e->state != 0, "RE2/0x11: der INIT ist nicht gelaufen (state 0)");
            re15_re2_hp_sync();
            CHECK(e->hp == 250, "RE2/Modell AN/0x11: der Stempel liefert %d statt 250", e->hp);
            CHECK(e->re2_hp_stamped == 1, "RE2/Modell AN/0x11: kein Stempel gesetzt");
            printf("  RE2 Modell AN  0x11: hp=%d (Stempel=%d)\n", e->hp, e->re2_hp_stamped);
        }
    }

    printf("\n%s\n", fails ? "FAILED" : "OK");
    return fails ? 1 : 0;
}
