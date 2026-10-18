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
#include "re15_game_step.h"   /* re15_schritt_station_hole (Mess-Schiene Stationen, Nachbesserung 3) */
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
    if (an < 0) { an = (getenv("RE15_R39_GEGNER_LOG")   /* Runde 39 Integration: eigener Name, RE15_R39_LOG ist der Dateiname der Spur F (zustand_r39.c) */ != NULL); if (an) lf = fopen("gegner_r39.log", "w"); }
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

/* ---------------------------------------------------------------- Messhaken (nur Messlaeufe, Nachbesserung 2) --
 * RE15_R39_MESS_SPIELER_HP=<n> (env-gegatet wie RE15_R38F_GORILLAS_TOT; KEIN Spielverhalten, ohne Env nie aktiv):
 * am Kopf jedes KI-Bildes (re15_r39_hund_riegel_tick = Kopf von re15_enemy_ai_run_all) die Spieler-HP auf mindestens
 * n heben; jeder aufgefuellte Abzug steht mit Bildnummer in gegner_r39.log. Zweck: ein STEHENDER Skript-Schuetze
 * (RE15_INPUT_SCRIPT) haelt den ganzen Gorilla-Kampf ROOM1141 mit 2 x 360 Energie durch, damit der Ablauf Tod ->
 * Kill-Flags -> sub01 -> Johns Abgang an der exe messbar ist (ohne Haken stirbt die stehende Elza nach ~1850
 * Kampfbildern; ein Spieler weicht aus). n >= 50 haelt auch den Luft-Finisher fern: sein Commit verlangt player.hp < 50
 * (`lh v0,0(a2)` player.hp @0x80118cec / `slti v0,v0,50` @0x80118cf4 / `beq` @0x80118cf8). Die uebrigen Gorilla-
 * Abzuege: Biss `addiu v0,v0,-6` @0x80118468, Heavy `addiu v0,v0,-12` @0x801187bc — zwei Gorillas in einem Bild
 * hoechstens 24, mit n = 100 bleibt die HP also ueber 50. */
static void r39_mess_spieler_hp(void)
{
    static int n = -2;
    if (n == -2) { const char *e = getenv("RE15_R39_MESS_SPIELER_HP"); n = (e && *e) ? atoi(e) : -1;
                   if (n > 0) re15_r39_log("messhaken RE15_R39_MESS_SPIELER_HP=%d aktiv (nur Messlauf)", n); }
    if (n <= 0) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!pl->active || pl->hp < 0 || pl->hp >= n) return;
    re15_r39_log("messhaken F%u spieler hp %d -> %d (Abzug %d aufgefuellt)", r39_bild(), (int)pl->hp, n, n - (int)pl->hp);
    pl->hp = (int16_t)n;
}

/* ---------------------------------------------------------------- Mess-Schiene Stationen (Nachbesserung 3) ---
 * RE15_R39_MESS_STATIONEN=1 (zusaetzlich zu RE15_R39_GEGNER_LOG; KEIN Spielverhalten): am Kopf jedes KI-Bildes die
 * Spielerlage an den Stationen des VORBILDES (game_step_common.c, Runde 27): OPFER = nach der Opfer-Platzierung
 * (re15_player_victim_tick), SCHUB = nach dem Koerper-Schub, KLEMME = nach der Wandklemme, ENDE = Bildende; dazu die
 * Opfer-Lage (Clip/Bild/Anker/Blick). Nur solange der Gorilla-Opfer-Haken den Spieler besitzt (und ein Bild danach).
 * Zweck: belegen, WELCHER Schreiber Elza im 1141-Wurf ueber die Wandmitte setzt (Befund E (b)). */
static void r39_mess_stationen(void)
{
    static int an = -2; static int vorher = 0;
    if (an == -2) { const char *e = getenv("RE15_R39_MESS_STATIONEN"); an = (e && *e && *e != '0') ? 1 : 0; }
    if (an <= 0) return;
    int jetzt = re15_player_victim_gorilla();
    if (!jetzt && !vorher) return;
    vorher = jetzt;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t ox = 0, oz = 0, sx = 0, sz = 0, kx = 0, kz = 0, ex = 0, ez = 0;
    int o = re15_schritt_station_hole(RE15_SCHRITT_OPFER, &ox, &oz);
    int sc = re15_schritt_station_hole(RE15_SCHRITT_SCHUB, &sx, &sz);
    int k = re15_schritt_station_hole(RE15_SCHRITT_KLEMME, &kx, &kz);
    int en = re15_schritt_station_hole(RE15_SCHRITT_ENDE, &ex, &ez);
    re15_r39_log("stationen F%u (Vorbild) opfer=(%d,%d)%s schub=(%d,%d)%s klemme=(%d,%d)%s ende=(%d,%d)%s mo=%d bild=%d "
                 "anker=(%d,%d) rot=%d gorilla_opfer=%d", r39_bild(), (int)ox, (int)oz, o ? "" : "?", (int)sx, (int)sz,
                 sc ? "" : "?", (int)kx, (int)kz, k ? "" : "?", (int)ex, (int)ez, en ? "" : "?", (int)pl->motion,
                 (int)pl->anim_frame, (int)pl->anchor_x, (int)pl->anchor_z, (int)pl->rot_y, jetzt);
}

void re15_r39_hund_riegel_tick(void)
{
    r39_mess_spieler_hp();   /* Messhaken RE15_R39_MESS_SPIELER_HP (nur mit Env), s. oben */
    r39_mess_stationen();    /* Mess-Schiene RE15_R39_MESS_STATIONEN (nur mit Env), s. oben */
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

/* ================================================================ (5) Querungs-Wache (Nachbesserung 3) ===== */
/* Befund E (Nachbesserung 2, E_gegner.md): die RE1.5-Wandklemme FUN_8003b0a4 -> Rechteck-Ausstoss FUN_8003bca8 nimmt
 * die NAEHERE der beiden um den Radius aufgeblasenen Kanten (`slt v0,-a0,t1` / `beq` / `addu t1,a0,zero`
 * @0x8003bcec-cfc; Port re15_collision.c push_rect, `if (-pxm < penX) penX = pxm`). Steht ein Aktor nach Platzierung/
 * Stoss jenseits der Mitte einer soliden Rechteckzelle, stellt sie ihn an die AUSSENkante: (a) Gorilla G3 ROOM1141
 * Westwand (Koerper-Stoss FUN_8002aec4, pen 2829 -> x -9803, Klemme -> -12218), (b) Elza im Gorilla-Wurf Suedwand
 * (Opfer-Platzierung ad68 @0x8011c294 -> z -24973, Klemme -> -26068; Mess-Schiene Stationen, Dossier Nachbesserung 3).
 * RE2 RETAIL hat genau das behoben (Beta -> Retail): die Rechteck-Routine FUN_8004c900 (Tabelle @0x800a737c[0],
 * `lw v0,29564(at)` @0x8004c434 / `jalr v0` @0x8004c448 in FUN_8004c1bc) entscheidet die Seite nach der VORBILD-Lage
 * +0x118/+0x11a (`lh v0,280(a3)` @0x8004c930 / `lh v0,282(a3)` @0x8004c94c; geschrieben nach jeder Klemme
 * `sh v0,280(s0)` / `sh v1,282(s0)` @0x800356f4-f8 in FUN_8003567c = Spieler UND Gegner, Zombie-Wurzel `jal 0x8003567c`
 * @0x80100638 EMZ0.BIN): lag die Vorbild-Lage in X ausserhalb der aufgeblasenen Zelle (`sltu`/`xori`/`sll`
 * @0x8004c940-48, Randfaelle -1 / t2+1 @0x8004c968-7c), kommt x auf die Seite, von der der Aktor kam: Mitte
 * cell.x + w/2 (@0x8004c9f8) -/+ (r + w/2 + 1) (@0x8004c9e0-f0) je nach Vorzeichen von x - Vorbild (`bltz`
 * @0x8004c9f4, @0x8004c9fc-ca08); Z ebenso (@0x8004c94c-64, @0x8004c984-a8, @0x8004ca18-50). Nur wenn die Vorbild-
 * Lage in BEIDEN Achsen innerhalb lag, rechnet RE2 die naehere Kante wie RE1.5 (FUN_8004c858 @0x8004c9b8 ->
 * FUN_8004ca68). FORM (kleinste belegte): die RE1.5-Klemme bleibt Byte fuer Byte; NUR wenn ihr Ergebnis eine solide
 * Rechteckzelle durchquert (Strecke Vorbild -> Ergebnis schneidet das Innere, Vorbild ausserhalb), gilt fuer diese Zelle
 * die RE2-Regel. Bleibt danach noch eine Querung (zwei Zellen, Ecke), gilt die Vorbild-Lage - die Form der Runde-38-T-
 * Invariante (re15_r38_griff_durch_wand) und des RE1.5-Blocks `*lx = prevx` (push_rect code 0). */
#include "re15_collision.h"   /* re15_collision_debug_band */

#define RE15_R39_SPIELER_R     450   /* DAT_80073e94[+6] = 0x01c2 (PSX.EXE Datei 0x6469a), `lhu a1,6(v0)` @0x80031d6c */
#define RE15_R39_MASKE_SPIELER 1u    /* `ori a2,zero,0x1` @0x80031d74 */
#define RE15_R39_MASKE_GEGNER  4u    /* `ori a2,zero,0x4` @0x80116e68 (Gorilla-Wurzel) */

static int r39_zelle_ok(const re15_sca_entry_t *c, int band, uint32_t maske)
{
    return c->type == 1 && band == (int)(c->floor >> 4) && (maske & c->u0) != 0;   /* wie collision_constrain_impl */
}

/* Strecke (x0,z0)->(x1,z1) schneidet das OFFENE Innere der Zelle; ein Start im geschlossenen Rechteck zaehlt nicht
 * (ein schon ueberlappender Aktor, z.B. Landung ueber einem Auto, hat keine Herkunftsseite). */
static int r39_quert(const re15_sca_entry_t *c, int32_t x0, int32_t z0, int32_t x1, int32_t z1)
{
    {   /* billiger Ganzzahl-Vortest (PSX-Budget): Huellrechteck der Strecke gegen das offene Innere */
        const int32_t cx0 = (int32_t)c->x, cx1 = cx0 + (int32_t)c->width, cz0 = (int32_t)c->z, cz1 = cz0 + (int32_t)c->density;
        if ((x0 > x1 ? x0 : x1) <= cx0 || (x0 < x1 ? x0 : x1) >= cx1 ||
            (z0 > z1 ? z0 : z1) <= cz0 || (z0 < z1 ? z0 : z1) >= cz1) return 0;
    }
    const double ax = (double)c->x, az = (double)c->z;
    const double bx = ax + (double)c->width, bz = az + (double)c->density;
    if ((double)x0 >= ax && (double)x0 <= bx && (double)z0 >= az && (double)z0 <= bz) return 0;
    const double p[2] = { (double)x0, (double)z0 };
    const double d[2] = { (double)x1 - (double)x0, (double)z1 - (double)z0 };
    const double lo0[2] = { ax, az }, hi0[2] = { bx, bz };
    double lo = 0.0, hi = 1.0;
    for (int a = 0; a < 2; a++) {
        if (d[a] == 0.0) { if (p[a] <= lo0[a] || p[a] >= hi0[a]) return 0; continue; }
        double t0 = (lo0[a] - p[a]) / d[a], t1 = (hi0[a] - p[a]) / d[a];
        if (t0 > t1) { double t = t0; t0 = t1; t1 = t; }
        if (t0 > lo) lo = t0;
        if (t1 < hi) hi = t1;
        if (!(lo < hi)) return 0;
    }
    return 1;
}

/* RE2 FUN_8004c900: Code aus der Vorbild-Lage (Bit 1 = ausserhalb in X, Bit 0 = ausserhalb in Z). */
static unsigned r39_re2_code(const re15_sca_entry_t *c, int32_t r, int32_t vx, int32_t vz)
{
    const uint32_t t2 = (uint32_t)(2 * r + (int32_t)c->width);       /* `sll v0,a0,1 / addu t2,v0,v1` @0x8004c91c-20 */
    const uint32_t t1 = (uint32_t)(2 * r + (int32_t)c->density);     /* @0x8004c928-2c */
    const uint32_t a0 = (uint32_t)(vx + r - (int32_t)c->x);          /* @0x8004c930-3c */
    const uint32_t a1 = (uint32_t)(vz + r - (int32_t)c->z);          /* @0x8004c94c-58 */
    unsigned code = (a0 >= t2 ? 2u : 0u) | (a1 >= t1 ? 1u : 0u);    /* @0x8004c940-48 / @0x8004c95c-64 */
    if (a0 == 0xffffffffu || a0 == t2 + 1u) code = 2u;               /* @0x8004c968-7c */
    if (a1 == 0xffffffffu || a1 == t1 + 1u) code = 1u;               /* @0x8004c984-a8 */
    return code;
}

/* RE2 FUN_8004c900: Lage auf die Herkunftsseite (Vorzeichen von Lage - Vorbild). */
static void r39_re2_seite(const re15_sca_entry_t *c, int32_t r, unsigned code, int32_t vx, int32_t vz,
                          int32_t *x, int32_t *z)
{
    if (code & 2u) {
        const int32_t h = (int32_t)(c->width >> 1);                  /* `srl v1,v1,1` @0x8004c9e8 */
        const int32_t t = r + h + 1;                                 /* `addu` / `addiu t2,v0,1` @0x8004c9ec-f0 */
        const int32_t m = (int32_t)c->x + h;                         /* `addu v0,a0,v1` @0x8004c9f8 */
        *x = (*x - vx >= 0) ? m - t : m + t;                         /* `bltz a1` @0x8004c9f4, @0x8004ca00 / @0x8004ca04 */
    }
    if (code & 1u) {
        const int32_t h = (int32_t)(c->density >> 1);                /* @0x8004ca30 */
        const int32_t t = r + h + 1;                                 /* @0x8004ca34-38 */
        const int32_t m = (int32_t)c->z + h;                         /* @0x8004ca40 */
        *z = (*z - vz >= 0) ? m - t : m + t;                         /* `bltz a1` @0x8004ca3c, @0x8004ca48 / @0x8004ca4c */
    }
}

/* Kern: korrigiert (x,z) nach der RE1.5-Klemme; 1 = korrigiert. *zelle = Index der letzten Querungszelle. */
static int r39_querung(int32_t r, int band, uint32_t maske, int32_t vx, int32_t vz, int32_t *x, int32_t *z,
                       int *zelle, unsigned *code_out, int *rueckfall)
{
    if (!g_room_rdt_ok || !g_room_rdt.sca || g_room_rdt.sca_count <= 0 || band < 0) return 0;
    if (*x == vx && *z == vz) return 0;
    int korr = 0;
    for (int pass = 0; pass < 4; pass++) {
        int gefunden = -1; unsigned code = 0;
        for (int i = 0; i < g_room_rdt.sca_count; i++) {
            const re15_sca_entry_t *c = &g_room_rdt.sca[i];
            if (!r39_zelle_ok(c, band, maske) || !r39_quert(c, vx, vz, *x, *z)) continue;
            code = r39_re2_code(c, r, vx, vz);
            if (code != 0u) { gefunden = i; break; }
        }
        if (gefunden < 0) break;
        r39_re2_seite(&g_room_rdt.sca[gefunden], r, code, vx, vz, x, z);
        if (zelle) *zelle = gefunden;
        if (code_out) *code_out = code;
        korr = 1;
    }
    /* Rest-Querung (auch Zellen, deren Vorbild-Code 0 ist): Vorbild-Lage halten. */
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if (!r39_zelle_ok(c, band, maske) || !r39_quert(c, vx, vz, *x, *z)) continue;
        *x = vx; *z = vz; korr = 1;
        if (rueckfall) *rueckfall = 1;
        if (zelle) *zelle = i;
        break;
    }
    return korr;
}

/* FUN_8001bf04 (Richtungs-Code, Port re15_collision.c coll_contact_dir) fuer die korrigierte Lage - derselbe Wert, den
 * FUN_8003b0a4 nach dem Push-Handler fuer die getroffene Zelle schreibt (@0x8003b4b8-dc / @0x8003b4ec). */
static int32_t r39_richtung(int32_t px, int32_t pz, const re15_sca_entry_t *e)
{
    uint32_t ax = (uint32_t)(px - ((int32_t)e->x + (int32_t)e->width));
    uint32_t bx = (uint32_t)(px -  (int32_t)e->x);
    unsigned cx = ((ax & 0x80000000u) | ((bx & 0x80000000u) >> 1)) >> 30;
    if (cx == 2) {
        uint32_t az = (uint32_t)(pz - ((int32_t)e->z + (int32_t)e->density));
        uint32_t bz = (uint32_t)(pz -  (int32_t)e->z);
        unsigned cz = ((az & 0x80000000u) | ((bz & 0x80000000u) >> 1)) >> 30;
        if (cz == 0) return 0x400;
        if (cz == 3) return 0xc00;
        return 0xf00;
    }
    if (cx == 0) return 0x800;
    if (cx == 3) return 0x000;
    return 0xf00;
}

void re15_r39_affen_querung(re15_actor_t *e, int32_t vx, int32_t vz)
{
    if (!s_an || !e || e->type != RE15_R39_AFFEN_TYP) return;
    int32_t x = e->x, z = e->z; int zelle = -1, rf = 0; unsigned code = 0;
    if (!r39_querung((int32_t)e->hit_radius_min, (int)e->floor, RE15_R39_MASKE_GEGNER, vx, vz, &x, &z, &zelle, &code, &rf))
        return;
    re15_r39_log("querung F%u gorilla slot=%d zelle=%d code=%u vorbild=(%d,%d) re15=(%d,%d) -> (%d,%d)%s", r39_bild(),
                 r39_slot(e), zelle, code, (int)vx, (int)vz, (int)e->x, (int)e->z, (int)x, (int)z,
                 rf ? " (Rest-Querung: Vorbild)" : " (RE2 FUN_8004c900)");
    e->x = x; e->z = z;
    if (zelle >= 0 && !rf) {                                          /* Kontakt fuer die korrigierte Lage */
        const re15_sca_entry_t *c = &g_room_rdt.sca[zelle];
        e->ai_contact = (uint8_t)((r39_richtung(x, z, c) >> 4) + 8 + (c->u1 & 3));      /* @0x8003b4c0-dc */
        e->coll_cell_attr = (uint16_t)((uint16_t)c->u1 | ((uint16_t)c->floor << 8));    /* @0x8003b4ec */
    }
}

void re15_r39_wurf_querung(re15_actor_t *pl, int32_t vx, int32_t vz)
{
    if (!s_an || !pl) return;
    int32_t x = pl->x, z = pl->z; int zelle = -1, rf = 0; unsigned code = 0;
    if (!r39_querung(RE15_R39_SPIELER_R, re15_collision_debug_band(), RE15_R39_MASKE_SPIELER, vx, vz, &x, &z,
                     &zelle, &code, &rf))
        return;
    re15_r39_log("querung F%u spieler zelle=%d code=%u vorbild=(%d,%d) re15=(%d,%d) -> (%d,%d)%s", r39_bild(), zelle, code,
                 (int)vx, (int)vz, (int)pl->x, (int)pl->z, (int)x, (int)z,
                 rf ? " (Rest-Querung: Vorbild)" : " (RE2 FUN_8004c900)");
    pl->x = x; pl->z = z;
}

/* Sonde/Riegel: der Kern ohne Aktor (dieselbe Regel). */
int re15_r39_querung_sonde(int32_t r, int band, uint32_t maske, int32_t vx, int32_t vz, int32_t *x, int32_t *z)
{
    if (!s_an) return 0;
    return r39_querung(r, band, maske, vx, vz, x, z, NULL, NULL, NULL);
}
