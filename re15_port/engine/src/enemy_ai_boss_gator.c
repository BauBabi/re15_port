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
#include "re15_collision.h"  /* re15_collision_constrain + ensure_band (Push-Klemme) */

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
#define GB_RING_M        2600   /* Wegpunkt-Abstand vom Block: MUSS ausserhalb
                                 * der SCA-Klemmzone (Hitbox 2200) liegen - mit
                                 * 1200 lagen die Wand-Following-Ecken IN der
                                 * Klemme = unerreichbare Ziele = Haenger
                                 * (Nutzer + Session-Telemetrie 2026-09-10). */
#define GB_GPAT_M        1900   /* Belagerungs-Kantenabstand: > GB_KOERPER_M
                                 * (sonst ueberschreibt das Following das Ziel)
                                 * und > skalierte Klemmlinie ~1470. */
#define GB_KOERPER_M     1800   /* Weg-frei-Marge der Following-Tests = SKALIERTER
                                 * Koerperradius 1466 (byte-true Box @0x80118b98
                                 * r=2200 x GB_SCALE_Q12, s. Boss-INIT) + ~330.
                                 * Lehre der Nutzer-Marken 1+2 (2026-09-10): eine
                                 * Marge unter dem Koerperradius meldet "Sicht-
                                 * linie frei", wo der Rumpf laengst an der Kante
                                 * schliff (x klemmte konstant auf Kante-Radius). */
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
 * (-1800 = floor-Ebene 1 x byte-true Y=-1800*floor @0x8010c7a4) aufliegt.
 * GEMESSEN 2026-09-11 (PEAK-Hook, Bone-Welt-y am Bahn-Scheitel): der
 * EM23-Rig-URSPRUNG sitzt an der Koerper-UNTERSEITE (tiefster Bone -3481
 * bei Root -3192 = ALLE Bones ueber dem Root) - die alte Bindpose-Rechnung
 * ("Bauch 1392 unter Root") hatte die y-Richtung invertiert, der Koerper
 * schwebte 1392 ueberm Deck (Nutzer-Bild fehler/error.png). Unterseite ~=
 * Root: Peak = Deck -1800 - 100 Toleranz = -1900; Hub = |-1900-(-1200)|. */
#define RE15_GB_CROSS_HUB 700
#define GB_ARC_VZ_MAX     100   /* DESIGN (Nutzer 2026-09-11 "zu umschweifend,
                                 * muss flacher aussehen"): ~9 deg je Wirbelgelenk
                                 * statt 23 - der Koerper bleibt fast gerade */
#define GB_PITCH_MAX      140   /* DESIGN: ~12 deg Nase-Neigung statt 26 */
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
    /* ROUTEN-LATCH (Nutzer-Marker 2026-09-10 "bleibt haengen": die je Frame neu
     * bewertete Wegwahl konnte zwischen Rim-Anlauf und Ring-Ecke flattern -
     * sichtbares Vor-zurueck-Pendeln im Westkanal, Marke 3). Die Route wird
     * einmal gewaehlt und nur bei Zonenwechsel/Cooldown-Ablauf neu bewertet. */
    int8_t   route;                /* 0=keine, 1=WEST (um die Plattform), 2=RAMPE (queren) */
    int8_t   route_zwang;          /* 1 = Route wurde unter cross_cd>0 gewaehlt
                                    * (RAMPE gesperrt) -> bei cd-Ablauf neu bewerten
                                    * (die alte cd==1-Flanke verpuffte, wenn der
                                    * Frame in einer anderen Phase verging) */
    int8_t   zone_g, zone_l;       /* gemerkte Becken-Zonen (Neubewertungs-Trigger) */
    int8_t   maul_kontakt;         /* Frames seit die SCHNAUZE Leon schob (SEG-
                                    * Push ofs>0): wen sie schiebt, den darf sie
                                    * auch beissen (Nutzer 2026-09-11: er schob
                                    * Leon am 5750er-Gleichgewicht vor sich her,
                                    * maul~3150 blieb ewig ueber dem 2400er-Gate) */
    int8_t   dbg_zweig;            /* Telemetrie: aktiver CHASE-Zweig */
    int32_t  dbg_tx, dbg_tz;       /* Telemetrie: aktuelles Steuer-Ziel */
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

/* Distanz Leon <-> MAUL (Kopfpunkt = Zentrum + 2600 voraus, die Kopf-Segment-
 * Lage des Koerperlaengen-Push). GEMESSEN 2026-09-11 (BISS-Telemetrie): der
 * Push haelt Leon am Kopf-Segment - das alte Zentrum-Gate (contact_reach
 * 1917) war dadurch fast nie erfuellbar (Messung: Leon bei maul=864-1264
 * DIREKT vorm Maul, Zentrum-dist 2187-3574 > 1917, hr blieb 0;
 * Nutzer: "trifft mich quasi so gut wie nie"). */
static int32_t gb_maul_dist(const re15_actor_t *e, const re15_actor_t *pl)
{
    int32_t fc = re15_cos_q12((int)e->rot_y);
    int32_t fs = re15_sin_q12((int)e->rot_y);
    int32_t kx = e->x + (int32_t)(((int64_t)fc * 2600) >> 12);
    int32_t kz = e->z - (int32_t)(((int64_t)fs * 2600) >> 12);
    int64_t dx = pl->x - kx, dz = pl->z - kz;
    int64_t d2 = dx * dx + dz * dz;
    int32_t d = 0;
    while ((int64_t)d * d < d2 && d < 30000) d += 16;
    return d;
}

/* Insel-Wand zwischen Gator und Leon? (Marge 0 = die reine Wand) - ein Biss
 * DURCH die Plattform ist unmoeglich (Nutzer 2026-09-11: "direkt an der
 * Platform auf der anderen Seite ... probiert staendig die Angriffs-
 * animation aber trifft mich nicht"). */
static int gb_wand_dazwischen(const re15_actor_t *e, const re15_actor_t *pl)
{
    return gb_seg_hits_platform(e->x, e->z, pl->x, pl->z, 0)
        || gb_seg_hits_ramp(e->x, e->z, pl->x, pl->z, 0);
}

/* Weg-frei-Test gegen BEIDE Bloecke (Plattform UND Rampe) - die Rampe blockt
 * den Gator genauso (SCA Band 0); Weg-Tests, die nur die Plattform sahen,
 * liessen ihn an der Rampen-NORDKANTE kriechen (Nutzer 2026-09-11, 4. Fall:
 * z=-13063-Klemmlinie, Ziel suedlich hinter der Rampe). */
static int gb_seg_frei(int32_t x0, int32_t z0, int32_t x1, int32_t z1, int32_t m);

/* Die vier Plattform-Umlauf-Ecken (+GB_RING_M) - gemeinsame Wegpunkte fuer
 * Ring-Following und den zentralen Kanten-Umweg. */
static const int32_t gb_ecke[4][2] = {
    { GB_PLAT_X0 - GB_RING_M, GB_PLAT_Z0 - GB_RING_M },
    { GB_PLAT_X1 + GB_RING_M, GB_PLAT_Z0 - GB_RING_M },
    { GB_PLAT_X1 + GB_RING_M, GB_PLAT_Z1 + GB_RING_M },
    { GB_PLAT_X0 - GB_RING_M, GB_PLAT_Z1 + GB_RING_M },
};

/* Ring-Wegpunkt: die vier Block-Ecken (+Marge); waehle die Ecke, die vom Gator aus
 * sichtbar ist (Strecke frei) und den Winkelumweg zu Leon minimiert. */
static int gb_seg_frei(int32_t x0, int32_t z0, int32_t x1, int32_t z1, int32_t m)
{
    return !gb_seg_hits_platform(x0, z0, x1, z1, m)
        && !gb_seg_hits_ramp(x0, z0, x1, z1, m);
}

static void gb_ring_target(const re15_actor_t *e, const re15_actor_t *pl,
                           int32_t *tx, int32_t *tz)
{
    const int32_t (*C)[2] = gb_ecke;   /* gemeinsame Umlauf-Ecken (s. gb_ecke) */
    /* WAND-FOLLOWING (Nutzer 2026-09-10: "ganz ans Ende der Wand laufen und
     * erst dann drehen"): statt der global guenstigsten Ecke die Ecken-FOLGE
     * in der kuerzeren Umlaufrichtung ablaufen - naechste Ecke auf dem Weg
     * als Ziel; Best-Cost steuerte in Wandnaehe diagonal in die Klemme. */
    int gs, ps, i;
    /* Seiten-Index 0=N,1=O,2=S,3=W (zwischen den Ecken C[0..3]) */
    #define GB_SEITE(px, pz)         ((pz) <= GB_PLAT_Z0 ? 0 : (pz) >= GB_PLAT_Z1 ? 2 : (px) >= GB_PLAT_X1 ? 1 : 3)
    gs = GB_SEITE(e->x, e->z);
    ps = GB_SEITE(pl->x, pl->z);
    if (gs == ps &&
        !gb_seg_hits_platform(e->x, e->z, pl->x, pl->z, GB_KOERPER_M)) {
        *tx = pl->x; *tz = pl->z; return;
    }
    /* kuerzere Drehrichtung: Seitenabstand im Uhrzeigersinn vs. gegen ihn */
    {
        int cw = (ps - gs + 4) & 3, ecke;
        if (gs == ps) {
            /* gleiche Seite, aber der Weg schert im Kanten-Schatten (Nutzer-
             * Marke 1, 2026-09-11: Gator im SW-Schatten, Leon Ostwand -
             * "direkt" kroch mit 2/F an der Westkanten-Klemmlinie): erst
             * die LEON-naehere Ecke der eigenen Seite anlaufen; der
             * Schatten-Fallback unten korrigiert notfalls auf die nahe. */
            int e1 = gs, e2 = (gs + 1) & 3;
            int64_t a_x = pl->x - C[e1][0], a_z = pl->z - C[e1][1];
            int64_t b_x = pl->x - C[e2][0], b_z = pl->z - C[e2][1];
            cw = 9;   /* markiert: ecke unten schon gesetzt */
            ecke = (a_x * a_x + a_z * a_z <= b_x * b_x + b_z * b_z) ? e1 : e2;
        } else
        /* Ziel-Ecke = die in Laufrichtung naechste Ecke der eigenen Seite:
         * Uhrzeigersinn: Seite k endet an Ecke (k+1)&3; gegen ihn: an Ecke k. */
        ecke = (cw == 9) ? ecke : (cw <= 2) ? ((gs + 1) & 3) : gs;
        /* SCHATTEN-FALLBACK (Nutzer-Marken 1+2, 2026-09-10): steht der Gator
         * im Radius-Schatten der Kante (Diagonal-Feld), schert die Luftlinie
         * zur fernen Ecke am Koerperradius entlang - die SCA-Klemme frisst
         * die Hauptkomponente und er kriecht (2/F statt 48/F). Dann erst die
         * NAEHERE der beiden Seiten-Ecken anlaufen (fuehrt von der Kante weg). */
        if (gb_seg_hits_platform(e->x, e->z, C[ecke][0], C[ecke][1], GB_KOERPER_M)) {
            int alt2 = (ecke == gs) ? ((gs + 1) & 3) : gs;
            int64_t da_x = e->x - C[ecke][0], da_z = e->z - C[ecke][1];
            int64_t db_x = e->x - C[alt2][0], db_z = e->z - C[alt2][1];
            if (db_x * db_x + db_z * db_z < da_x * da_x + da_z * da_z)
                ecke = alt2;
        }
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
/* Liegt (px,pz) ueber der RAMPE (+300)? Positions-Wahrheit fuer den Bogen.
 * NUR die Rampe: geklettert wird ausschliesslich ueber den kleinen Vorsprung
 * (Nutzer: "nur HIER sollte er ... druebersteigen" + 2026-09-10 "klettert
 * irgendwo anders rum als auf den kleinen Vorsprung"). */
static int gb_ueber_rampe(int32_t px, int32_t pz)
{
    return (px >= GB_RAMP_X0 - 300 && px <= GB_RAMP_X1 + 300 &&
            pz >= GB_RAMP_Z0 - 300 && pz <= GB_RAMP_Z1 + 300);
}

/* Becken-Zone aus den drei sce-8-Wasserzonen des RDT (@0x09D6/@0x09EA/@0x09FE:
 * Suedbecken z[-27000..-18100], Nordbecken z[-14500..-5400], Westkanal
 * x[-8900..-1700] z[-18100..-14500]): 0=Sued, 1=Nord, 2=Mittelstreifen (dort
 * ist nur der Westkanal Wasser - Plattform und Rampe sind SCA-Bloecke).
 * Sued<->Nord geht NUR westlich um die Plattform oder kletternd ueber die
 * Rampe; genau diese Wahl trifft die CHASE-Routenlogik. */
static int gb_zone(int32_t z)
{
    return (z <= GB_RAMP_Z0) ? 0 : (z >= GB_RAMP_Z1) ? 1 : 2;
}

static int32_t gb_iabs(int32_t v) { return v < 0 ? -v : v; }

static void gb_rim_point(int32_t px, int32_t pz, int32_t *ox, int32_t *oz)
{
    int32_t x = px, z = pz;
    int it;
    for (it = 0; it < 3; it++) {
        gb_rim_out(&x, &z, GB_PLAT_X0, GB_PLAT_Z0, GB_PLAT_X1, GB_PLAT_Z1);
        gb_rim_out(&x, &z, GB_RAMP_X0, GB_RAMP_Z0, GB_RAMP_X1, GB_RAMP_Z1);
    }
    /* In die fuer den GATOR ERREICHBARE Pool-Flaeche klemmen: Wandabstand =
     * SCA-Klemmradius 2200 + Marge (GEMESSEN 2026-09-10: mit 500er-Rand lag
     * das Patrouillen-Ziel (-8400,-26500) in der NW-Poolecke JENSEITS beider
     * Klemmgrenzen (-6700/-24800) - er schob ewig in die Ecke = der Haenger,
     * dieselbe Fehlerklasse wie die alten 1200er-Ring-Ecken). */
    if (x < -8900 + 2500) x = -8900 + 2500;
    if (x >  7200 - 2500) x =  7200 - 2500;
    if (z < -27000 + 2500) z = -27000 + 2500;
    if (z >  -5400 - 2500) z =  -5400 - 2500;
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

/* Ueberquerung: feste Nord-Sued-Bahn ueber die RAMPE (den kleinen Vorsprung)
 * am aktuellen x des Gators - NUR dort wird geklettert (Nutzer: "nur HIER
 * sollte er ... druebersteigen"; 2026-09-10: "klettert irgendwo anders rum
 * als auf den kleinen Vorsprung" beendete die freien rim->rim-Bahnen, die
 * diagonal ueber die Plattform-Mitte liefen, Marke 4: y=-3192 mitten auf dem
 * Block). Der Aufrufer startet die Bahn aus Kanten-Naehe (Anlaufpunkt). */
static void gb_cross_begin(re15_actor_t *e, gb_state_t *g, int von_sued)
{
    int32_t cx = e->x;
    if (cx < GB_RAMP_X0 + GB_RING_M) cx = GB_RAMP_X0 + GB_RING_M;
    if (cx > GB_RAMP_X1 - 700)       cx = GB_RAMP_X1 - 700;
    /* Westgrenze = X0+GB_RING_M (4450), NICHT +700: die Plattform-Ostkante
     * (1850) wirft mit dem 2200er-Koerperradius (@0x80118b98) einen
     * Klemm-Schatten bis x~3950 - ein Anlaufpunkt darunter ist mit dem
     * Koerper UNERREICHBAR (Nutzer-Marke 3, 2026-09-10: er stand bei
     * (2522,-22170) exakt auf der Suedkanten-Klemmlinie, Ziel (2550,-20700),
     * 1470 vor dem 1400er-Gate - fuer immer). */
    g->cx0 = cx; g->cx1 = cx;
    if (von_sued) { g->cz0 = GB_RAMP_Z0 - GB_BAHN_M; g->cz1 = GB_RAMP_Z1 + GB_BAHN_M; }
    else          { g->cz0 = GB_RAMP_Z1 + GB_BAHN_M; g->cz1 = GB_RAMP_Z0 - GB_BAHN_M; }
    {
        int32_t len = (GB_RAMP_Z1 - GB_RAMP_Z0) + 2 * GB_BAHN_M;   /* 8800 */
        g->cframes = (int16_t)(len / 40);
        if (g->cframes < 90)  g->cframes = 90;
        if (g->cframes > 240) g->cframes = 240;
    }
    /* Bogen-Fenster: nur der Rampen-Abschnitt der Bahn traegt den Hub. */
    {
        int32_t t0, t1;
        if (gb_seg_rect_span(g->cx0, g->cz0, g->cx1, g->cz1,
                             GB_RAMP_X0, GB_RAMP_Z0, GB_RAMP_X1, GB_RAMP_Z1,
                             300, &t0, &t1)) {
            g->t_ein = (int16_t)(((int32_t)g->cframes * t0) >> 12);
            g->t_aus = (int16_t)(((int32_t)g->cframes * t1) >> 12);
        } else { g->t_ein = 0; g->t_aus = (int16_t)g->cframes; } /* Sicherheitsnetz */
        if (g->t_aus <= g->t_ein) g->t_aus = (int16_t)(g->t_ein + 1);
    }
    g->ct = 0; g->bite_done = 0;
    g->phase = GBP_CROSS;
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
        /* KOLLISION IM MASSSTAB DES MODELLS (Nutzer 2026-09-10: "musst du ihn
         * halt noch ein wenig kleiner machen"): die byte-true Box @0x80118b98
         * (r=2200, rz=800, h=720) gilt dem Full-Size-Alligator; der Boss
         * laeuft optisch mit GB_SCALE_Q12 (2/3). Ungeskaliert kollidierte er
         * wie in Originalgroesse und blieb an Kanten haengen, wo er optisch
         * laengst vorbeipasst (alle Haenger-Marken 2026-09-10). Idempotent
         * ueber den >2000-Wachtest (INIT laeuft je Aktor-Leben einmal). */
        if (e->hit_radius_min > 2000) {
            e->hit_radius_min = (uint16_t)(((uint32_t)e->hit_radius_min * GB_SCALE_Q12) >> 12);
            e->hit_radius_max = (uint16_t)(((uint32_t)e->hit_radius_max * GB_SCALE_Q12) >> 12);
            e->hit_height     = (uint16_t)(((uint32_t)e->hit_height     * GB_SCALE_Q12) >> 12);
        }
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
                      if (*tv == '2') gb_cross_begin(e, g, gb_zone(e->z) == 0);
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
    if (g->maul_kontakt > 0) g->maul_kontakt--;
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

    case GBP_CHASE: {                         /* Punkt 6: Verfolgung im Wasser */
        int player_on_platform =                    /* begehbare Flaeche OBEN =
                                                     * Block UND Ost-Rampe (Nutzer-
                                                     * Marker: Leon auf der Rampe
                                                     * bei 2318,-1800). y-CHECK
                                                     * (Nutzer-Session 2026-09-10:
                                                     * Leon WATET bei y=0 im Wasser
                                                     * neben der Rampe - x/z allein
                                                     * loeste die Belagerung aus,
                                                     * waehrend er unten stand;
                                                     * OBEN hat er y=-1800). */
            (pl->y < -900) &&
            ((pl->x >= GB_PLAT_X0 && pl->x <= GB_PLAT_X1 &&
              pl->z >= GB_PLAT_Z0 && pl->z <= GB_PLAT_Z1) ||
             (pl->x >= GB_RAMP_X0 && pl->x <= GB_RAMP_X1 &&
              pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1));
        if (g->cross_cd > 0) g->cross_cd--;
        {   const char *tv6 = getenv("RE15_GB_TEST");
            if (tv6 && *tv6 == '6') player_on_platform = 1;
        }
        if (player_on_platform) {
            /* Nutzer 2026-09-10: solange Leon OBEN steht -> BELAGERN und nach
             * oben schnappen; geklettert wird erst, wenn er heruntergesprungen
             * ist (dann faellt player_on_platform und die Routenwahl unten
             * fuehrt ihn herum oder ueber die Rampe). */
            g->phase = GBP_GUARD; g->guard_t = 0; g->bite_done = 0;
            e->motion = 0; e->anim_frame = 0;
            break;
        }

        /* ROUTENWAHL ueber die Becken-Zonen (ersetzt rim-Anlauf + Ring-Mix,
         * die in der Nutzer-Session 2026-09-10 frameweise gegeneinander
         * flatterten: 200-F-Kriechstand im Nordbecken, Marken 1+2, und
         * Pendeln im Westkanal, Marke 3). Sued<->Nord trennt die Plattform;
         * es gibt GENAU zwei Wege: WEST um die Plattform-Ecken oder KLETTERN
         * ueber die Rampe (den kleinen Vorsprung - NUR dort, Nutzer-Wort).
         * Die Route wird gelatcht und nur neu bewertet, wenn eine Zone
         * wechselt oder der Kletter-Cooldown ablaeuft. */
        int32_t tx = pl->x, tz = pl->z;
        {
            int zg = gb_zone(e->z), zl = gb_zone(pl->z);
            g->dbg_zweig = 0;
            if (zg != zl && zg != 2 && zl != 2) {
                /* Sued<->Nord: Route waehlen/halten */
                int32_t eckz_g = (zg == 0) ? (GB_PLAT_Z0 - GB_RING_M) : (GB_PLAT_Z1 + GB_RING_M);
                int32_t eckz_l = (zg == 0) ? (GB_PLAT_Z1 + GB_RING_M) : (GB_PLAT_Z0 - GB_RING_M);
                int32_t eckx   = GB_PLAT_X0 - GB_RING_M;      /* Westumlauf-Ecken */
                int32_t rx     = e->x;                        /* Rampen-Anlauf-x */
                if (rx < GB_RAMP_X0 + GB_RING_M) rx = GB_RAMP_X0 + GB_RING_M;
                if (rx > GB_RAMP_X1 - 700)       rx = GB_RAMP_X1 - 700;
                int32_t rz_ein = (zg == 0) ? (GB_RAMP_Z0 - GB_BAHN_M) : (GB_RAMP_Z1 + GB_BAHN_M);
                int32_t rz_aus = (zg == 0) ? (GB_RAMP_Z1 + GB_BAHN_M) : (GB_RAMP_Z0 - GB_BAHN_M);
                if (g->route == 0 || zg != g->zone_g || zl != g->zone_l
                    || (g->route_zwang && g->cross_cd == 0)) {
                    /* Kostenvergleich (grobe Manhattan-Summen reichen zur Wahl) */
                    int32_t kw = gb_iabs(e->x - eckx) + gb_iabs(e->z - eckz_g)
                               + gb_iabs(eckz_l - eckz_g)
                               + gb_iabs(pl->x - eckx) + gb_iabs(pl->z - eckz_l);
                    int32_t kr = gb_iabs(e->x - rx) + gb_iabs(e->z - rz_ein)
                               + gb_iabs(rz_aus - rz_ein)
                               + gb_iabs(pl->x - rx) + gb_iabs(pl->z - rz_aus);
                    g->route = (g->cross_cd == 0 && kr < kw) ? 2 : 1;
                    g->route_zwang = (int8_t)(g->cross_cd > 0);
                    g->zone_g = (int8_t)zg; g->zone_l = (int8_t)zl;
                }
                if (g->route == 2 && g->cross_cd == 0) {
                    int64_t adx = e->x - rx, adz = e->z - rz_ein;
                    if (adx * adx + adz * adz < (int64_t)1400 * 1400) {
                        gb_cross_begin(e, g, zg == 0);
                        break;
                    }
                    tx = rx; tz = rz_ein; g->dbg_zweig = 2;
                } else {
                    /* WEST: naechsten noch noetigen Wegpunkt ansteuern */
                    if (gb_seg_frei(e->x, e->z, pl->x, pl->z, GB_KOERPER_M))
                        { g->dbg_zweig = 1; }                     /* frei: direkt */
                    else if (gb_seg_frei(e->x, e->z, eckx, eckz_l, GB_KOERPER_M))
                        { tx = eckx; tz = eckz_l; g->dbg_zweig = 3; }
                    else { tx = eckx; tz = eckz_g; g->dbg_zweig = 4; }
                }
            } else {
                /* verbunden (gleiche Zone oder Westkanal): direkt zu Leon;
                 * streift die Sichtlinie den Plattform-Sporn oder die
                 * Rampen-Westkante, um die Ecke fuehren. */
                if (gb_seg_hits_platform(e->x, e->z, pl->x, pl->z, GB_KOERPER_M))
                    { gb_ring_target(e, pl, &tx, &tz); g->dbg_zweig = 5; }
                else if (gb_seg_hits_ramp(e->x, e->z, pl->x, pl->z, GB_KOERPER_M)) {
                    /* westlich um die Rampe = westlich um die PLATTFORM (die
                     * Rampe schliesst an sie an; eine "Rampen-Westecke" laege
                     * im Radius-Schatten der Plattform-Suedkante). */
                    tx = GB_PLAT_X0 - GB_RING_M;
                    tz = (gb_zone(e->z) == 0)
                           ? (GB_PLAT_Z0 - GB_RING_M) : (GB_PLAT_Z1 + GB_RING_M);
                    g->dbg_zweig = 6;
                }
                g->route = 0;                          /* Latch loesen */
                g->zone_g = (int8_t)zg; g->zone_l = (int8_t)zl;
            }
        }
        {   /* FORTSCHRITTS-WAECHTER (Selbstheilung; 4. Klemm-Fall in Folge,
             * 2026-09-11): macht der Gator ueber 90 F kaum Strecke, obwohl
             * das Ziel fern ist, erzwingt er 60 F lang die naechstgelegene
             * FREIE Umlauf-Ecke und setzt das Routen-Latch zurueck - JEDER
             * kuenftige Geometrie-Sonderfall heilt sich damit sichtbar
             * selbst, statt zum Dauerstand zu werden. */
            static int32_t s_fw_x = 0, s_fw_z = 0;
            static int s_fw_t = 0, s_fw_not = 0;
            int64_t zdx = (int64_t)tx - e->x, zdz = (int64_t)tz - e->z;
            if (s_fw_not > 0) {
                s_fw_not--;
                tx = g->dbg_tx; tz = g->dbg_tz;   /* Not-Ecke halten */
            } else if (++s_fw_t >= 90) {
                int64_t mdx = (int64_t)e->x - s_fw_x, mdz = (int64_t)e->z - s_fw_z;
                if (mdx * mdx + mdz * mdz < (int64_t)300 * 300
                    && zdx * zdx + zdz * zdz > (int64_t)1500 * 1500) {
                    int ci, bi = -1; int64_t best = 0;
                    for (ci = 0; ci < 4; ci++) {
                        int64_t k = (int64_t)gb_iabs(e->x - gb_ecke[ci][0])
                                  + gb_iabs(e->z - gb_ecke[ci][1]);
                        if (!gb_seg_frei(e->x, e->z, gb_ecke[ci][0], gb_ecke[ci][1],
                                         GB_KOERPER_M)) k += 100000;
                        if (bi < 0 || k < best) { best = k; bi = ci; }
                    }
                    tx = gb_ecke[bi][0]; tz = gb_ecke[bi][1];
                    s_fw_not = 60; g->route = 0;
                    {   static FILE *s_fl = NULL;
                        if (!s_fl) s_fl = fopen("gator_boss.log", "a");
                        if (s_fl) { fprintf(s_fl, "NOTFREI pos=(%d,%d) ecke=%d\n",
                                            e->x, e->z, bi); fflush(s_fl); }
                    }
                }
                s_fw_t = 0; s_fw_x = e->x; s_fw_z = e->z;
            }
        }
        /* ZENTRALER KANTEN-UMWEG (Nutzer-Marke 2026-09-11, dritter Fall
         * derselben Klasse: der Rampen-ANLAUF steuerte blind auf
         * (4450,-20700), waehrend der Gator westlich der Plattform im
         * Koerper-Schatten stand - Klemm-Kriechen auf x=-3137). Gilt fuer
         * JEDEN Zweig: schert die Luftlinie zum Steuerziel im Schatten der
         * Plattform, erst zur guenstigsten FREIEN Umlauf-Ecke; ist keine
         * frei (Start selbst im Schatten-Band), zur naechstgelegenen. */
        if (!gb_seg_frei(e->x, e->z, tx, tz, GB_KOERPER_M)) {
            int ci, bi = -1; int64_t best = 0;
            for (ci = 0; ci < 4; ci++) {
                int64_t k;
                if (!gb_seg_frei(e->x, e->z, gb_ecke[ci][0], gb_ecke[ci][1],
                                 GB_KOERPER_M)) continue;
                k = (int64_t)gb_iabs(e->x - gb_ecke[ci][0]) + gb_iabs(e->z - gb_ecke[ci][1])
                  + gb_iabs(tx - gb_ecke[ci][0]) + gb_iabs(tz - gb_ecke[ci][1]);
                if (bi < 0 || k < best) { best = k; bi = ci; }
            }
            if (bi < 0)
                for (ci = 0; ci < 4; ci++) {
                    int64_t k = (int64_t)gb_iabs(e->x - gb_ecke[ci][0])
                              + gb_iabs(e->z - gb_ecke[ci][1]);
                    if (bi < 0 || k < best) { best = k; bi = ci; }
                }
            tx = gb_ecke[bi][0]; tz = gb_ecke[bi][1];
            g->dbg_zweig = (int8_t)(10 + bi);   /* Telemetrie: Umweg-Ecke */
        }
        /* Steuerziel in die vom KOERPER-ZENTRUM erreichbare Flaeche klemmen
         * (Raum-SCA x[-8900..7200] z[-27000..-5400], Klemmlinie = Kante +
         * Radius 2200; GEMESSEN -24829 an der Suedwand): steht Leon im
         * Wand-Schatten (z.B. Tuernische am Suedufer), schliff der Gator
         * dauerhaft an der Wand (Repro 2026-09-10: 10 Stillstands-Ticks
         * bei z=-24829, +-350-Slew-Pendel). Am geklemmten Punkt lauert er
         * ruhig, bis Leon wieder erreichbar ist. */
        if (tx < -8900 + 1600)  tx = -8900 + 1600;
        if (tx >  7200 - 1600)  tx =  7200 - 1600;
        if (tz < -27000 + 1600) tz = -27000 + 1600;
        if (tz >  -5400 - 1600) tz =  -5400 - 1600;
        g->dbg_tx = tx; g->dbg_tz = tz;
        {   /* Dreh-Telemetrie (Nutzer 2026-09-11: "dreht fast immer erst in
             * die entgegengesetzte Richtung bevor er Richtung Platform
             * dreht"): bei jedem Zweig-/Routenwechsel 48 F Drehverlauf. */
            static FILE *s_dl = NULL;
            static int8_t s_zw_alt = -99, s_rt_alt = -99;
            static int s_dreh_n = 0;
            if (!s_dl) s_dl = fopen("gator_boss.log", "a");
            if (g->dbg_zweig != s_zw_alt || g->route != s_rt_alt) {
                s_zw_alt = g->dbg_zweig; s_rt_alt = g->route; s_dreh_n = 48;
                if (s_dl) { fprintf(s_dl, "WECHSEL zw=%d rt=%d\n",
                                    (int)g->dbg_zweig, (int)g->route); fflush(s_dl); }
            }
            if (s_dreh_n > 0 && s_dl && (s_dreh_n-- & 3) == 0) {
                int soll = ((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff;
                int dlt = ((soll - (int)e->rot_y + 0x800) & 0x0fff) - 0x800;
                fprintf(s_dl, "DREH rot=%d soll=%d dlt=%d pos=(%d,%d) ziel=(%d,%d)\n",
                        (int)e->rot_y & 0x0fff, soll, dlt, e->x, e->z, tx, tz);
                fflush(s_dl);
            }
        }
        {
            /* WENDE STATT BOGEN (Nutzer 2026-09-11: "dreht fast immer erst
             * in die entgegengesetzte Richtung bevor er Richtung Platform
             * dreht"): der byte-true B[4]-Slew (6..37/F) braucht fuer 180
             * Grad 55-340 Frames - da advance stur vorwaerts schwimmt, zog
             * jeder harte Zielwechsel einen sichtbaren Bogen in die ALTE
             * Richtung. DESIGN: ab ~67 Grad Winkelfehler (0x300) dreht er
             * auf der Stelle (0x38/F = 180 Grad in ~37 F) und schwimmt erst
             * wieder an, wenn die Nase zum Ziel zeigt. */
            int soll = ((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff;
            int dlt  = ((soll - (int)e->rot_y + 0x800) & 0x0fff) - 0x800;
            if (dlt > 0x300 || dlt < -0x300) {
                re15_enemy_steer_point(e, tx, tz, 0x38);
                e->y = GB_WATER_Y;
                if (e->motion != 0) { e->motion = 0; e->anim_frame = 0; }
                e->anim_frame++;
                break;
            }
            re15_enemy_steer_point(e, tx, tz, (re15_engine_rand8() & 0x1f) + 6);
        }
        re15_ai_advance(e, GB_SWIM_SPEED);
        e->y = GB_WATER_Y;
        if (e->motion != 0) { e->motion = 0; e->anim_frame = 0; }
        e->anim_frame++;

        /* Biss-Commit: byte-true Reichweiten-Test, Winkel-Gate auf 0x400
         * geoeffnet + Nah-Trigger (Session-Telemetrie 2026-09-10: er stand
         * 3 s reglos bei dist=623 vor Leon - das enge 0x180-Gate liess den
         * Schnapp nie zu, wenn Leon seitlich in der Nische stand). */
        if (pl->hit_react == 0 && e->hit_stun == 0
            && !gb_wand_dazwischen(e, pl)
            && (dist < 2600 || gb_maul_dist(e, pl) < 2400 || g->maul_kontakt > 0)) {
            /* Commit-Gate GEMESSEN 2026-09-11: der alte 0x1770er-Sichtkegel
             * (6000) startete Leer-Schnapps, deren Maul Leon im 45-F-Clip
             * nie erreichte (Messung: maul 3088->2624, kein Treffer);
             * maul<2400 = Fenster-Ankunft bei ~40/F Lunge-Vortrieb. */
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
        if (gb_maul_dist(e, pl) > 1400) re15_ai_advance(e, GB_LUNGE_SPEED);
        e->y = GB_WATER_Y;
        {   /* Mess-Telemetrie Biss-Fenster (Nutzer 2026-09-11: "trifft mich
             * quasi so gut wie nie") */
            static FILE *s_bl = NULL;
            if (!s_bl) s_bl = fopen("gator_boss.log", "a");
            if (s_bl && e->anim_frame >= 8 && e->anim_frame <= 20) {
                fprintf(s_bl, "BISS af=%d dist=%d maul=%d wand=%d hr=%d\n",
                        (int)e->anim_frame, (int)dist, gb_maul_dist(e, pl),
                        gb_wand_dazwischen(e, pl), (int)pl->hit_react);
                fflush(s_bl);
            }
        }
        if (!g->bite_done
            && e->anim_frame >= 8 && e->anim_frame <= 20   /* Maul-offen-Fenster der
                                                            * gemessenen Clip-4-Kurve */
            && pl->hit_react == 0
            && (gb_maul_dist(e, pl) <= 1500 || g->maul_kontakt > 0)
            && !gb_wand_dazwischen(e, pl)) {
            /* Treffer = MAUL-Kontakt (Kopf-Segment 900 + Spieler 450 + 150
             * Puffer); das alte Zentrum-Gate s. gb_maul_dist-Messnotiz. */
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
        int oben = (pl->y < -900) &&          /* y-Check wie im CHASE: watender
                                               * Leon (y=0) ist NICHT oben */
                   ((pl->x >= GB_PLAT_X0 && pl->x <= GB_PLAT_X1 &&
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
        {   /* PATROUILLE statt Erstarren (Nutzer "bleibt haengen": stand Leon
             * oben ausser Reichweite, drueckte steer ihn nur in die Wand und er
             * stand regungslos). Ziel = der Leon-naechste RANDPUNKT der Insel -
             * er schwimmt sichtbar an der Kante unter Leon her. */
            int32_t gx, gz;
            int64_t gdx, gdz;
            /* Randpunkt auf der EIGENEN Seite konstruieren (Nutzer-Marken
             * F2579-F2809, 2026-09-10: Leon oben auf der Rampe, der Gator
             * kreiste ewig im Nordbecken an der Plattform-NO-Ecke - der
             * gb_rim_point/ring_target-Weg fuehrte um die PLATTFORM, aber um
             * die RAMPE gibt es keinen Ostumlauf. Belagert wird die Kante
             * der Gator-Zone unter Leon; Kantenabstand GB_BAHN_M 2600:
             * ausserhalb der Klemm-Box (Kante+2170, @0x80118b98-Radius) UND
             * ausserhalb der 2500er-Following-Marge - ein naeheres Ziel (2300)
             * wurde vom Following-Zweig sofort wieder durch eine Ecke ersetzt
             * (GB_TEST=6-Lauf: GPAT pendelte an der Westecke). Hochbiss-
             * Reichweite bleibt unter dem 4200er-Aufricht-Gate. */
            {
                int zg2 = gb_zone(e->z);
                int auf_rampe = (pl->x >= GB_RAMP_X0);
                if (zg2 == 2) {               /* Westkanal: Plattform-Westkante */
                    gx = GB_PLAT_X0 - GB_GPAT_M;
                    gz = pl->z;
                    if (gz < GB_PLAT_Z0) gz = GB_PLAT_Z0;
                    if (gz > GB_PLAT_Z1) gz = GB_PLAT_Z1;
                } else if (auf_rampe) {       /* Rampen-Kante der eigenen Seite */
                    gx = pl->x;
                    if (gx < GB_RAMP_X0 + GB_RING_M) gx = GB_RAMP_X0 + GB_RING_M;
                    if (gx > GB_RAMP_X1 - 700)       gx = GB_RAMP_X1 - 700;
                    gz = (zg2 == 0) ? (GB_RAMP_Z0 - GB_GPAT_M) : (GB_RAMP_Z1 + GB_GPAT_M);
                } else {                      /* Plattform-Kante der eigenen Seite */
                    gx = pl->x;
                    if (gx < GB_PLAT_X0) gx = GB_PLAT_X0;
                    if (gx > GB_PLAT_X1) gx = GB_PLAT_X1;
                    gz = (zg2 == 0) ? (GB_PLAT_Z0 - GB_GPAT_M) : (GB_PLAT_Z1 + GB_GPAT_M);
                }
                {   /* Kanten-Pendel: steht Leon still (und damit das Ziel),
                     * patrouilliert er sichtbar laengs der Kante statt zu
                     * erstarren (+-1500 alle 90 F, Timer laeuft nur nahe
                     * am Ziel; laengs = x an Sued-/Nordkante, z am Kanal). */
                    static int s_bt = 0; static int s_bdir = 1;
                    int64_t bdx = e->x - gx, bdz = e->z - gz;
                    if (bdx * bdx + bdz * bdz < (int64_t)800 * 800)
                        if (++s_bt >= 90) { s_bt = 0; s_bdir = -s_bdir; }
                    if (zg2 == 2) {
                        gz += s_bdir * 1500;
                        if (gz < GB_PLAT_Z0) gz = GB_PLAT_Z0;
                        if (gz > GB_PLAT_Z1) gz = GB_PLAT_Z1;
                    } else {
                        gx += s_bdir * 1500;
                        if (auf_rampe) {
                            if (gx < GB_RAMP_X0 + GB_RING_M) gx = GB_RAMP_X0 + GB_RING_M;
                            if (gx > GB_RAMP_X1 - 700)       gx = GB_RAMP_X1 - 700;
                        } else {
                            if (gx < GB_PLAT_X0) gx = GB_PLAT_X0;
                            if (gx > GB_PLAT_X1) gx = GB_PLAT_X1;
                        }
                    }
                }
            }
            /* Liegt die Insel zwischen Gator und Patrouillenziel, fuehrt das
             * Wand-Following herum (Nutzer-Session 2026-09-10: er stand 3 s
             * noerdlich der Rampe und schob stur gegen sie, Ziel suedlich). */
            if (gb_seg_hits_platform(e->x, e->z, gx, gz, GB_KOERPER_M)
                || gb_seg_hits_ramp(e->x, e->z, gx, gz, GB_KOERPER_M)) {
                gb_ring_target(e, pl, &gx, &gz);
                /* SACKGASSEN-PENDEL (Telemetrie: 3-s-Staende an der NO-Ecke -
                 * die T-Geometrie hat ostseitig keinen Umlauf): ist auch das
                 * Following-Ziel schon erreicht, patrouilliere sichtbar an
                 * der Kante (Ziel alle 90 F um +-1500 laengs versetzt). */
                {
                    static int s_pt = 0; static int s_pdir = 1;
                    int64_t rdx = e->x - gx, rdz = e->z - gz;
                    if (rdx * rdx + rdz * rdz < (int64_t)800 * 800) {
                        if (++s_pt >= 90) { s_pt = 0; s_pdir = -s_pdir; }
                        if (gx >= GB_PLAT_X1 || gx <= GB_PLAT_X0) gz += s_pdir * 1500;
                        else                                      gx += s_pdir * 1500;
                    }
                }
            }
            gdx = e->x - gx; gdz = e->z - gz;
            re15_enemy_steer_point(e, gx, gz, 0x40);
            {   /* Wende statt Bogen auch hier (s. CHASE-Steer-Notiz) */
                int gsoll = ((int)re15_atan2_q12(gz - e->z, gx - e->x) - 0x400) & 0x0fff;
                int gdlt  = ((gsoll - (int)e->rot_y + 0x800) & 0x0fff) - 0x800;
                if ((gdlt <= 0x300 && gdlt >= -0x300)
                    && gdx * gdx + gdz * gdz > (int64_t)600 * 600)
                    re15_ai_advance(e, GB_SWIM_SPEED);
            }
            {   /* Feindiagnose Haenger (temporaer aussagekraeftig, billig) */
                static FILE *s_gd = NULL; static int s_gc2 = 0;
                if (!s_gd) s_gd = fopen("gator_boss.log", "a");
                if (s_gd && (++s_gc2 % 30) == 0) {
                    fprintf(s_gd, "GPAT ziel=(%d,%d) rot=%d\n",
                            gx, gz, (int)e->rot_y);
                    fflush(s_gd);
                }
            }
        }
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
            g->cross_cd = 90;                /* Nutzer-Telemetrie 2026-09-10: mit 300
                                              * (10 s) kam er nicht hinterher, wenn
                                              * Leon direkt zurueckquerte - 3 s
                                              * reichen als Abwechslungs-Pause */
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
            /* Bogen aus der POSITIONS-WAHRHEIT (Session-Telemetrie 2026-09-10:
             * die t-Fenster ueberspannten die WASSERTASCHE zwischen Steg und
             * Rampe sowie Streif-Bahnen -> Schweben ueberm Wasser). Ziel-Y =
             * Peak nur UEBER der Insel, sonst Wasserlinie; y bewegt sich mit
             * begrenzter Rate dorthin (glatte Kanten), Bogen/Neigung folgen
             * der tatsaechlichen Hoehe. */
            /* LOOKAHEAD (Nutzer 2026-09-10 "kletterte im Nirgendwo kurzzeitig
             * nach oben": der Raten-Abbau trug die Hoehe ~600-1200 Einheiten
             * ueber die Kante hinaus, Telemetrie y=-2442 bei 491 neben der
             * Insel). Peak nur, wenn JETZT und in 13 Bahn-Frames (Abbauzeit
             * 1992/150) noch Insel darunter ist - das Sinken beginnt vor der
             * Kante und endet AN ihr. */
            int nfr2 = (g->cframes ? g->cframes : GB_CROSS_FRAMES);
            int tla  = t + (RE15_GB_CROSS_HUB / 100); /* = Abbauzeit HUB/Rate
                                                        * (Rate unten 100/F) */
            if (tla > nfr2) tla = nfr2;
            int32_t lax = g->cx0 + (int32_t)((int64_t)(g->cx1 - g->cx0) * tla / nfr2);
            int32_t laz = g->cz0 + (int32_t)((int64_t)(g->cz1 - g->cz0) * tla / nfr2);
            int32_t y_ziel = (gb_ueber_rampe(e->x, e->z) && gb_ueber_rampe(lax, laz))
                               ? (GB_WATER_Y - RE15_GB_CROSS_HUB) : GB_WATER_Y;
            int32_t dy = y_ziel - e->y;
            if (dy >  100) dy =  100;        /* DESIGN 2026-09-11 "flacher": 100/F
                                              * = weicher An-/Abstieg (~20 F); der
                                              * Lookahead oben ist an HUB/Rate
                                              * gekoppelt, damit das Sinken weiter
                                              * AN der Kante endet (v0.7.51!) */
            if (dy < -100) dy = -100;
            e->y += dy;
            {
                int32_t hub = GB_WATER_Y - e->y;           /* 0..HUB */
                if (hub < 0) hub = 0;
                g->arc_vz = (int16_t)(-(int32_t)GB_ARC_VZ_MAX * hub / RE15_GB_CROSS_HUB);
                g->arc_vz = (int16_t)(-g->arc_vz);         /* Betrag: haengen wie gehabt */
                /* Neigung aus der y-RATE: steigen = Nase hoch, sinken = runter. */
                g->pitch_vz = (int16_t)((dy * GB_PITCH_MAX) / 100);
            }
        }
        {   /* MESS-HOOK (Nutzer-Bild fehler/error.png 2026-09-11: "Abstand
             * hin zur Rampe sehr hoch"): am Bahn-Peak einmal die tiefste
             * gerenderte Bone-Welt-y loggen - die Bindpose-Rechnung (Bauch
             * 1392 unter Root) passt nicht zur Loko-Pose. */
            static int s_pk_done = 0;
            if (e->y <= (GB_WATER_Y - RE15_GB_CROSS_HUB) && !s_pk_done) {
                int bi2; int32_t bp2[3], miny = -99999, root = e->y;
                for (bi2 = 0; bi2 < 22; bi2++) {
                    re15_enemy_bone_world_pos(e, bi2, bp2);
                    if (bp2[1] > miny) miny = bp2[1];   /* groesstes y = tiefster Punkt */
                }
                {   static FILE *s_pk = NULL;
                    if (!s_pk) s_pk = fopen("gator_boss.log", "a");
                    if (s_pk) { fprintf(s_pk, "PEAK root=%d tiefster_bone=%d deck=-1800\n",
                                        root, miny); fflush(s_pk); }
                }
                s_pk_done = 1;
            }
        }
        /* Blick in Bahnrichtung (Engine-Peilung, Sofort-Snap) */
        re15_enemy_steer_point(e, g->cx1, g->cz1, 0x800);
        e->anim_frame++;
        /* Biss-Fenster auch auf der Plattform (Leon vertreiben): Reichweiten-Test,
         * EIN Biss pro Passage + byte-true Cooldown-Feld als Zweitsperre. */
        if (!g->bite_done && e->hit_stun == 0
            && pl->hit_react == 0 && gb_maul_dist(e, pl) <= 1500) {
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

    {   /* VOLLPHASEN-TELEMETRIE (Nutzer "bleibt immer noch haengen"): alle 30 F
         * Phase+Position in gator_boss.log - der Stillstands-Detektor findet
         * damit JEDEN Haenger samt Phase/Ziel-Kontext. */
        static FILE *s_tl = NULL; static int s_tc = 0;
        if (!s_tl) s_tl = fopen("gator_boss.log", "a");
        if (s_tl && (++s_tc % 30) == 0) {
            fprintf(s_tl, "TICK ph=%d pos=(%d,%d) y=%d dist=%d mo=%d cd=%d "
                          "rt=%d zw=%d ziel=(%d,%d)\n",
                    (int)g->phase, e->x, e->z, e->y, (int)dist,
                    (int)e->motion, (int)g->cross_cd,
                    (int)g->route, (int)g->dbg_zweig, g->dbg_tx, g->dbg_tz);
            fflush(s_tl);
        }
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
            int32_t plox = pl->x, ploz = pl->z;   /* fuer die Wand-Klemme unten */
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
                if (SEG[si].ofs > 0) g->maul_kontakt = 10;   /* Schnauze schiebt */
            }
            /* WAND-KLEMME fuer den geschobenen Spieler (Nutzer-Marker
             * 2026-09-11: Biss an der Suedwand schob Leon HINTER die Wand,
             * F351-F411 z=-27087..-29468, Marke 2 x=-13067 hinter der
             * Westwand): dieser Push schrieb pl->x/z DIREKT, als einziger
             * Leon-Beweger ohne re15_collision_constrain - kd_move und der
             * Pad-Move klemmen beide. */
            if (g_room_rdt_ok && (pl->x != plox || pl->z != ploz)) {
                int32_t nx = pl->x, nz = pl->z;
                re15_collision_ensure_band(pl->y);
                re15_collision_constrain(&g_room_rdt, plox, ploz, &nx, &nz);
                pl->x = nx; pl->z = nz;
            }
        }
        {   /* AUSSEN-WAECHTER (Diagnose, Nutzer-Marker 2026-09-11: Leon
             * landete hinter Sued-/Westwand): meldet die ersten Frames, in
             * denen der Spieler ausserhalb des SCA-Innenraums steht, samt
             * Spieler-Zustand - zeigt den verschiebenden Mechanismus. */
            static FILE *s_ow = NULL; static int s_ow_n = 0; static int s_ow_war = 0;
            int aus = (pl->x < -8900 || pl->x > 7200 || pl->z < -27000 || pl->z > -5400);
            if (aus && !s_ow_war) s_ow_n = 0;         /* neue Episode */
            s_ow_war = aus;
            if (aus && s_ow_n < 24) { s_ow_n++;
                if (!s_ow) s_ow = fopen("gator_boss.log", "a");
                if (s_ow) {
                    fprintf(s_ow, "AUSSEN pos=(%d,%d) hp=%d hr=%d mo=%d gph=%d gpos=(%d,%d)\n",
                            pl->x, pl->z, (int)pl->hp, (int)pl->hit_react,
                            (int)pl->motion, (int)g->phase, e->x, e->z);
                    fflush(s_ow);
                }
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

/* Punkt 7 ENTFERNT (Nutzer 2026-09-10): die Spinnen leben wieder auf ihren
 * RDT-Wasserpositionen; da der Boss von Gegnern nicht mehr geschoben wird,
 * brauchte es weder Plattform-Sitz noch Wandflucht. */

