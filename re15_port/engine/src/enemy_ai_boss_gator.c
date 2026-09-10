/*
 * RE1.5 Rebuilt — ROOM2090 Alligator-BOSSKAMPF (NUTZER-DESIGN, 2026-09-10).
 *
 * ⛔ EINORDNUNG: Dieser Kampf ist AUSDRUECKLICH ein vom Original abweichendes
 * Nutzer-Design ("Der laeuft aber etwas anders ab als im Original") — die
 * Ablauf-Konstanten hier sind DESIGN-Werte (je Konstante als DESIGN markiert),
 * die BAUSTEINE sind byte-true belegte Systeme:
 *   - Biss-Fenster {19,20,21} + Reichweiten-Mechanik: 0x23-KI B[3] @0x80118c68
 *     (enemy_ai_common.c re15_alligator_ai_tick — bleibt fuer andere Raeume
 *     unveraendert bestehen).
 *   - Schaden am Spieler: re15_player_take_damage mit attack_type 5
 *     (dmg_table[5] = 50, byte-true Tabelle DAT_8006f418, re15_damage.c:40).
 *   - Treffer AM Boss: re15_enemy_take_damage (FUN_80012d60-Gegner-Zweig) —
 *     der Boss-Tick faengt dessen state-2-Folge ab (HURT-Intercept unten).
 *   - Blut: re15_esp_fx_spawn_ex Code 0x1500 (der belegte Treffer-Burst,
 *     bite_blood_fx.md) an Bone 1 (vorderer Rumpf des EM23-Skeletts;
 *     Hierarchie selbst geparst: Kopfkette 1->5->6, Kiefer 7, Schwanz 11..15).
 *   - Modell/Clips: RE2-EM23-Bank (22 Bones, 12 Clips; Clip-Statistik
 *     2026-09-10: 0=Loko, 3=Lunge-Biss (Kiefer-Range 507), 4=Schnapp (596),
 *     6=Wende (Root-Yaw 2822), 7=Todesrolle (rueckwaerts+seitlich),
 *     10=Flinch (30 F, Rueckzuck), 11=Gleiten).
 *
 * RAUM-GEOMETRIE (ROOM2090.RDT, selbst geparst 2026-09-10):
 *   Pool innen        x[-8900..7200], z[-27000..-5400] (SCA-Aussenwaende)
 *   Wasserlinie       Y = -1620 (alle drei sce-8-Zonen, AOT p1 @0x09d6/09ea/09fe)
 *   Plattform-Block   x[-1700..1850], z[-20000..-12450] (SCA [4]/[7])
 *   Rampe Ost         x[1850..7200], z[-18100..-14500] (SCA [5], u1=0x01)
 *   Tuer A oben-links (x~-8900, z~-26000), Tuer B rechts (x~7200, z~-18100)
 *   LEITER: Westwand — Kamera-Cut 0 (pos 2754,-3132,-26190 -> target
 *   -7992,-18,-21564) zeigt sie links im Bild an der Westwand, z ~ -20500.
 *
 * ABLAUF (Nutzer-Punkte 2,3,4,5,6,8):
 *   LAUER   Start links oben im Wasser, Blick zur Leiter; voellig ruhig.
 *   Aggro   dist < GB_AGGRO ODER erster Treffer -> VERFOLGUNG.
 *   VERFOLG Ring um die Plattform (Sichtlinien-Test gegen den Block; wenn die
 *           Gerade zum Spieler den Block schneidet -> Eck-Wegpunkt der
 *           kuerzeren Umlaufrichtung).
 *   BISS    byte-true Fenster/Reichweite, Schaden 50 + Knockdown-Latch.
 *   HURT    Blut je Treffer; FLINCH-Clip nur beim Unterschreiten jeder
 *           10%-HP-Schwelle (300er-Schritte), sonst Panzerung (weiter jagen).
 *   CROSS   Leon auf der Plattform -> Anlauf an die naechste Laengskante,
 *           Oberkoerper heben, Bogen ueber die Plattform (Root-Bahn + Wirbel-
 *           saeulen-vz auf Kopfkette/Schwanz), drueben eintauchen.
 *   TOD     Clip 7 einmal, dann CORPSE-Verhalten der byte-true KI (state 7).
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>   /* getenv - RE15_GB_TEST (Sichtlauf-Hebel) */
#include "re15_boss_gator.h"
#include "re15_damage.h"     /* re15_enemy_player_dist, re15_player_take_damage, rand8, bone_world_pos */
#include "re15_esp.h"        /* re15_esp_fx_spawn_ex + re15_esp_room_bank */
#include "re15_room.h"       /* g_current_room_id */
#include "re15_skeleton.h"   /* re15_sin_q12 */

/* Wrapper aus enemy_ai_common.c (dort static — als re15_ai_* exportiert). */
extern void    re15_ai_advance(re15_actor_t *e, int32_t sp);
extern int     re15_ai_arc(const re15_actor_t *e, const re15_actor_t *t, int32_t r, int arc);
extern int32_t re15_ai_contact_reach(const re15_actor_t *e);
extern int     re15_ai_facing_dir(const re15_actor_t *e, const re15_actor_t *pl);
extern void    re15_player_knockdown_begin(int dir);
extern int     re15_player_is_grabbed(void);
extern void    re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);
extern re15_actor_t g_actors[];

/* ==== DESIGN-Konstanten (Nutzer-Auftrag 2026-09-10; KEINE Original-Werte) ==== */
#define GB_HP            3000   /* DESIGN: 10x die byte-true 300er-HP-Zeile @0x801175dc */
#define GB_AGGRO         9000   /* DESIGN: ~halbe Pool-Diagonale (Pool 16100x21600) */
#define GB_BITE_TYPE        5   /* byte-true dmg_table[5] = 50 (DAT_8006f418) */
#define GB_FLINCH_STEP    300   /* DESIGN: 10%-Schwellen von GB_HP */
#define GB_WATER_Y      -1200   /* byte-true Wasser-INIT-Y der 0x23-KI @0x8010c65c */
#define GB_SWIM_SPEED      48   /* byte-true B[4]-Advance-Proxy der 0x23-KI */
#define GB_LUNGE_SPEED     64   /* byte-true B[3]-Advance-Proxy der 0x23-KI */
/* Start/Blick (Nutzer-Punkt 2): links oben im Wasser, Blick zur Leiter (Westwand). */
#define GB_START_X      -7200   /* Nutzer 2026-09-10: "ganz hinten" - das dem
                                 * Einstieg (Tuer A, NW) diagonal ferne Ende des
                                 * Beckens: Sued-Becken West (Zone Slot3). Blick
                                 * ueber das Becken zur Leiter (Sichtlinie laeuft
                                 * WESTLICH am Block vorbei, frei). */
#define GB_START_Z      -9200
#define GB_LADDER_X     -8800   /* Leiter = Leons EINSTIEG an der Tuer-A-Nische oben
                                 * links (Tuer-Spawn -8200,-25550). GEMESSEN 2026-09-10:
                                 * der alte Kamera-Cut-0-Anker (-8700,-20500) lag 5300
                                 * suedlich - der Gator lauerte mit rot=2655 fast genau
                                 * ENTGEGENGESETZT ("muss erst 180 Grad drehen"). */
#define GB_LADDER_Z    -25600
/* Plattform-Block + Ring (RDT-SCA [4], Rand = halbe Koerperbreite ~800 + Marge). */
#define GB_PLAT_X0      -1700
#define GB_PLAT_X1       1850
#define GB_PLAT_Z0     -20000
#define GB_PLAT_Z1     -12450
#define GB_RING_M        1200   /* DESIGN: Wegpunkt-Abstand vom Block */
#define GB_BAHN_M        2600   /* Bahn-Endpunkte AUSSERHALB des SCA-Clamp-Radius
                                 * (Hitbox 2200 + Marge): der Anlauf erreicht die
                                 * Kante MIT aktiver Wand-Klemme - das fruehere
                                 * skip_clamp im Anlauf liess ihn flach QUER DURCH
                                 * die Insel gleiten, wenn die Route sie kreuzte
                                 * (Nutzer 2026-09-10: "klettert drueber, obwohl
                                 * er sich garnicht dort befindet"). */
/* Ost-Rampe (SCA [5], u1=0x01): zweite Uebersteig-Barriere (Nutzer-Marker
 * 2026-09-10: Leon (5766,-13028) noerdlich, Gator (4997,-20318) SUEDLICH fest
 * dagegen, spd=0 - "nur HIER sollte er ueber den Bereich druebersteigen"). */
#define GB_RAMP_X0       1850
#define GB_RAMP_X1       7200
#define GB_RAMP_Z0     -18100
#define GB_RAMP_Z1     -14500
/* CROSS-Bogen: Hub ueber die Wasserlinie. Plattformhoehe im Spiel gemessen
 * (siehe GB_PLAT_TOP_Y-Messnotiz unten) + Kopffreiheit. */
#define GB_CROSS_FRAMES   150   /* DESIGN: ~5950 Einheiten Bahn / 40 pro Frame */
/* MODELL-MASSSTAB (Nutzer 2026-09-10: "ein wenig zu gross fuer den Raum"):
 * GEMESSEN an der Bindpose des RE2-EM23 (MD1-Vertices + EMR-Bone-Offsets,
 * selbst geparst): x-Laenge 18313, Hoehe 3984 (y -1896..+2088), Breite 3378 —
 * LAENGER als die Pool-Kurzseite (16100). Scale 2/3 (Q12 2731) ueber den
 * byte-true ENTITY-RENDER-SCALE +0x166 (ScaleMatrix-Pfad FUN_8001e8c8
 * @0x8001e904-40; der Gorilla-Boss nutzt ihn mit 0x1b33): Koerper ~12200 lang,
 * ~2650 hoch — Boss-Praesenz (~1,5x Leon-Hoehe), passt in die Ring-Rinne
 * (7200). Hitbox/Reichweiten bleiben die byte-true 0x23-Werte. */
#define GB_SCALE_Q12     2731
/* Bogen-Hub: Peak-Root-Y so, dass der BAUCH auf der Plattformoberflaeche
 * (-1800 = floor-Ebene 1 x byte-true Y=-1800*floor @0x8010c7a4) aufliegt:
 * Bauch-Tiefe unter Root = 2088 (Mesh-y-Max, gemessen) x Scale 2/3 = 1392;
 * Peak = -1800 - 1392 = -3192; Hub = |-3192 - (-1200)|. */
#define RE15_GB_CROSS_HUB 1992
#define GB_ARC_VZ_MAX     260   /* DESIGN: Q12-Spitzenkruemmung je Wirbelgelenk (~23 deg) */
#define GB_PITCH_MAX      300   /* DESIGN: Root-Neigung am Bahnanfang/-ende (~26 deg) */
/* Spinnen (Nutzer-Punkte 1+7): Plattform-Sitzplaetze + Ostwand-Flucht. */
#define GB_SPID0_X       -700
#define GB_SPID0_Z     -17800
#define GB_SPID1_X        800
#define GB_SPID1_Z     -14800
#define GB_WALL_X        7200   /* Ostwand ("die Wand dahinter" aus Kamera-Cut-9-Sicht) */
#define GB_SPID_RUN        90   /* DESIGN: Flucht-Tempo (schnell, Nutzer: "schnell hochlaufen") */
#define GB_SPID_CLIMB      60   /* DESIGN: Kletter-Tempo an der Wand */

/* Boss-Phasen (eigenes Feld, unabhaengig vom byte-true state/sub_state-Raum). */
enum {
    GBP_OFF = 0,    /* nicht initialisiert */
    GBP_LURK,       /* Punkt 2: Lauerstellung an der Leiter */
    GBP_CHASE,      /* Punkt 6: Ring-Verfolgung im Wasser */
    GBP_LUNGE,      /* Punkt 3: Biss */
    GBP_FLINCH,     /* Punkt 5: Schadensanimation (Schwelle) */
    GBP_GUARD,      /* Plattform-Belagerung: Hochbiss, solange Leon oben steht */
    GBP_CROSS_APPR, /* Punkt 8: Anlauf an die Startkante (KEIN Teleport) */
    GBP_CROSS,      /* Punkt 8: Bogen ueber die Plattform */
    GBP_DIE,        /* Todesrolle (Clip 7) */
    GBP_DEAD
};

typedef struct {
    uint8_t  phase;
    uint8_t  aggro;          /* Kampf gestartet (Punkt 2: "nicht eher bis Leon naeher kommt") */
    int16_t  prev_hp;        /* Treffer-Erkennung (HP-Delta) */
    int16_t  next_flinch_hp; /* naechste 10%-Schwelle */
    int16_t  timer;
    /* CROSS-Bahn */
    int32_t  cx0, cz0, cx1, cz1;   /* Kante A -> Kante B */
    int16_t  ct;                   /* 0..GB_CROSS_FRAMES */
    int16_t  cross_cd;             /* Ring-Pflichtphase nach einer Ueberquerung */
    int16_t  cframes;              /* Bahn-Dauer der laufenden Ueberquerung */
    int16_t  t_ein, t_aus;         /* Frames, in denen die Bahn UEBER der Insel ist */
    int16_t  arc_vz;               /* aktueller Wirbelsaeulen-Winkel (Q12) */
    int16_t  pitch_vz;             /* Root-Neigung entlang der Bogen-Bahn (Q12) */
    int16_t  guard_t;              /* Aufricht-Rampe der Plattform-Belagerung */
    uint8_t  bite_done;            /* 1 Biss pro Lunge/Cross-Passage (Design-Schaden!) */
    /* Spinnen-Flucht */
    uint8_t  spider_flee;          /* 1 = Flucht ausgeloest (Punkt 7) */
} gb_state_t;

static gb_state_t s_gb[RE15_ACTOR_MAX];
static unsigned   s_gb_room;       /* Raum, fuer den s_gb gilt (Reset bei Wechsel) */

int re15_gator_boss_active(const re15_actor_t *e)
{
    return e && e->type == 0x23u && ((g_current_room_id & 0xFFFEu) == 0x2090u);
}

int re15_gator_boss_skip_clamp(const re15_actor_t *e)
{
    if (!re15_gator_boss_active(e)) return 0;
    const gb_state_t *g = &s_gb[(int)(e - g_actors)];
    /* NUR die Bogen-Phase: der Anlauf laeuft seit GB_BAHN_M (Kante ausserhalb
     * des Clamp-Radius) wieder MIT Wand-Klemme - das fruehere skip im Anlauf
     * war der Durch-die-Insel-Tunnel (Nutzer 2026-09-10). */
    return g->phase == GBP_CROSS;
}

/* Schneidet die Strecke (x0,z0)->(x1,z1) das um `m` aufgeblasene Plattform-Rechteck?
 * (Slab-Test; reicht fuer die Ring-Entscheidung.) */
static int gb_seg_hits_rect(int32_t x0, int32_t z0, int32_t x1, int32_t z1,
                            int32_t bx0, int32_t bz0, int32_t bx1, int32_t bz1,
                            int32_t m)
{
    int32_t rx0 = bx0 - m, rx1 = bx1 + m;
    int32_t rz0 = bz0 - m, rz1 = bz1 + m;
    /* beide Punkte auf derselben Aussenseite -> kein Schnitt */
    if (x0 < rx0 && x1 < rx0) return 0;
    if (x0 > rx1 && x1 > rx1) return 0;
    if (z0 < rz0 && z1 < rz0) return 0;
    if (z0 > rz1 && z1 > rz1) return 0;
    /* Punkt innen -> Schnitt */
    if (x0 >= rx0 && x0 <= rx1 && z0 >= rz0 && z0 <= rz1) return 1;
    if (x1 >= rx0 && x1 <= rx1 && z1 >= rz0 && z1 <= rz1) return 1;
    /* Strecke gegen die 4 Kanten: Parametertest auf X- und Z-Slabs */
    int64_t dx = x1 - x0, dz = z1 - z0;
    /* t-Intervall im X-Slab */
    int64_t tx0 = 0, tx1 = 4096;
    if (dx != 0) {
        int64_t ta = ((int64_t)(rx0 - x0) << 12) / dx;
        int64_t tb = ((int64_t)(rx1 - x0) << 12) / dx;
        tx0 = (ta < tb) ? ta : tb; tx1 = (ta < tb) ? tb : ta;
    } else if (x0 < rx0 || x0 > rx1) return 0;
    int64_t tz0 = 0, tz1 = 4096;
    if (dz != 0) {
        int64_t ta = ((int64_t)(rz0 - z0) << 12) / dz;
        int64_t tb = ((int64_t)(rz1 - z0) << 12) / dz;
        tz0 = (ta < tb) ? ta : tb; tz1 = (ta < tb) ? tb : ta;
    } else if (z0 < rz0 || z0 > rz1) return 0;
    int64_t lo = (tx0 > tz0) ? tx0 : tz0;
    int64_t hi = (tx1 < tz1) ? tx1 : tz1;
    if (lo > hi) return 0;
    if (hi < 0 || lo > 4096) return 0;
    return 1;
}

static int gb_seg_hits_platform(int32_t x0, int32_t z0, int32_t x1, int32_t z1, int32_t m)
{
    return gb_seg_hits_rect(x0, z0, x1, z1,
                            GB_PLAT_X0, GB_PLAT_Z0, GB_PLAT_X1, GB_PLAT_Z1, m);
}
static int gb_seg_hits_ramp(int32_t x0, int32_t z0, int32_t x1, int32_t z1, int32_t m)
{
    return gb_seg_hits_rect(x0, z0, x1, z1,
                            GB_RAMP_X0, GB_RAMP_Z0, GB_RAMP_X1, GB_RAMP_Z1, m);
}

/* Ring-Wegpunkt: die vier Block-Ecken (+Marge); waehle die Ecke, die vom Gator aus
 * sichtbar ist (Strecke frei) und den Winkelumweg zu Leon minimiert. */
static void gb_ring_target(const re15_actor_t *e, const re15_actor_t *pl,
                           int32_t *tx, int32_t *tz)
{
    static const int32_t C[4][2] = {
        { GB_PLAT_X0 - GB_RING_M, GB_PLAT_Z0 - GB_RING_M },
        { GB_PLAT_X1 + GB_RING_M, GB_PLAT_Z0 - GB_RING_M },
        { GB_PLAT_X1 + GB_RING_M, GB_PLAT_Z1 + GB_RING_M },
        { GB_PLAT_X0 - GB_RING_M, GB_PLAT_Z1 + GB_RING_M },
    };
    /* WAND-FOLLOWING (Nutzer 2026-09-10: "ganz ans Ende der Wand laufen und
     * erst dann drehen"): statt der global guenstigsten Ecke die Ecken-FOLGE
     * in der kuerzeren Umlaufrichtung ablaufen - naechste Ecke auf dem Weg
     * als Ziel; Best-Cost steuerte in Wandnaehe diagonal in die Klemme. */
    int gs, ps, i;
    /* Seiten-Index 0=N,1=O,2=S,3=W (zwischen den Ecken C[0..3]) */
    #define GB_SEITE(px, pz)         ((pz) <= GB_PLAT_Z0 ? 0 : (pz) >= GB_PLAT_Z1 ? 2 : (px) >= GB_PLAT_X1 ? 1 : 3)
    gs = GB_SEITE(e->x, e->z);
    ps = GB_SEITE(pl->x, pl->z);
    if (gs == ps) { *tx = pl->x; *tz = pl->z; return; }
    /* kuerzere Drehrichtung: Seitenabstand im Uhrzeigersinn vs. gegen ihn */
    {
        int cw = (ps - gs + 4) & 3, ecke;
        /* Ziel-Ecke = die in Laufrichtung naechste Ecke der eigenen Seite:
         * Uhrzeigersinn: Seite k endet an Ecke (k+1)&3; gegen ihn: an Ecke k. */
        ecke = (cw <= 2) ? ((gs + 1) & 3) : gs;
        *tx = C[ecke][0]; *tz = C[ecke][1];
    }
    #undef GB_SEITE
    (void)i;
}

/* Treffer-Intercept: re15_enemy_take_damage hat state=2 (+ ggf. 3) gesetzt und
 * sub_state_1 mit dem Reaktions-Code ueberschrieben. Der Boss uebersetzt das in
 * sein eigenes Phasenmodell: Blut IMMER, Flinch nur an 10%-Schwellen (Punkt 5). */
static void gb_absorb_hit(re15_actor_t *e, gb_state_t *g)
{
    if (e->state == 3) { g->phase = GBP_DIE; g->timer = 0; e->state = 1; return; }
    if (e->state != 2) return;
    e->state = 1;                       /* Boss-Panzerung: HURT-Route selbst verwalten */
    e->hit_react &= (uint8_t)~1u;       /* Fenster-Latch loesen (byte-true HURT-Tail) */
    if (!g->aggro) g->aggro = 1;        /* Fernschuss startet den Kampf */
    /* Blut bei JEDEM Treffer (Punkt 5): Burst 0x1500 am vorderen Rumpf (Bone 1). */
    {
        int32_t bp[3];
        re15_enemy_bone_world_pos(e, 1, bp);
        re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500,
                             bp[0], bp[1], bp[2], (int16_t)e->rot_y);
    }
    if (e->hp <= g->next_flinch_hp && g->phase != GBP_CROSS && g->phase != GBP_DIE) {
        while (g->next_flinch_hp >= e->hp) g->next_flinch_hp -= GB_FLINCH_STEP;
        g->phase = GBP_FLINCH; g->timer = 0;
        e->motion = 10; e->anim_frame = 0;   /* Flinch-Clip 10 (30 F, Clip-Statistik) */
    }
}

/* Naechster Randpunkt der aufgeblasenen Insel-VEREINIGUNG (Block und Rampe,
 * je +GB_BAHN_M) fuer einen Aussenpunkt. Zwei Durchgaenge schieben aus der
 * jeweils anderen Klemmzone heraus (Innenecken Block/Rampe) - die frueheren
 * FESTEN Bahnkanten konnten in der Klemmzone des Nachbar-Rechtecks liegen
 * (Nutzer-Marker 2026-09-10: Anlauf-Patt, spd=0, "bleibt komisch stehen"). */
static void gb_rim_out(int32_t *x, int32_t *z,
                       int32_t bx0, int32_t bz0, int32_t bx1, int32_t bz1)
{
    int32_t rx0 = bx0 - GB_BAHN_M, rx1 = bx1 + GB_BAHN_M;
    int32_t rz0 = bz0 - GB_BAHN_M, rz1 = bz1 + GB_BAHN_M;
    if (*x < rx0 || *x > rx1 || *z < rz0 || *z > rz1) return;   /* schon draussen */
    {
        int32_t dw = *x - rx0, de = rx1 - *x, dn = *z - rz0, ds = rz1 - *z;
        int32_t m = dw; int side = 0;
        if (de < m) { m = de; side = 1; }
        if (dn < m) { m = dn; side = 2; }
        if (ds < m) { m = ds; side = 3; }
        if      (side == 0) *x = rx0;
        else if (side == 1) *x = rx1;
        else if (side == 2) *z = rz0;
        else                *z = rz1;
    }
}
static void gb_rim_point(int32_t px, int32_t pz, int32_t *ox, int32_t *oz)
{
    int32_t x = px, z = pz;
    int it;
    for (it = 0; it < 2; it++) {
        gb_rim_out(&x, &z, GB_PLAT_X0, GB_PLAT_Z0, GB_PLAT_X1, GB_PLAT_Z1);
        gb_rim_out(&x, &z, GB_RAMP_X0, GB_RAMP_Z0, GB_RAMP_X1, GB_RAMP_Z1);
    }
    /* In die Pool-Innenflaeche klemmen (nie hinter die Aussenwaende). */
    if (x < -8900 + 500) x = -8900 + 500;
    if (x >  7200 - 500) x =  7200 - 500;
    if (z < -27000 + 500) z = -27000 + 500;
    if (z >  -5400 - 500) z =  -5400 - 500;
    *ox = x; *oz = z;
}

/* t-Intervall (Q12, 0..4096) der Strecke (x0,z0)->(x1,z1) durch ein Rechteck
 * (+m). Rueckgabe 0 = kein Schnitt. Fuer das Bogen-Fenster der Ueberquerung. */
static int gb_seg_rect_span(int32_t x0, int32_t z0, int32_t x1, int32_t z1,
                            int32_t bx0, int32_t bz0, int32_t bx1, int32_t bz1,
                            int32_t m, int32_t *t0, int32_t *t1)
{
    int32_t rx0 = bx0 - m, rx1 = bx1 + m, rz0 = bz0 - m, rz1 = bz1 + m;
    int64_t dx = x1 - x0, dz = z1 - z0;
    int64_t ax0 = 0, ax1 = 4096, az0 = 0, az1 = 4096;
    if (dx != 0) {
        int64_t ta = ((int64_t)(rx0 - x0) << 12) / dx;
        int64_t tb = ((int64_t)(rx1 - x0) << 12) / dx;
        ax0 = (ta < tb) ? ta : tb; ax1 = (ta < tb) ? tb : ta;
    } else if (x0 < rx0 || x0 > rx1) return 0;
    if (dz != 0) {
        int64_t ta = ((int64_t)(rz0 - z0) << 12) / dz;
        int64_t tb = ((int64_t)(rz1 - z0) << 12) / dz;
        az0 = (ta < tb) ? ta : tb; az1 = (ta < tb) ? tb : ta;
    } else if (z0 < rz0 || z0 > rz1) return 0;
    {
        int64_t lo = (ax0 > az0) ? ax0 : az0;
        int64_t hi = (ax1 < az1) ? ax1 : az1;
        if (lo > hi || hi < 0 || lo > 4096) return 0;
        if (lo < 0) lo = 0;
        if (hi > 4096) hi = 4096;
        *t0 = (int32_t)lo; *t1 = (int32_t)hi;
    }
    return 1;
}

/* Ueberquerung: gerade Bahn vom eigenen Randpunkt zum Leon-seitigen Randpunkt
 * UEBER die Insel; Dauer aus der Bahnlaenge. Kein Anlauf-Zustand mehr - der
 * Aufrufer startet die Bahn erst aus Kanten-Naehe (Rim-Distanz < 1400). */
static void gb_cross_begin(re15_actor_t *e, gb_state_t *g, const re15_actor_t *pl)
{
    int32_t ex, ez, lx, lz;
    gb_rim_point(e->x, e->z, &ex, &ez);
    gb_rim_point(pl->x, pl->z, &lx, &lz);
    g->cx0 = ex; g->cz0 = ez;
    g->cx1 = lx; g->cz1 = lz;
    {
        int64_t dx = lx - ex, dz = lz - ez;
        int64_t d2 = dx * dx + dz * dz;
        int32_t len = 64; while ((int64_t)len * len < d2 && len < 30000) len += 64;
        g->cframes = (int16_t)(len / 40);
        if (g->cframes < 90)  g->cframes = 90;
        if (g->cframes > 240) g->cframes = 240;
    }
    /* BOGEN-FENSTER (Nutzer-Marker 2026-09-10: er hing bei (-2454, y=-2986)
     * UEBER DEM WASSER - der sin-Hub lief ueber die GESAMTE Bahn, also auch
     * ueber die 2600er-Wasser-Endstuecke vor/nach der Insel: er "schwebte
     * voellig frei"). Der Bogen wirkt nur auf dem Insel-Abschnitt der Bahn. */
    {
        int32_t ta0, ta1, tb0, tb1, lo = 4096, hi = 0;
        if (gb_seg_rect_span(g->cx0, g->cz0, g->cx1, g->cz1,
                             GB_PLAT_X0, GB_PLAT_Z0, GB_PLAT_X1, GB_PLAT_Z1,
                             300, &ta0, &ta1)) { if (ta0 < lo) lo = ta0; if (ta1 > hi) hi = ta1; }
        if (gb_seg_rect_span(g->cx0, g->cz0, g->cx1, g->cz1,
                             GB_RAMP_X0, GB_RAMP_Z0, GB_RAMP_X1, GB_RAMP_Z1,
                             300, &tb0, &tb1)) { if (tb0 < lo) lo = tb0; if (tb1 > hi) hi = tb1; }
        if (lo >= hi) { lo = 0; hi = 4096; }              /* Sicherheitsnetz */
        g->t_ein = (int16_t)(((int32_t)g->cframes * lo) >> 12);
        g->t_aus = (int16_t)(((int32_t)g->cframes * hi) >> 12);
        if (g->t_aus <= g->t_ein) g->t_aus = (int16_t)(g->t_ein + 1);
    }
    g->ct = 0; g->bite_done = 0;
    g->phase = GBP_CROSS;
    g->spider_flee = 1;                  /* Punkt 7: Spinnen fliehen JETZT */
    e->motion = 0; e->anim_frame = 0;
    re15_enemy_steer_point(e, g->cx1, g->cz1, 0x800);   /* Blick ueber die Insel */
    {   /* CROSS-Start-Telemetrie (Diagnose) */
        static FILE *s_cl = NULL;
        if (!s_cl) s_cl = fopen("gator_boss.log", "a");
        if (s_cl) {
            fprintf(s_cl, "CROSS start=(%d,%d) ende=(%d,%d) frames=%d fenster=%d..%d\n",
                    g->cx0, g->cz0, g->cx1, g->cz1,
                    (int)g->cframes, (int)g->t_ein, (int)g->t_aus);
            fflush(s_cl);
        }
    }
}

void re15_gator_boss_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    gb_state_t   *g  = &s_gb[slot];

    if (s_gb_room != g_current_room_id) {     /* Raumwechsel: Boss-Zustand frisch */
        memset(s_gb, 0, sizeof s_gb);
        s_gb_room = g_current_room_id;
    }
    /* NUTZER-MARKER 2026-09-10 ("haengt ganz gewaltig"): nach Toeten des Bosses
     * + SAME-ROOM-Re-Entry (Raum-ID unveraendert!) spawnte main.c einen frischen
     * Aktor (memset: hp=0/state=0), aber s_gb stand noch auf GBP_DEAD des
     * Vorgaengers - der Neue blieb als Geist stehen. Neuspawn erkennen und den
     * Boss-Zustand nullen (em_flag_id 0xFF = Boss erscheint je Betreten). */
    if (g->phase != GBP_OFF && e->state == 0 && e->hp <= 0)
        memset(g, 0, sizeof *g);

    if (g->phase == GBP_OFF) {                /* INIT (Punkt 2): Lauerstellung */
        if (e->hp <= 0) e->hp = GB_HP;
        g->prev_hp = e->hp;
        g->next_flinch_hp = (int16_t)(e->hp - GB_FLINCH_STEP);
        e->x = GB_START_X; e->z = GB_START_Z; e->y = GB_WATER_Y;
        /* Blick zur Leiter ueber die ENGINE-Peilung (steer mit Voll-Slew 0x800 =
         * Sofort-Snap) — exakt die Konvention, mit der advance/Verfolgung laufen. */
        re15_enemy_steer_point(e, GB_LADDER_X, GB_LADDER_Z, 0x800);
        e->render_scale_q12 = GB_SCALE_Q12;   /* 2/3-Massstab, s. GB_SCALE_Q12 oben */
        /* CLIPPING-FIX (Nutzer 2026-09-10, GEMESSEN an den Markern: Gator y=-1200,
         * Leon y=0): das byte-true Y-Band-Gate des Body-Push (@0x8002b1f0, Push nur
         * bei |dy| < hit_height-Summe) sah dy=1200 > 720 und liess Leon DURCH den
         * Koerper laufen. Kollisionszentrum auf Bodenniveau heben: +0x7c-Offset =
         * -GB_WATER_Y. Auf der Plattform-Passage (Root -3192, Leon oben -1800)
         * bleibt dy=192 im Band - der Push wirkt dort ebenfalls. */
        e->hit_offset_y = (int16_t)(-GB_WATER_Y);
        e->motion = 0; e->anim_frame = 0;     /* flacher Loko-Zyklus als Wasser-Lauern
                                               * (Clip 6 = WENDE ringelte den Koerper,
                                               * Clip 11 = AUFGERICHTETE Pose — beide im
                                               * Sichtlauf 2026-09-10 verworfen) */
        e->state = 1; e->sub_state_1 = 0;
        g->phase = GBP_LURK;
        {   /* Sichtlauf-Hebel: 1 = Kampf sofort, 2 = zusaetzlich sofort die
             * Plattform-Ueberquerung erzwingen (Bogen + Spinnen-Flucht pruefen),
             * 3 = Todes-Pfad testen (hp=1; nach ~10 s Raumzeit toeten). */
            const char *tv = getenv("RE15_GB_TEST");
            if (tv) { g->aggro = 1;
                      if (*tv == '2') gb_cross_begin(e, g, &g_actors[RE15_ACTOR_SLOT_PLAYER]);
                      if (*tv == '3') e->hp = 1;
                      if (*tv == '5') {          /* Rampen-Repro: Nutzer-Marker-Lage
                                                  * (Gator noerdlich der Rampe), scharf
                                                  * erst wenn Leon seine Route lief */
                          e->x = 4997; e->z = -20318;
                          g->aggro = 0;
                      } }
        }
    }

    {   /* GB_TEST=3: Kill nach ~10 s Raumzeit (Todes-Pfad-Probe ohne Waffe).
         * EIGENER Zaehler - g->timer wird von LUNGE/FLINCH-Commits genullt. */
        static int s_kill_ctr = 0;
        const char *tv = getenv("RE15_GB_TEST");
        if (tv && *tv == '3' && g->phase != GBP_DIE && g->phase != GBP_DEAD) {
            s_kill_ctr++;
            if (s_kill_ctr == 300) { e->hp = -1; e->state = 3; }
        }
    }
    /* Treffer seit letztem Tick? (HP-Delta + HURT/DEATH-Intercept) */
    if (e->hp != g->prev_hp || e->state == 2 || e->state == 3) {
        gb_absorb_hit(e, g);
        g->prev_hp = e->hp;
    }

    int32_t dist = re15_enemy_player_dist(e, pl);

    switch (g->phase) {
    case GBP_LURK:
        {   const char *tv5 = getenv("RE15_GB_TEST");
            if (tv5 && *tv5 == '5') {
                static int s_t5 = 0;
                if (++s_t5 >= 900) g->aggro = 1;   /* ~30 s: Leon ist im SO-Becken */
                if (!g->aggro) { e->motion = 0; e->anim_frame++; break; }
                g->phase = GBP_CHASE; e->motion = 0; e->anim_frame = 0; break;
            }
        }                            /* Punkt 2: ruhig, bis Leon naeher kommt.
                                               * GEMESSEN 2026-09-10: der Tuer-A-Spawn
                                               * (-8200,-25550) liegt nur ~1400 vom
                                               * Lauerplatz — Distanz allein wuerde den
                                               * Kampf schon in der Tuernische starten.
                                               * "Naeher kommen" = Leon dringt vom
                                               * Einstieg in den Pool vor (oder laeuft
                                               * dem Gator direkt vor die Nase, oder
                                               * schiesst — gb_absorb_hit setzt aggro). */
        if (dist < GB_AGGRO) g->aggro = 1;                 /* DESIGN: mit dem fernen
                                                            * Lauerplatz ("ganz hinten")
                                                            * reicht die Distanzregel -
                                                            * der Tuer-Spawn ist ~16600
                                                            * entfernt */
        if (g->aggro) { g->phase = GBP_CHASE; e->motion = 0; e->anim_frame = 0; break; }
        e->motion = 0;                        /* still liegen, Blick zur Leiter */
        e->anim_frame++;
        break;

    case GBP_CHASE: {                         /* Punkt 6: Ring-Verfolgung */
        int player_on_platform =                    /* begehbare Flaeche OBEN =
                                                     * Block UND Ost-Rampe (Nutzer-
                                                     * Marker: Leon auf der Rampe
                                                     * bei 2318,-1800) */
            ((pl->x >= GB_PLAT_X0 && pl->x <= GB_PLAT_X1 &&
              pl->z >= GB_PLAT_Z0 && pl->z <= GB_PLAT_Z1) ||
             (pl->x >= GB_RAMP_X0 && pl->x <= GB_RAMP_X1 &&
              pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1));
        int blocked = gb_seg_hits_platform(e->x, e->z, pl->x, pl->z, GB_RING_M / 2);
        int rampe   = gb_seg_hits_ramp(e->x, e->z, pl->x, pl->z, GB_RING_M / 2);
        if (g->cross_cd > 0) g->cross_cd--;
        /* Nutzer-Marker 2026-09-10: die RAMPE ist der natuerliche Uebersteigpunkt
         * ("nur HIER sollte er ... druebersteigen") - sie triggert OHNE Cooldown;
         * oestlich von ihr steht die Aussenwand, ein Umweg existiert dort nicht. */
        /* Punkt 7/8 (Nutzer-Wortlaut "wenn der Aligator Richtung Platform kommt"):
         * die Ueberquerung passiert AUCH in der normalen Verfolgung, sobald die
         * Plattform zwischen Gator und Leon liegt - nicht nur, wenn Leon oben
         * steht (Nutzer-Befund 2026-09-10: "klettert nicht ueber die Platform").
         * Nach einer Passage erzwingt cross_cd eine Ring-Phase (Abwechslung). */
        {   const char *tv6 = getenv("RE15_GB_TEST");
            if (tv6 && *tv6 == '6') player_on_platform = 1;
        }
        if (player_on_platform) {
            /* Nutzer 2026-09-10: solange Leon OBEN steht -> BELAGERN und nach
             * oben schnappen; geklettert wird erst, wenn er heruntergesprungen
             * ist (dann faellt player_on_platform und die blocked-Regel unten
             * loest die Ueberquerung aus). */
            g->phase = GBP_GUARD; g->guard_t = 0; g->bite_done = 0;
            e->motion = 0; e->anim_frame = 0;
            break;
        }
        if ((blocked || rampe) && g->cross_cd == 0) {
            /* Insel zwischen Gator und Leon: zum eigenen Randpunkt schwimmen
             * (MIT Wand-Klemme - Randpunkte liegen ausserhalb der Klemmzonen)
             * und erst DORT die Bogen-Bahn starten.
             * BAHN-PROBE (Nutzer-Marker 2026-09-10: Bogen an der NW-ECKE,
             * Leon im NO - beide auf der Nordseite!): streift die Sichtlinie
             * die Insel nur an einer Ecke, laeuft die Randpunkt-Bahn an der
             * Kante ENTLANG statt darueber. Klettern NUR, wenn die Bahn die
             * Insel substanziell quert (Marge -200) - sonst fuehrt das
             * Wand-Following unten um die Ecke. */
            int32_t ex, ez, lx, lz;
            int64_t ddx, ddz;
            gb_rim_point(e->x, e->z, &ex, &ez);
            gb_rim_point(pl->x, pl->z, &lx, &lz);
            if (gb_seg_hits_platform(ex, ez, lx, lz, -200)
                || gb_seg_hits_ramp(ex, ez, lx, lz, -200)) {
                ddx = e->x - ex; ddz = e->z - ez;
                if (ddx * ddx + ddz * ddz < (int64_t)1400 * 1400) {
                    gb_cross_begin(e, g, pl);
                } else {
                    re15_enemy_steer_point(e, ex, ez, 0x40);
                    re15_ai_advance(e, GB_SWIM_SPEED);
                    e->y = GB_WATER_Y;
                    if (e->motion != 0) { e->motion = 0; e->anim_frame = 0; }
                    e->anim_frame++;
                }
                break;
            }
            /* Bahn quert nichts -> kein Klettern; unten uebernimmt der Ring. */
        }

        int32_t tx = pl->x, tz = pl->z;
        if (blocked)
            gb_ring_target(e, pl, &tx, &tz);  /* Block im Weg -> Eck-Wegpunkt */
        {
            int slew = (re15_engine_rand8() & 0x1f) + 6;   /* byte-true B[4]-Slew-Streuung */
            re15_enemy_steer_point(e, tx, tz, slew);
        }
        re15_ai_advance(e, GB_SWIM_SPEED);
        e->y = GB_WATER_Y;
        if (e->motion != 0) { e->motion = 0; e->anim_frame = 0; }
        e->anim_frame++;

        /* Biss-Commit: byte-true Reichweiten-/Bogen-Test (B[3]-Gate 6000/0x180). */
        if (pl->hit_react == 0 && e->hit_stun == 0 &&
            re15_ai_arc(e, pl, 0x1770, 0x180)) {
            g->phase = GBP_LUNGE; g->timer = 0; g->bite_done = 0;
            /* Clip 4 = SCHNAPP-Biss - Kieferkurve GEMESSEN (EDD/EMR 2026-09-10):
             * Maul reisst ab Frame 4 auf, Peak -591 @F12, zu @F24 (45 F). Clip 3
             * oeffnet erst ab ~F54 (Peak F96/150) - der alte 40-Frame-Abbruch
             * zeigte deshalb "keinerlei Beissanimation" (Nutzer-Befund). */
            e->motion = 4; e->anim_frame = 0;
        }
        break; }

    case GBP_LUNGE: {                         /* Punkt 3: Biss mit Schaden */
        re15_enemy_steer_point(e, pl->x, pl->z, 0x30);    /* byte-true B[3]-Slew */
        if (dist > re15_ai_contact_reach(e)) re15_ai_advance(e, GB_LUNGE_SPEED);
        e->y = GB_WATER_Y;
        if (!g->bite_done
            && e->anim_frame >= 8 && e->anim_frame <= 20   /* Maul-offen-Fenster der
                                                            * gemessenen Clip-4-Kurve */
            && pl->hit_react == 0 && dist <= re15_ai_contact_reach(e)
            && re15_ai_arc(e, pl, re15_ai_contact_reach(e), 0x400)) {
            /* Fenster {19,20,21} @0x80118c68 (byte-true Mechanik) — DESIGN-Schaden:
             * attack_type 5 -> dmg_table[5] = 50 + Knockdown-Latch. EIN Biss pro
             * Lunge (bite_done): ohne den Latch traefen alle drei Fensterframes
             * (gemessen 2026-09-10: Leon 100 HP -> Game Over in <2 s). */
            re15_player_take_damage(pl, GB_BITE_TYPE, e->x, e->z);
            /* Byte-true KNOCKDOWN (cmd-2-Klasse) - die Reaktion, die der 0x23-Biss
             * im Original ausloest; die 22-Frame-Hurt-Zuckung allein war "keinerlei
             * Schadens-Animation" (Nutzer-Befund, POSE_DUMP-gemessen mo=8/22F). */
            re15_player_knockdown_begin(re15_ai_facing_dir(e, pl));
            pl->hit_react |= 1;
            g->bite_done = 1;
            e->hit_stun = 0x64;                            /* byte-true Re-Attack-Sperre */
        }
        e->anim_frame++;
        if (e->anim_frame > 42) {             /* Clip 4 (45 F) ausklingen lassen */
            if (e->hit_stun == 0) e->hit_stun = 0x2d;      /* byte-true Nachlauf-Sperre */
            g->phase = GBP_CHASE;
            e->motion = 0; e->anim_frame = 0;
        }
        break; }

    case GBP_FLINCH:                          /* Punkt 5: sichtbare Schadensanimation */
        e->motion = 10;
        e->anim_frame++;
        g->timer++;
        if (g->timer >= 30) {                 /* Clip 10 = 30 Frames (Clip-Statistik) */
            g->phase = GBP_CHASE;
            e->motion = 0; e->anim_frame = 0;
        }
        break;

    case GBP_GUARD: {                         /* Plattform-Belagerung (Nutzer-Design):
                                               * an die Kante unter Leon, aufrichten,
                                               * nach OBEN schnappen inkl. Schaden. */
        int oben = ((pl->x >= GB_PLAT_X0 && pl->x <= GB_PLAT_X1 &&
                     pl->z >= GB_PLAT_Z0 && pl->z <= GB_PLAT_Z1) ||
                    (pl->x >= GB_RAMP_X0 && pl->x <= GB_RAMP_X1 &&
                     pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1));
        {   const char *tv6 = getenv("RE15_GB_TEST");
            if (tv6 && *tv6 == '6') oben = 1;   /* Sichtlauf: Belagerung erzwingen */
        }
        if (!oben) {                          /* heruntergesprungen -> Verfolgung
                                               * (deren blocked-Regel klettert) */
            g->phase = GBP_CHASE; g->arc_vz = 0; g->pitch_vz = 0;
            e->motion = 0; e->anim_frame = 0;
            break;
        }
        re15_enemy_steer_point(e, pl->x, pl->z, 0x40);
        if (dist > 3000) re15_ai_advance(e, GB_SWIM_SPEED);  /* der SCA-Clamp haelt
                                               * ihn an der Blockwand - er drueckt
                                               * sich unter die Kante */
        e->y = GB_WATER_Y;
        /* Aufrichten NUR NAHE DER KANTE (Nutzer 2026-09-10: "klettert ueber die
         * Insel, obwohl er ueberhaupt nicht da ist" - die Aufbaeum-Rampe lief ab
         * GUARD-EINTRITT, also schon beim Anschwimmen quer durchs Becken).
         * Fern: Rampe abbauen, flach schwimmen, kein Schnapp. */
        if (dist < 4200) { if (g->guard_t < 24) g->guard_t++; }
        else             { if (g->guard_t > 0)  g->guard_t--; }
        {   /* Mess-Telemetrie (Nutzer-Diagnose): alle 30 F in gator_boss.log */
            static FILE *s_gl = NULL; static int s_gc = 0;
            if (!s_gl) s_gl = fopen("gator_boss.log", "a");
            if (s_gl && (++s_gc % 30) == 0) {
                fprintf(s_gl, "GUARD dist=%d guard_t=%d pitch=%d arc=%d mo=%d af=%d pos=(%d,%d)\n",
                        (int)dist, (int)g->guard_t, (int)g->pitch_vz, (int)g->arc_vz,
                        (int)e->motion, (int)e->anim_frame, e->x, e->z);
                fflush(s_gl);
            }
        }
        g->pitch_vz = (int16_t)(-(350 * g->guard_t) / 24);
        g->arc_vz   = (int16_t)(-(220 * g->guard_t) / 24);
        /* Hochbiss im Schnapp-Takt: Clip 4, Fenster = gemessene Maul-offen-Phase. */
        if (dist < 4200 && e->hit_stun == 0 && e->motion != 4) {
            e->motion = 4; e->anim_frame = 0; g->bite_done = 0;
        }
        if (e->motion == 4) {
            e->anim_frame++;
            if (!g->bite_done && e->anim_frame >= 8 && e->anim_frame <= 20
                && pl->hit_react == 0 && dist < 3400) {   /* DESIGN: Kantenstand +
                                               * gehobener Kopf erreicht die Flaeche */
                re15_player_take_damage(pl, GB_BITE_TYPE, e->x, e->z);
                re15_player_knockdown_begin(re15_ai_facing_dir(e, pl));
                pl->hit_react |= 1;
                g->bite_done = 1;
                e->hit_stun = 0x64;
            }
            if (e->anim_frame > 42) { e->motion = 0; e->anim_frame = 0; }
        } else {
            e->anim_frame++;
        }
        break; }

    case GBP_CROSS_APPR:                      /* entfaellt (Rim-System) - Altzustand
                                               * sicher in die Verfolgung zurueck */
        g->phase = GBP_CHASE; g->arc_vz = 0; g->pitch_vz = 0;
        e->motion = 0; e->anim_frame = 0;
        break;

    case GBP_CROSS: {                         /* Punkt 8: Bogen ueber die Plattform */
        g->ct++;
        int32_t t = g->ct;                    /* 0..GB_CROSS_FRAMES */
        if (t >= (g->cframes ? g->cframes : GB_CROSS_FRAMES)) {
            g->phase = GBP_CHASE; g->arc_vz = 0; g->pitch_vz = 0;
            g->cross_cd = 300;               /* DESIGN: ~10 s Ring, dann darf er wieder drueber */
            e->y = GB_WATER_Y;
            e->motion = 0; e->anim_frame = 0;
            break;
        }
        /* Bahn: linear cx0->cx1; Hoehe + Kruemmung als Halbbogen sin(pi*t). */
        e->x = g->cx0 + (int32_t)((int64_t)(g->cx1 - g->cx0) * t
                                  / (g->cframes ? g->cframes : GB_CROSS_FRAMES));
        e->z = g->cz0 + (int32_t)((int64_t)(g->cz1 - g->cz0) * t
                                  / (g->cframes ? g->cframes : GB_CROSS_FRAMES));
        {
            /* sin(pi * t/N) ueber die Q12-Tabelle: Winkel 0..0x800 (halbe Periode). */
            /* Bogen nur ueber dem Insel-Abschnitt (t_ein..t_aus); die Wasser-
             * Endstuecke der Bahn werden flach geschwommen. */
            if (t <= g->t_ein || t >= g->t_aus) {
                e->y = GB_WATER_Y;
                g->arc_vz = 0; g->pitch_vz = 0;
            } else {
                int span = (int)g->t_aus - (int)g->t_ein;
                int ang = (int)((int64_t)(t - g->t_ein) * 0x800 / span);
                int sv  = re15_sin_q12(ang);               /* 0..4096..0 */
                int cv  = re15_cos_q12(ang);               /* 4096..0..-4096 */
                e->y = GB_WATER_Y - (int32_t)((int64_t)(RE15_GB_CROSS_HUB) * sv >> 12);
                g->arc_vz = (int16_t)((GB_ARC_VZ_MAX * sv) >> 12);
                g->pitch_vz = (int16_t)(-((GB_PITCH_MAX * cv) >> 12));
            }
        }
        /* Blick in Bahnrichtung (Engine-Peilung, Sofort-Snap) */
        re15_enemy_steer_point(e, g->cx1, g->cz1, 0x800);
        e->anim_frame++;
        /* Biss-Fenster auch auf der Plattform (Leon vertreiben): Reichweiten-Test,
         * EIN Biss pro Passage + byte-true Cooldown-Feld als Zweitsperre. */
        if (!g->bite_done && e->hit_stun == 0
            && pl->hit_react == 0 && dist <= re15_ai_contact_reach(e)
            && re15_ai_arc(e, pl, re15_ai_contact_reach(e), 0x400)) {
            re15_player_take_damage(pl, GB_BITE_TYPE, e->x, e->z);
            re15_player_knockdown_begin(re15_ai_facing_dir(e, pl));
            pl->hit_react |= 1;
            g->bite_done = 1;
            e->hit_stun = 0x64;
        }
        break; }

    case GBP_DIE:                             /* Todesrolle Clip 7, dann CORPSE */
        e->motion = 7;
        e->anim_frame++;
        g->timer++;
        g->arc_vz = 0;
        if (g->timer >= 165) {                /* Clip 7 = 165 Frames (Clip-Statistik) */
            g->phase = GBP_DEAD;
            e->state = 7; e->sub_state_2 = 0; e->sub_state_3 = 0;  /* byte-true CORPSE */
        }
        break;

    case GBP_DEAD:
        /* CORPSE laeuft in der byte-true KI (state 7) — nichts zu tun. */
        break;

    default:
        g->phase = GBP_LURK;
        break;
    }

    /* byte-true ACTIVE-Tail: Cooldowns jeden Frame herunterzaehlen (@0x8010c9d4-ca40). */
    if (e->hit_stun > 0) e->hit_stun--;

    /* KOERPERLAENGEN-PUSH (Nutzer 2026-09-10 "kann immer noch durchclippen"):
     * die byte-true Root-Ellipse (2200/800) deckt nur den Rumpf - das gerenderte
     * Tier ist 2/3 x 18313 = 12200 lang, Kopf und Schweif lagen AUSSERHALB. Vier
     * Zusatzkreise entlang der Blickachse (Bindpose-Enden: Kopf 2/3x8403=5600,
     * Schweif 2/3x9910=6600; Radien nach Koerperbreite): Leon prallt auf der
     * GANZEN Laenge ab. Y-Fenster wie der Body-Push (Zentrum auf Bodenniveau,
     * s. hit_offset_y im INIT): Leon OBEN auf dem Steg wird nicht geschoben. */
    if (g->phase != GBP_DIE && g->phase != GBP_DEAD
        && pl->hp >= 0 && !re15_player_is_grabbed()) {
        static const struct { int32_t ofs, r; } SEG[4] = {
            { 2600, 900 }, { 4600, 700 }, { -2600, 900 }, { -4800, 750 }
        };
        int32_t fc = re15_cos_q12((int)e->rot_y);
        int32_t fs = re15_sin_q12((int)e->rot_y);
        int32_t dy = pl->y - (e->y - GB_WATER_Y);  /* Push-Zentrum = e->y + 1200 */
        if (dy > -1500 && dy < 1500) {
            int si;
            for (si = 0; si < 4; si++) {
                int32_t cx = e->x + (int32_t)(((int64_t)fc * SEG[si].ofs) >> 12);
                int32_t cz = e->z - (int32_t)(((int64_t)fs * SEG[si].ofs) >> 12);
                int32_t dx = pl->x - cx, dz = pl->z - cz;
                int32_t R  = SEG[si].r + 450;      /* + Spieler-Radius 0x1c2 */
                int64_t d2; int32_t dd, pen;
                if (dx > R || dx < -R || dz > R || dz < -R) continue;
                d2 = (int64_t)dx * dx + (int64_t)dz * dz;
                if (d2 >= (int64_t)R * R) continue;
                dd = 1; while ((int64_t)dd * dd < d2) dd++;
                pen = R - dd;
                if (pen < 1) continue;
                pl->x += dx * pen / (dd + 1);
                pl->z += dz * pen / (dd + 1);
            }
        }
    }
}

/* ==== Punkt 8: Wirbelsaeulen-vz fuer den Skelett-Bogen ======================== *
 * Kopfkette (1,5,6) kippt zur Bahn hin, Schwanzkette (11,12,13) gegenlaeufig —
 * konstante Kruemmung je Gelenk formt den Halbbogen. Kiefer (7), Beine und
 * Schwanzspitze bleiben der Animation ueberlassen. */
int re15_gator_spine_arc_vz(const re15_actor_t *e, int bone)
{
    if (!re15_gator_boss_active(e)) return 0;
    const gb_state_t *g = &s_gb[(int)(e - g_actors)];
    if (g->arc_vz == 0 && g->pitch_vz == 0) return 0;
    /* Nutzer-Befund 2026-09-10: "Der Bogen muss sein - unten Kopf, Ruecken
     * oben, unten Schweif" - die alte Vorzeichenwahl schweifte BEIDE Enden
     * nach OBEN (U statt Bogen). Rz-Geometrie: +X-Kette (Kopf) haengt mit
     * az>0 ab (x'->+y = PSX-unten), -X-Kette (Schwanz) mit az<0. */
    switch (bone) {
    case 0:                            return  (int)g->pitch_vz; /* Root-Neigung der Bahn */
    case 1: case 5: case 6:            return  (int)g->arc_vz;   /* Kopf haengt drueben ab */
    case 11: case 12: case 13:         return -(int)g->arc_vz;   /* Schweif haengt diesseits */
    default:                           return 0;
    }
}

/* ==== Punkt 7: Spinnen-Wandflucht ============================================ *
 * Sobald der Gator Richtung Plattform kommt (CROSS ausgeloest), rennen die zwei
 * 0x25-Spinnen zur Ostwand und klettern sie hoch (Kamera-Cut 9 schaut von
 * Nordwest auf die Plattform — die Ostwand ist "die Wand dahinter"). */
int re15_gator_boss_spider_override(int slot)
{
    re15_actor_t *e = &g_actors[slot];
    if (e->type != 0x25u || ((g_current_room_id & 0xFFFEu) != 0x2090u)) return 0;
    /* Nur im BOSSKAMPF kapern: ohne aktiven 0x23er (z.B. RE15_KEIN_GATOR oder die
     * SCA-Testfixtures, unit_sca_wall_hit laedt 2090-Spinnen) laeuft die normale
     * Spinnen-KI unveraendert. */
    int gator = 0, flee = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == 0x23u) {
            gator = 1;
            if (s_gb[i].spider_flee) flee = 1;
        }
    if (!gator) return 0;
    if (!flee) {
        /* Punkt 1: SITZEN, bis der Gator Richtung Plattform kommt — die normale
         * Spinnen-KI liefe sofort von der Plattform (Sichtlauf 2026-09-10).
         * Anker: Position/Hoehe halten, ruhiger Idle-Zyklus. */
        if (e->state != 0 && e->state < 3 && e->hp > 0) {   /* nur LEBENDE ankern -
                                               * Nutzer-Marker: getoetete Spinne
                                               * hing mit hp=0 im Sitz-Anker */
            static const int32_t sitz[2][2] = { { -700, -17800 }, { 800, -14800 } };
            int which = (e->x > 0);
            e->x = sitz[which][0]; e->z = sitz[which][1];
            e->y = -1800; e->motion = 0x10; e->anim_frame++;
            return 1;
        }
        return 0;   /* INIT der Spinnen-KI noch durchlaufen lassen (HP/Clip-Setup) */
    }

    if (e->x < GB_WALL_X - 400) {             /* Phase 1: schnell zur Ostwand */
        re15_enemy_steer_point(e, GB_WALL_X, e->z, 0x80);
        re15_ai_advance(e, GB_SPID_RUN);
        e->motion = 2; e->anim_frame++;       /* Lauf-Zyklus der Spinnen-Bank */
    } else if (e->y > -5000) {                /* Phase 2: Wand hoch (Y negativ = oben) */
        e->y -= GB_SPID_CLIMB;
        e->anim_frame++;
    } else {
        e->flags |= 0x40;                     /* oben angekommen: inert parken */
    }
    return 1;
}
