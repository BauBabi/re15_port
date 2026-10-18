/*
 * RE1.5 Rebuilt — Runde 39 Spur E "gegner". Belege: include/re15_gegner_r39.h (Kopf) und
 * analysis/befunde_runde39/E_gegner.md. Haken (je 1 Zeile): enemy_ai_common.c (Gorilla-INIT/HURT/Tick-Ende,
 * run_all), enemy_ai_re2_dog.c (Tore + Boden-Biss), enemy_ai_re2_zombie.c (Kriecher-Wurzel), platform/pc/main.c
 * (NPC-Schatten).
 */
#include "re15_gegner_r39.h"
#include "re15_affen.h"       /* re15_affen_treffer_zaehlen (Runde 35: 3 Treffer -> Vergeltungs-Sprung) */
#include "re15_audio.h"       /* re15_audio_room_se */
#include "re15_damage.h"      /* re15_ai_arc_test */
#include "re15_enemy_ai.h"    /* re15_player_hit_flinch_active / re15_player_knockdown_active */
#include "re15_room.h"        /* g_current_room_id */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include "re15_engine.h"      /* g_engine.frame_count — nur fuer die Messzeile (wie hundeschatten_1190.c) */
#endif

static unsigned r39_bild(void)
{
#ifdef RE15_PLATFORM_PC
    return (unsigned)g_engine.frame_count;
#else
    return 0u;
#endif
}

void re15_r39_affen_blut(re15_actor_t *e);   /* enemy_ai_common.c: re15_maggot_gore (vec @0x80121388) + Se 3 */

static int s_an = 1;
void re15_r39_gegner_enable(int an) { s_an = an ? 1 : 0; }

/* ---------------------------------------------------------------- Mess-Schiene -------------------------- */
void re15_r39_log(const char *fmt, ...)
{
    static int an = -1; static FILE *lf = NULL;
    if (an < 0) { an = (getenv("RE15_R39_LOG") != NULL); if (an) lf = fopen("gegner_r39.log", "w"); }
    if (!lf) return;
    va_list ap; va_start(ap, fmt); vfprintf(lf, fmt, ap); va_end(ap);
    fputc('\n', lf); fflush(lf);
}

/* ================================================================ (1) Gorilla 0x27 (alle Raeume) ========= */
/* Nachbesserung 2 (Nutzer-Nachtrag 3, AUFTRAG.md 2026-10-18: "Zu 2. ja, wir machen das Gorilla Verhalten generell so."):
 * kein Raum-Tor mehr — die Regel haengt allein am Typ 0x27 (Wurzel 0x80116db8, Dispatch-Platz 0x80072c48 @0x8011e900-0c).
 * Zensus: Sce_em_set 0x27 nur in ROOM11C0 (Leon) und ROOM1141 (Elza), Kopf re15_gegner_r39.h (1). */
typedef struct { uint8_t ok, sub1, sub2, sub3; } r39_merk_t;
static r39_merk_t s_merk[RE15_ACTOR_MAX];
static int16_t    s_hp_ref[RE15_ACTOR_MAX];     /* HP beim letzten Flinch (bzw. INIT) = Bezug des Akkus */
static uint8_t    s_ref_ok[RE15_ACTOR_MAX];     /* Bezug gesetzt (INIT oder erstes Tick-Ende) */

static int r39_slot(const re15_actor_t *e)
{
    int s = (int)(e - g_actors);
    return (e && s >= 0 && s < RE15_ACTOR_MAX) ? s : -1;
}

int re15_r39_affen_regel(const re15_actor_t *e)
{
    return s_an && e && e->type == RE15_R39_AFFEN_TYP;
}

int16_t re15_r39_affen_hp_init(const re15_actor_t *e, int16_t tabelle)
{
    int s = r39_slot(e);
    int16_t hp = tabelle;
    if (re15_r39_affen_regel(e)) hp = (int16_t)(tabelle * RE15_R39_AFFEN_HP_FAKTOR);   /* 360 */
    if (s >= 0) { s_hp_ref[s] = hp; s_ref_ok[s] = 1; memset(&s_merk[s], 0, sizeof s_merk[s]); }
    re15_r39_log("affen init slot=%d raum=%04x typ=%02x hp=%d (tabelle %d @0x8011f514, regel %d)", s, g_current_room_id,
                 e ? e->type : 0, (int)hp, (int)tabelle, re15_r39_affen_regel(e));
    return hp;
}

void re15_r39_affen_merken(const re15_actor_t *e)
{
    int s = r39_slot(e);
    if (s < 0) return;
    /* Bezug des Akkus (PORT-WAHL der Buchfuehrung): ein Gorilla verliert HP nur; HP ueber dem Bezug = der Platz traegt
     * einen Aktor, der den INIT dieses Hakens nicht durchlief (Sonde/Riegel mit gesetzter HP) -> Bezug neu. */
    if (!s_ref_ok[s] || e->hp > s_hp_ref[s]) { s_hp_ref[s] = e->hp; s_ref_ok[s] = 1; }
    if (e->state != 1) return;
    s_merk[s].ok = 1; s_merk[s].sub1 = e->sub_state_1; s_merk[s].sub2 = e->sub_state_2; s_merk[s].sub3 = e->sub_state_3;
}

int re15_r39_affen_akku(const re15_actor_t *e)
{
    int s = r39_slot(e);
    return (s >= 0) ? (int)s_hp_ref[s] - (int)e->hp : 0;
}

/* HURT-Eintritt (Zustand 2, +0x7 = 0 vom Resolver @0x80012428): unter der Schwelle kein Flinch.
 * Nicht fuer Krit-Treffer (+0x93 & 0x40, Magnum/nahe Schrotflinte, @0x800123b4-b8): die schreiben in Luft-/
 * Pin-Zustaenden Zustand 2 selbst (@0x8011a8c8-fc) und tragen ohnehin >= 38 Energie. */
int re15_r39_affen_zucken_unterdruecken(re15_actor_t *e)
{
    int s = r39_slot(e);
    if (s < 0 || !re15_r39_affen_regel(e)) return 0;
    if (e->state != 2 || e->sub_state_3 != 0) return 0;
    if (e->hp < 0 || (e->hit_react & 0x40u)) return 0;
    int akku = (int)s_hp_ref[s] - (int)e->hp;
    if (!s_merk[s].ok || akku >= RE15_R39_ZUCK_SCHWELLE) {
        re15_r39_log("affen treffer F%u slot=%d raum=%04x hp=%d akku=%d -> FLINCH (Schwelle %d)", r39_bild(), s,
                     g_current_room_id, (int)e->hp, akku, RE15_R39_ZUCK_SCHWELLE);
        s_hp_ref[s] = e->hp;                               /* Flinch wie bisher; Akku neu ab hier */
        return 0;
    }
    /* Treffer ohne Zucken: Gehirn mit den Unterzustaenden des letzten Ticks weiter (+0x4 = 1 wie der Exit
     * @0x8011b188), +0x93 = 0 (@0x8011b178), Blut + Se 3 (@0x8011b0bc-138), Runde-35-Zaehler wie ein Flinch. */
    e->state = 1;
    e->sub_state_1 = s_merk[s].sub1; e->sub_state_2 = s_merk[s].sub2; e->sub_state_3 = s_merk[s].sub3;
    e->hit_react = 0;
    re15_affen_treffer_zaehlen(e);
    re15_r39_affen_blut(e);
    re15_r39_log("affen treffer F%u slot=%d raum=%04x hp=%d akku=%d -> OHNE ZUCKEN (zurueck in 1/%d/%d/%d)", r39_bild(), s,
                 g_current_room_id, (int)e->hp, akku, e->sub_state_1, e->sub_state_2, e->sub_state_3);
    return 1;
}

/* ================================================================ (2) Hund: Ein-Angreifer-Riegel ======== */
static int s_hund_riegel = 0;   /* Besitzer-Merker: der Riegel kam vom Boden-Biss (Zombie-/Arm-Riegel loeschen sich selbst) */

int re15_r39_hund_riegel(const re15_actor_t *pl)
{
    return s_an && pl && (pl->re2z_self1d3 & 0x80u) != 0;        /* lbu 467(s1) / andi 0x80 @0x80104e5c-64 */
}

void re15_r39_hund_riegel_setzen(re15_actor_t *pl)
{
    if (!s_an || !pl) return;
    pl->re2z_self1d3 |= 0x80u;                                   /* ori 0x80 @0x801051f8 / sb @0x80105200 */
    s_hund_riegel = 1;
    re15_r39_log("hund riegel GESETZT F%u hp=%d", r39_bild(), (int)pl->hp);
}

int re15_r39_hund_riegel_vom_hund(void) { return s_hund_riegel; }

void re15_r39_hund_riegel_tick(void)
{
    if (!s_hund_riegel) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Ende der Spieler-Reaktion = @0x8003f8b0-b4 (Zustand 1 + +0x1D3 := 0). Port: der NORMAL-Prolog loescht
     * +0x93 Bit 1 (@0x80031964) erst, wenn weder Flinch (cmd 2, 22 Bilder) noch Niederschlag (cmd 4/5) laeuft. */
    if ((pl->hit_react & 1u) || re15_player_hit_flinch_active() || re15_player_knockdown_active()) return;
    pl->re2z_self1d3 &= 0x7Fu;
    s_hund_riegel = 0;
    re15_r39_log("hund riegel FREI F%u hp=%d", r39_bild(), (int)pl->hp);
}

/* ================================================================ (3) Kriecher-Wecker (RE1.5 Block B) ==== */
int re15_r39_kriecher_weckt(const re15_actor_t *e, const re15_actor_t *pl)
{
    if (!s_an || !e || !pl) return 0;
    if (e->ai_dist >= RE15_R39_KRIECH_WECK_DIST) return 0;                         /* sltiu 0xbb8 @0x80104718 */
    if (re15_ai_arc_test(e, pl->x, pl->z, RE15_R39_KRIECH_WECK_ARC) != 0) return 0;  /* @0x8010472C-34 */
    re15_r39_log("kriecher weckt slot=%d typ=%02x dist=%u -> Lokomotion (Wort 1 @0x80104738/48)",
                 r39_slot(e), e->type, e->ai_dist);
    return 1;
}

/* ================================================================ (4) Schatten auf der Boden-Referenz ===== */
/* Original (STAGE1.BIN, selbst disassembliert; Nachbesserung 1 nach Abnahme 0 M1): die Gorilla-Wurzel FUN_80116db8
 * erreicht den Schatten-Schwanz `lw v0,g_entity` @0x80116f24 / `lh a1,442(v0)` @0x80116f30 / `jal 0x8001b064`
 * @0x80116f34 / `addiu a0,a0,176` @0x80116f38 auf JEDEM Pfad: das Pause-Bit (`bne` @0x80116dd8) und der Root-Skip
 * `lbu v0,9(v1)` @0x80116dec / `andi v0,v0,0x20` @0x80116df4 / `bne v0,zero,0x80116e88` @0x80116df8 springen nur
 * ueber den Gehirn-Dispatch (`jalr` @0x80116e20, Tabelle 0x801213c8) und Push-out/Kollision (@0x80116e28-e84) auf
 * 0x80116e88; von dort geht es ueber grid&0x40 (@0x80116e9c-ea0 -> 0x80116f20) bzw. die +0x1d8-Formel (@0x80116ea8-
 * f1c) in den Schwanz @0x80116f20. Kleiner Affe 0x29 ebenso (STAGE3.BIN FUN_80110b10: `bne` @0x80110b40 -> 0x80110bd0,
 * Schwanz @0x80110c68-80 mit `lh a1,442(v0)` @0x80110c78 / `jal 0x8001b064` @0x80110c7c).
 * Die Szenen-Gorillas (grid 0x30: geparkt y=-20000 aus dem RDT @0x01784, im Wagen y=-2500 per Member_set @0x0198E)
 * geben ihren Quad also ebenfalls aus: Hoehe +0x1ba = -3600 (Sce_em_set pc[4]=2), Ecken +0xbc/+0xbe = 1700/1800
 * (INIT @0x801171e8/f0; der INIT laeuft im Spawn-Bild, weil Sce_em_set das Bit 0x20 vor dem Wurzelaufruf loescht:
 * `andi v0,v1,0xdf` @0x8004256c / `sb v0,9(s0)` @0x80042570 / `jalr` @0x8004259c, zurueck `or v0,v0,s4` @0x80042604
 * / `sb` @0x80042608). Original-RAM (Runde-35-Savestates orig_scene/r3, Pool 0x800acc2c Stride 0x1f4): t=6.11 Zustand
 * 1/0/0/0, grid 0x30, y=-20000, +0x1ba=-3600, +0xbc/+0xbe=1700/1800; t=34.89 (Cut 12) y=-2500, +0x1ba=-3600.
 * Ob der Quad ins Bild kommt, entscheidet FUN_8001b064 selbst: `lw a1,DAT_800ac790` @0x8001b180 (Region-Viereck des
 * aktiven Cuts, FUN_80014324 -> @0x80021c0c) / `jal 0x80014368` @0x8001b224 / `beq v0,zero,0x8001b364` @0x8001b22c =
 * Punkt-im-Viereck ueber (x,z) der Entity (FUN_80014368 liest `lw v0,8(a0)` = z @0x80014370, `lw v0,0(a0)` = x
 * @0x80014384, die Hoehe nie). Original-RAM: geparkt (-1220,-21568)/(-554,-25423) in den Cuts 0/1/3/4/13 = 0 (kein
 * Quad); Cut 12 (-1220,-21568) = 1, (-554,-25423) = 0; Cut 5 beide = 1. Der Port cullt den NPC-Schatten mit demselben
 * Viereck (main.c npc_region_culled, re15_rdt_get_region_quad) und zeichnet die zugelassenen Quads: Cut 12 als Band
 * (120..214, 107..130) in der Klappenoeffnung (schwarzer Innenraum, Gorilla, Heckklappe davor), Cut 5 bei x = -692..-22
 * links ausserhalb des Bildes - wie im Original nichts davon sichtbar (E_gegner.md Nachbesserung 1). Deshalb KEIN
 * Sonderweg fuer grid&0x20; der Root-Skip-Haken des Zwischenstands c3823fa4 war am Byte falsch und ist entfernt. */
int re15_r39_schatten_y(const re15_actor_t *npc, int32_t *y)
{
    if (!s_an || !npc || !y) return 0;
    if (npc->type != 0x27u && npc->type != 0x29u) return 0;
    int32_t alt = *y;
    *y = (int32_t)npc->dog_floor_y;    /* +0x1ba: `lh a1,442(v0)` @0x80116f30 (0x27) / @0x80110c78 (0x29), auf jedem Pfad */
    if (alt != *y)
        re15_r39_log("schatten F%u slot=%d typ=%02x grid=%02x st=%d/%d koerper_y=%d boden_y=%d quad_y=%d",
                     r39_bild(), r39_slot(npc), npc->type, npc->grid_id, npc->state, npc->sub_state_1,
                     (int)npc->y, (int)npc->dog_floor_y, (int)*y);
    return 1;
}
