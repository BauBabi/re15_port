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
#include "re15_anim_select.h"/* re15_compute_actor_kf - FRESS-SYNC-Mess-Schiene (gb_bone_welt) */
#include "re15_enemy.h"      /* re15_enemy_find - Frame-Flag-SEs aus der EM23-Bank */
#include "re15_emd.h"
#include "re15_collision.h"  /* re15_collision_constrain + ensure_band (Push-Klemme) */
#include "re15_math.h"       /* re15_squareroot0 - Fress-Ausrichtung (Maulachsen-Elevation) */
#include "re15_enemy_ai.h"   /* victim_force + Clip-Anker-Exporte (Fress-Finisher R6) */

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
    GBP_FRESSEN,    /* toedlicher Biss: Kopf runter, Leon fressen, abgehen
                     * (Nutzer-Design 2026-09-11) */
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
    int16_t  jaw_vz;               /* Kiefer-Zusatzoeffnung Bone 7 (Fressen) */
    int32_t  wirbel_x, wirbel_y, wirbel_z;  /* Startpunkt der Wirbelbahn */
    int16_t  pitch_vz;             /* Root-Neigung entlang der Bogen-Bahn (Q12) */
    int16_t  guard_t;              /* Aufricht-Rampe der Plattform-Belagerung */
    uint8_t  bite_done;            /* 1 Biss pro Lunge/Cross-Passage (Design-Schaden!) */
    /* ROUTEN-LATCH (Nutzer-Marker 2026-09-10 "bleibt haengen": die je Frame neu
     * bewertete Wegwahl konnte zwischen Rim-Anlauf und Ring-Ecke flattern -
     * sichtbares Vor-zurueck-Pendeln im Westkanal, Marke 3). Die Route wird
     * einmal gewaehlt und nur bei Zonenwechsel/Cooldown-Ablauf neu bewertet. */
    int8_t   route;                /* 0=keine, 1=WEST (um die Plattform), 2=RAMPE (queren) */
    int8_t   gzone_h;              /* Hysterese-Zone der GUARD-Zielwahl */
    int8_t   befrei;               /* Selbst-Befreiungs-Fenster (Klemm-Slab) */
    int32_t  ziel_lx, ziel_lz;     /* GUARD-Etappen-Latch: gehaltenes Steuer-Ziel */
    int16_t  ziel_zt;              /* Restframes der Etappe */
    int8_t   seite_l, seite_init;  /* gelatchte Belagerungsseite (0=Sued,1=Nord) */
    int8_t   umlauf;               /* Seitenwechsel-Umlauf laeuft (bis Ankunft) */
    int32_t  leon_lx, leon_lz;     /* Leon-Vortick (Vorhalte-Jagd) */
    int16_t  burst_t, burst_cd;    /* Wasser-BURST: Sprintframes / Abklingzeit */
    int8_t   leon_oben_alt;        /* Absprung-Flanke oeffnet den Burst sofort */
    int8_t   gefressen;            /* wir haben Leon versteckt (no_draw-Rueckgabe!) */
    int8_t   hit_zaehler;          /* Treffer seit letztem Flinch (alle 6 zuckt er) */
    int8_t   cross_oben;           /* Querung startete mit Leon OBEN (nur dann
                                    * bricht Leons Absprung die Bahn ab) */
    int16_t  seite_t;              /* Frames, die die NEUE Seite schon anliegt */
    int8_t   frei_seen;            /* frei_lx/lz gueltig (1 ab dem 2. Tick) */
    int32_t  frei_lx, frei_lz;     /* Position im Vortick (Totalstand-Nachweis) */
    /* v0.7.65: Steuer-Zustaende AUS den file-statics hierher - die lebten
     * ueber Neuspawns/Raum-Re-Entries (und Sweep-Testpaare) hinweg weiter
     * und verschleppten Pendel-/Waechterziele (Sweep-Beleg: Einzelfall
     * konvergent, Sequenz-Lauf rot). memset beim Neuspawn nullt sie. */
    int16_t  pend_t;  int8_t pend_dir;      /* Kanten-Pendel */
    int32_t  fw_x, fw_z; int16_t fw_t, fw_not;               /* CHASE-Waechter */
    int32_t  gw_x, gw_z, gw_notx, gw_notz; int16_t gw_t, gw_not; /* GUARD-Waechter */
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

/* RE15_GB_STUMM=1: gesamte gator_boss.log-Telemetrie aus (Sweep-Test). */
static int s_gb_stumm = -1;

/* ---- AUDIO (Nutzer 2026-09-12: "es fehlt der Biss Sound beim Alligator") -----
 * Der Boss war komplett stumm by omission: kein einziger SE-Aufruf, keine
 * ENEMSE-Bank registriert (Dossier gator-biss-sound.md). RE2-Belege:
 *   - ENEMSE-Bank des EM23 = 17 (Paar-Tabelle @0x800A7400; laengste Probe
 *     SE 4 = 18032 B = der Biss).
 *   - SE 4 spielt beim ANGRIFFS-START (ACTIVE sub3 Phase 0: li a0,4
 *     @0x80100D64, jal 0x8005bd6c @0x80100D84) und als Frame-Flag im
 *     Lunge-Clip 3 (Woerter 0x48000178/0x48000193 = f50/f77).
 *   - Frame-Flag-Dekodierung FUN_80016028 @0x80016034-54: Bit 0x08000000
 *     gesetzt -> SE = Wort >> 28.
 *   - SE-4-Frames armen einen 55-Frame-Cooldown fuer den Standard-Hurt-SE 5
 *     (@0x80100424-50; Hurt-Gate @0x801020AC-B8).
 * Der Port-Schnapp nutzt Clip 4 (in RE2 datenseitig stumm) - der explizite
 * SE-4-Ruf am Schnapp-Start ist das dokumentierte MAPPING des Angriffs-Start-
 * Rufs auf das Port-Design. Hook-Muster wie enemy_ai_re2_dog.c. */
#define GATOR_ENEMSE_BANK 17
static void (*s_gb_se_fn)(int se_id, int flag2000) = 0;
static void (*s_gb_bank_fn)(int bank) = 0;
static int  s_gb_brull_cd = 0;        /* 55-Frame-Hurt-SE-Sperre (@0x80100424-50) */
static int  s_gb_kf_zuletzt = -1;     /* Frame-Flag: je Keyframe genau einmal */

void re15_gator_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int))
{
    s_gb_se_fn   = se_fn;
    s_gb_bank_fn = bank_fn;
    if (s_gb_bank_fn) s_gb_bank_fn(GATOR_ENEMSE_BANK);
}
static void gb_se(int id)
{
    if (s_gb_se_fn) s_gb_se_fn(id, 0);
    if (id == 4) s_gb_brull_cd = 55;  /* Biss/Bruell armt die Hurt-SE-Sperre */
}
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
    return g->phase == GBP_CROSS || g->befrei > 0;
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
/* Biss-Treffer-Abschluss: normal = Schaden + Knockdown; TOEDLICH = die
 * FRESS-Sequenz (Nutzer-Design 2026-09-11: "Kopf zu Leon runter machen,
 * ihn Fressen - Leon verschwindet, und danach geht er einfach weg"). */
static void gb_biss_abschluss(re15_actor_t *e, gb_state_t *g, re15_actor_t *pl)
{
    re15_player_take_damage(pl, GB_BITE_TYPE, e->x, e->z);
    if (pl->hp < 0) {
        g->phase = GBP_FRESSEN; g->timer = 0;
        /* SE 3, NICHT 4 (Korrektur 2026-09-12, gator-se-korrektur.md): SE 4 ist der
         * 2,40-s-Brueller mit 1,43 s Attack - beim 45-F-Schnapp (Maul zu @F24) kam der
         * Peak 1,6+ s NACH dem Biss ("der Sound von den Aligator Bissen ist falsch").
         * SE 3 ist das einzige kurz-perkussive Sample der Bank 17 (<30 ms Attack,
         * 22 kHz "Klatsch"; RE2-Kontext Wasser-Einschlag Clip 5 f80). Ein echtes
         * Schnapp-Sample existiert nirgends (Clip 4 datenstumm, ROOM40A0-Raumbank
         * leer). RE2 startet SE 4 nur am Lunge-Clip-3-BEGINN (@0x80100D64/84), wo
         * das Maul erst ~F54 oeffnet - Peak faellt MIT dem Biss zusammen. */
        gb_se(3); e->motion = 4; e->anim_frame = 0;     /* Schnapp-Clip als Zubeissen */
        g->arc_vz = 0;
    } else {
        re15_player_knockdown_begin(re15_ai_facing_dir(e, pl));
    }
    pl->hit_react |= 1;
    e->hit_stun = 0x64;                       /* byte-true Re-Attack-Sperre */
}

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

/* ==== FRESS-SYNC-MESS-SCHIENE (Befund B, analysis/befunde_runde4_2026-09-12/
 * fress-sync.md): GERENDERTE Bone-Weltposition — dieselbe Kette wie der
 * NPC-Renderer (main.c:8119 g_anim_pose_actor=npc; main.c:8155-8159 Uniform-
 * Scale +0x166 auf der 3x3 = Offsets x/y/z um die Wurzel skaliert).
 * re15_enemy_bone_world_pos taugt hier NICHT als Render-Wahrheit: es NULLT
 * g_anim_pose_actor (re15_damage.c:1979), wodurch der pitch_vz/jaw_vz/arc_vz-
 * Hook (skeleton_common.c:602, gated auf bact) NICHT posiert wird, und es
 * skaliert nicht (Wurzel-Fallback + ungeskalierte Offsets).
 * NUR MESSUNG: der zusaetzliche prev_root/prev_angles-Snapshot des Pose-Laufs
 * ist waehrend FRESSEN wirkungsfrei, weil der Boss e->motion DIREKT setzt
 * (anim_frac bleibt 0 -> blend nie aktiv, skeleton_common.c:213). */
static void gb_bone_welt(re15_actor_t *e, int bone, int32_t out[3])
{
    out[0] = e->x; out[1] = e->y; out[2] = e->z;
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok || b->skel.bone_count <= 0
        || bone < 0 || bone >= b->skel.bone_count) return;
    int kf = re15_compute_actor_kf(&b->anim, &b->skel, e, -1, e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = e;               /* Render-Pfad: vz-Hook aktiv (main.c:8119) */
    int rv = re15_skel_compute_pose(&b->skel, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return;
    re15_skel_bone_to_world(poses[bone].trans, (int16_t)e->rot_y,
                            e->x, e->y, e->z, out);
    if (e->render_scale_q12) {           /* main.c:8155: Scale liegt auf der 3x3 */
        out[0] = e->x + (int32_t)(((int64_t)(out[0] - e->x) * e->render_scale_q12) >> 12);
        out[1] = e->y + (int32_t)(((int64_t)(out[1] - e->y) * e->render_scale_q12) >> 12);
        out[2] = e->z + (int32_t)(((int64_t)(out[2] - e->z) * e->render_scale_q12) >> 12);
    }
}


/* Insel-Wand zwischen Gator und Leon? (Marge 0 = die reine Wand) - ein Biss
 * DURCH die Plattform ist unmoeglich (Nutzer 2026-09-11: "direkt an der
 * Platform auf der anderen Seite ... probiert staendig die Angriffs-
 * animation aber trifft mich nicht"). */
static int gb_wand_dazwischen(const re15_actor_t *e, const re15_actor_t *pl)
{
    /* Prueflinie ab MAULPUNKT (Zentrum+2600 voraus) statt ab Zentrum
     * (Biss-Sweep 2026-09-11): die lange Zentrum-Linie streifte schon bei
     * Ecken-Beruehrung die Plattform-Box und blockte legitime Bisse. */
    int32_t fc = re15_cos_q12((int)e->rot_y);
    int32_t fs = re15_sin_q12((int)e->rot_y);
    int32_t kx = e->x + (int32_t)(((int64_t)fc * 2600) >> 12);
    int32_t kz = e->z - (int32_t)(((int64_t)fs * 2600) >> 12);
    return gb_seg_hits_platform(kx, kz, pl->x, pl->z, 0)
        || gb_seg_hits_ramp(kx, kz, pl->x, pl->z, 0);
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

static int     gb_zone(int32_t z);       /* fwd (definiert unten) */
static int32_t gb_iabs(int32_t v);       /* fwd */

/* Zentraler Kanten-Umweg als FUNKTION (v0.7.64): schert die Luftlinie zum
 * Ziel im Koerper-Schatten von Plattform ODER Rampe, erst zur guenstigsten
 * FREIEN Umlauf-Ecke (Kosten = Umwegstrecke); ist keine frei, zur
 * naechstgelegenen. Rueckgabe: gewaehlte Ecke (0..3) oder -1 (Weg frei).
 * CHASE und GUARD nutzen dieselbe Maschine - die GUARD-eigene Following/
 * ring_target-Wegwahl widersprach ihr dreimal (Marken 2026-09-11). */
static int gb_kanten_umweg(const re15_actor_t *e, int32_t *tx, int32_t *tz)
{
    /* v0.7.65: KUERZESTER PFAD im 6-Knoten-Sichtgraph (Start, 4 Ecken, Ziel)
     * statt 1-Hop-Greedy. SWEEP-BELEG (probe_gator_sweep, 530/3286 Paare):
     * ein Sued-Ost-Kantenziel ist von Norden NUR ueber die 2-Hop-Kette
     * Nordecke->Westecke erreichbar - der Ecken-Graph ist wegen der Rampe
     * (Ost-Schiene blockiert) eine KETTE C1-C0-C3-C2, und die greedy
     * zielnaechste Ecke (C2) war eine Sackgasse mit stabilem Attraktor
     * (Endpunkt (3903,-9628) aus 28+ Starts). */
    enum { NK = 5 };                       /* 0..3 = Ecken, 4 = Ziel */
    int32_t kx[NK], kz[NK];
    int64_t dist[NK]; int prev[NK], fest[NK];
    int i, j;
    if (gb_seg_frei(e->x, e->z, *tx, *tz, GB_KOERPER_M)) return -1;
    for (i = 0; i < 4; i++) { kx[i] = gb_ecke[i][0]; kz[i] = gb_ecke[i][1]; }
    kx[4] = *tx; kz[4] = *tz;
    for (i = 0; i < NK; i++) {
        fest[i] = 0; prev[i] = -2;
        if (gb_seg_frei(e->x, e->z, kx[i], kz[i], GB_KOERPER_M)) {
            dist[i] = (int64_t)gb_iabs(e->x - kx[i]) + gb_iabs(e->z - kz[i]);
            prev[i] = -1;                  /* direkt vom Start */
        } else dist[i] = (int64_t)1 << 60;
    }
    for (;;) {                             /* naive Dijkstra-Relaxation */
        int u = -1;
        for (i = 0; i < NK; i++)
            if (!fest[i] && dist[i] < ((int64_t)1 << 60) && (u < 0 || dist[i] < dist[u]))
                u = i;
        if (u < 0 || u == 4) break;
        fest[u] = 1;
        for (j = 0; j < NK; j++) {
            int64_t w;
            if (fest[j] || !gb_seg_frei(kx[u], kz[u], kx[j], kz[j], GB_KOERPER_M))
                continue;
            w = dist[u] + gb_iabs(kx[u] - kx[j]) + gb_iabs(kz[u] - kz[j]);
            if (w < dist[j]) { dist[j] = w; prev[j] = u; }
        }
    }
    if (prev[4] != -2) {                   /* Ziel erreichbar: ersten Hop nehmen */
        int hop = 4;
        while (prev[hop] >= 0) hop = prev[hop];
        if (hop == 4) return -1;           /* (kann nicht: Ziel nicht seg-frei) */
        /* Stand-Ecke als erster Hop bringt nichts - dann den NAECHSTEN
         * Pfadknoten nehmen (Marke F721: er stand AUF C0). */
        if (gb_iabs(e->x - kx[hop]) + gb_iabs(e->z - kz[hop]) < 700) {
            int nxt = 4;
            while (prev[nxt] >= 0 && prev[nxt] != hop) nxt = prev[nxt];
            hop = nxt;
        }
        *tx = kx[hop]; *tz = kz[hop];
        return (hop < 4) ? hop : -1;
    }
    {   /* Ziel unerreichbar (Schattenlage): naechstgelegene freie Ecke,
         * sonst naechstgelegene ueberhaupt - raus aus dem Schatten. */
        int bi = -1; int64_t best = 0;
        for (i = 0; i < 4; i++) {
            int64_t k = (int64_t)gb_iabs(e->x - kx[i]) + gb_iabs(e->z - kz[i]);
            if (k < 700) k += 200000;
            if (!gb_seg_frei(e->x, e->z, kx[i], kz[i], GB_KOERPER_M)) k += 400000;
            if (bi < 0 || k < best) { best = k; bi = i; }
        }
        *tx = kx[bi]; *tz = kz[bi];
        return bi;
    }
}

/* Becken-Zone mit 300er-HYSTERESE (Nutzer-Marke 2026-09-11 "schwankt hin
 * und her": der Gator schwamm AUF der Grenze z=-18100, gb_zone flippte
 * frameweise 0<->2 und mit ihr das komplette Belagerungsziel). */
static int gb_zone_hyst(int32_t z, int8_t *alt)
{
    int zn = gb_zone(z);
    if (zn != *alt) {
        if (*alt < 0 || *alt > 2
            || (zn == 0 && z <= GB_RAMP_Z0 - 300)
            || (zn == 1 && z >= GB_RAMP_Z1 + 300)
            || (zn == 2 && z >= GB_RAMP_Z0 + 300 && z <= GB_RAMP_Z1 - 300))
            *alt = (int8_t)zn;
    }
    return *alt;
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
    if (s_gb_brull_cd == 0) gb_se(5);   /* Standard-Hurt-SE 5 (@0x801020C4), gegated
                                         * auf die 55er-Bruell-Sperre (@0x801020AC-B8) */
    if (!g->aggro) g->aggro = 1;        /* Fernschuss startet den Kampf */
    /* Blut bei JEDEM Treffer (Punkt 5): Burst 0x1500 am vorderen Rumpf (Bone 1). */
    {
        int32_t bp[3];
        re15_enemy_bone_world_pos(e, 1, bp);
        re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500,
                             bp[0], bp[1], bp[2], (int16_t)e->rot_y);
    }
    g->hit_zaehler++;
    if ((e->hp <= g->next_flinch_hp || g->hit_zaehler >= 6)
        && g->phase != GBP_CROSS && g->phase != GBP_DIE) {
        /* Nutzer 2026-09-11 ("die Schadenanimation fehlt mir noch ab einer
         * gewissen Menge an treffern"): mit der 10%-Schwelle allein waeren
         * es ~20 Handgun-Treffer je Flinch - zusaetzlich flincht er alle
         * 6 TREFFER (DESIGN), grosse Schadensspruenge weiterhin sofort. */
        while (g->next_flinch_hp >= e->hp) g->next_flinch_hp -= GB_FLINCH_STEP;
        g->hit_zaehler = 0;
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

/* Belagerungsseite mit BEHARRLICHKEIT (Nutzer 2026-09-11: "Gehe ich vor
 * kommt er an, gehe ich dann zurueck, geht er weg" - Leon konnte das
 * Kantenziel per Taenzeln um die Flaechen-Mitte im Sekundentakt flippen,
 * der Gator lief fernsteuerbare Riesen-Achten): 800er-Totband um die
 * Mitte + die neue Seite muss 90 F stabil anstehen. */
static int gb_seite_latch(gb_state_t *g, int32_t lz, int32_t mitte)
{
    int roh = (lz >= mitte + 800) ? 1 : (lz <= mitte - 800) ? 0 : -1;
    if (!g->seite_init) {
        g->seite_init = 1; g->seite_t = 0;
        g->seite_l = (int8_t)(lz >= mitte);
    } else if (roh >= 0 && roh != g->seite_l) {
        /* v0.7.67 (Nutzer: "geht immer noch so"): Leon wechselte die Seite
         * mit ECHTEN Schritten (2600er, jenseits des Totbands) im 10-s-Takt
         * und entwertete jeden Riesen-Westumlauf per Schritt zurueck. 300 F
         * (10 s) stabile Gegenseite noetig, und ein BEGONNENER Umlauf wird
         * zu Ende geschwommen (g->umlauf, geloescht bei Kanten-Ankunft). */
        if (!g->umlauf && ++g->seite_t >= 300) {
            g->seite_l = (int8_t)roh; g->seite_t = 0; g->umlauf = 1;
        }
    } else g->seite_t = 0;
    return g->seite_l;
}

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
static void gb_cross_begin(re15_actor_t *e, gb_state_t *g, int von_sued,
                           int32_t ziel_x)
{
    /* DIAGONAL-BAHN (Nutzer 2026-09-11 "man kann ihn perfekt austricksen
     * waehrend er oben vorbei laeuft": Leon stand im WEST-Drittel der
     * Rampe (x=2318), alle Bahnen liefen auf der 4450er-Klemme 2100+
     * seitlich vorbei - der Vorbeigeh-Schnapp war nie erreichbar):
     * START-x bleibt im erreichbaren Osten (Anlauf-Klemme, Plattform-
     * Ecken-Schatten), das BAHN-ENDE zieht auf LEONS Spur (Lande-x-
     * Klemme 3350 = Eckenschatten sqrt(1466^2-550^2)+1850 + Marge). */
    int32_t cx = e->x;
    if (cx < GB_RAMP_X0 + GB_RING_M) cx = GB_RAMP_X0 + GB_RING_M;
    if (cx > GB_RAMP_X1 - 700)       cx = GB_RAMP_X1 - 700;
    if (ziel_x < 3350)               ziel_x = 3350;
    if (ziel_x > GB_RAMP_X1 - 700)   ziel_x = GB_RAMP_X1 - 700;
    /* Westgrenze = X0+GB_RING_M (4450), NICHT +700: die Plattform-Ostkante
     * (1850) wirft mit dem 2200er-Koerperradius (@0x80118b98) einen
     * Klemm-Schatten bis x~3950 - ein Anlaufpunkt darunter ist mit dem
     * Koerper UNERREICHBAR (Nutzer-Marke 3, 2026-09-10: er stand bei
     * (2522,-22170) exakt auf der Suedkanten-Klemmlinie, Ziel (2550,-20700),
     * 1470 vor dem 1400er-Gate - fuer immer). */
    g->cx0 = cx; g->cx1 = ziel_x;
    if (von_sued) { g->cz0 = GB_RAMP_Z0 - GB_BAHN_M; g->cz1 = GB_RAMP_Z1 + GB_BAHN_M; }
    else          { g->cz0 = GB_RAMP_Z1 + GB_BAHN_M; g->cz1 = GB_RAMP_Z0 - GB_BAHN_M; }
    {
        int64_t ldx = g->cx1 - g->cx0, ldz = g->cz1 - g->cz0;
        int64_t l2 = ldx * ldx + ldz * ldz;
        int32_t len = 64; while ((int64_t)len * len < l2 && len < 30000) len += 64;
        g->cframes = (int16_t)(len / 32);   /* DESIGN 2026-09-11: 25% fixer
                                             * (Kletterphase war 7 s Hilflosigkeit) */
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
        if (!s_cl) s_cl = s_gb_stumm ? NULL : fopen("gator_boss.log", "a");
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
    if (s_gb_stumm < 0) s_gb_stumm = (getenv("RE15_GB_STUMM") != NULL);
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    gb_state_t   *g  = &s_gb[slot];

    if (s_gb_room != g_current_room_id) {     /* Raumwechsel: Boss-Zustand frisch */
        memset(s_gb, 0, sizeof s_gb);
        s_gb_room = g_current_room_id;
        s_gb_brull_cd = 0; s_gb_kf_zuletzt = -1;
    }
    if (s_gb_brull_cd > 0) s_gb_brull_cd--;   /* @0x80100440-50: je Tick -1 */
    {   /* FRAME-FLAG-SEs byte-true (FUN_80016028 @0x80016034-54): das EDD-Wort
         * der aktuellen (Clip,Frame)-Position; Bit 0x08000000 -> SE = Wort>>28.
         * Deckt Schwimmen (Clip 0: SE 2 @f76/f162) und die Todesrolle (Clip 7:
         * SE 4 @f4/f45/f115 + SE 2 @f25/f36/f44) ab; Clip 4 ist datenseitig
         * stumm (der Schnapp-SE kommt vom expliziten Mapping-Ruf).
         * emd_common.c:93 haelt die EDD-Woerter ROH - die Flag-Bits sind da. */
        re15_enemy_bank_t *bb = re15_enemy_find(0x23);
        if (bb && bb->ok && e->motion >= 0 && e->motion < bb->anim.clip_count) {
            const re15_emd_clip_t *cl = &bb->anim.clips[e->motion];
            if (e->anim_frame < cl->frame_count) {
                int kf = cl->first_frame + (int)e->anim_frame;
                if (kf != s_gb_kf_zuletzt) {
                    uint32_t w = bb->anim.frames[kf];
                    s_gb_kf_zuletzt = kf;
                    if (w & 0x08000000u) gb_se((int)(w >> 28));
                }
            }
        }
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
        g->gzone_h = (int8_t)gb_zone(e->z);
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
                      if (*tv == '2') gb_cross_begin(e, g, gb_zone(e->z) == 0,
                                                    g_actors[RE15_ACTOR_SLOT_PLAYER].x);
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

    /* SELBST-BEFREIUNG (Sweep-Klasse "end==start", probe_gator_sweep
     * 2026-09-11): steckt das ZENTRUM im Klemm-Slab (Blockkante +
     * Koerperradius), verwirft die Box-Klemme von innen JEDE Bewegung -
     * Totalstand trotz korrektem Ziel/advance. Real erreichbar ueber
     * CROSS-Landungen (Klemme ausgesetzt). Raus zum naechsten Slab-Rand,
     * Klemme fuer das Fenster ausgesetzt (skip_clamp liest g->befrei). */
    if (g->befrei > 0) g->befrei--;
    if (g->phase == GBP_LURK || g->phase == GBP_CHASE || g->phase == GBP_GUARD) {
        /* Gefangen = im Slab (Kante+radius+60) UND Totalstand (<8 Bewegung
         * seit dem Vortick): echte Gefangene bewegen sich EXAKT null (die
         * Box-Klemme verwirft von innen alles), legitime Kanten-Staende
         * wackeln oder lauern ausserhalb des Slabs (Ziele >= Kante+1600).
         * Ein reiner Tiefen-Test war nicht trennscharf (Klemmlinie ~radius-30
         * vs. Gefangenen-Tiefen bis radius-50, Sweep 365->476-Rueckschlag). */
        int32_t r = (int32_t)e->hit_radius_min + 60;
        /* Aussenwand-Slabs (Sweep-Rest: Starts (-3200,-26200)/(6400,-21400)
         * sassen im Sued-/Ostwand-Schatten mit Totalstand): */
        {
            int32_t bx = e->x, bz = e->z, war = 0;
            if (bx < -8900 + r) { bx = -8900 + r + 120; war = 1; }
            if (bx >  7200 - r) { bx =  7200 - r - 120; war = 1; }
            if (bz < -27000 + r) { bz = -27000 + r + 120; war = 1; }
            if (bz >  -5400 - r) { bz =  -5400 - r - 120; war = 1; }
            if (war && gb_iabs(e->x - g->frei_lx) + gb_iabs(e->z - g->frei_lz) < 8
                && g->frei_seen) {
                re15_enemy_steer_point(e, bx, bz, 0x800);
                re15_ai_advance(e, GB_SWIM_SPEED);
                e->y = GB_WATER_Y;
                g->befrei = 2;
                if (e->motion != 0) { e->motion = 0; e->anim_frame = 0; }
                e->anim_frame++;
                return;
            }
        }
        int in_p = (e->x >= GB_PLAT_X0 - r && e->x <= GB_PLAT_X1 + r &&
                    e->z >= GB_PLAT_Z0 - r && e->z <= GB_PLAT_Z1 + r);
        int in_r = (e->x >= GB_RAMP_X0 - r && e->x <= GB_RAMP_X1 + r &&
                    e->z >= GB_RAMP_Z0 - r && e->z <= GB_RAMP_Z1 + r);
        if ((in_p || in_r)
            && gb_iabs(e->x - g->frei_lx) + gb_iabs(e->z - g->frei_lz) < 8
            && g->frei_seen) {
            int32_t bx = e->x, bz = e->z;
            if (in_p) {
                int32_t dw = bx - (GB_PLAT_X0 - r), de2 = (GB_PLAT_X1 + r) - bx;
                int32_t dn = bz - (GB_PLAT_Z0 - r), ds = (GB_PLAT_Z1 + r) - bz;
                int32_t m2 = dw; int s2 = 0;
                if (de2 < m2) { m2 = de2; s2 = 1; }
                if (dn  < m2) { m2 = dn;  s2 = 2; }
                if (ds  < m2) { m2 = ds;  s2 = 3; }
                if      (s2 == 0) bx = GB_PLAT_X0 - r - 120;
                else if (s2 == 1) bx = GB_PLAT_X1 + r + 120;
                else if (s2 == 2) bz = GB_PLAT_Z0 - r - 120;
                else              bz = GB_PLAT_Z1 + r + 120;
            } else {
                int32_t dw = bx - (GB_RAMP_X0 - r), de2 = (GB_RAMP_X1 + r) - bx;
                int32_t dn = bz - (GB_RAMP_Z0 - r), ds = (GB_RAMP_Z1 + r) - bz;
                int32_t m2 = dw; int s2 = 0;
                if (de2 < m2) { m2 = de2; s2 = 1; }
                if (dn  < m2) { m2 = dn;  s2 = 2; }
                if (ds  < m2) { m2 = ds;  s2 = 3; }
                if      (s2 == 0) bx = GB_RAMP_X0 - r - 120;
                else if (s2 == 1) bx = GB_RAMP_X1 + r + 120;
                else if (s2 == 2) bz = GB_RAMP_Z0 - r - 120;
                else              bz = GB_RAMP_Z1 + r + 120;
            }
            re15_enemy_steer_point(e, bx, bz, 0x800);
            re15_ai_advance(e, GB_SWIM_SPEED);
            e->y = GB_WATER_Y;
            g->befrei = 2;
            if (e->motion != 0) { e->motion = 0; e->anim_frame = 0; }
            e->anim_frame++;
            return;
        }
    }
    /* SIEGER-ABZUG (Nutzer 2026-09-11: "Sobald ich tot bin dreht sich der
     * aligator noch die ganze Zeit ueber mir"): nach Leons Tod laesst er ab
     * und gleitet zum Lauerplatz zurueck - kein Nahstand-Gewende ueber der
     * Leiche waehrend der Todes-Kamera. */
    if (pl->hp >= 0 && g->gefressen) {        /* Continue-Respawn: Leon
                                               * wieder sichtbar machen! */
        pl->no_draw = 0; g->gefressen = 0;
        pl->fress_skip_mask = 0; pl->rot_x = 0;
    }
    if (pl->hp < 0 && g->phase != GBP_DIE && g->phase != GBP_DEAD
        && g->phase != GBP_OFF && g->phase != GBP_FRESSEN) {
        int64_t rdx = (int64_t)GB_START_X - e->x, rdz = (int64_t)GB_START_Z - e->z;
        re15_enemy_steer_point(e, GB_START_X, GB_START_Z, 0x20);
        if (rdx * rdx + rdz * rdz > (int64_t)1500 * 1500)
            re15_ai_advance(e, 40);
        e->y = GB_WATER_Y;
        g->arc_vz = 0; g->pitch_vz = 0;
        if (e->motion != 0) { e->motion = 0; e->anim_frame = 0; }
        e->anim_frame++;
        return;
    }
    g->frei_lx = e->x; g->frei_lz = e->z; g->frei_seen = 1;
    {   /* WASSER-BURST-Verwaltung (DESIGN "gefaehrlich machen", Nutzer
         * 2026-09-11 "relativ dumm und 0 gefaehrlich"): Timer + Absprung-
         * Flanke (Leon verlaesst die Flaeche -> Burst sofort frei). */
        int lo = (pl->y < -900);
        if (g->leon_oben_alt && !lo) g->burst_cd = 0;
        g->leon_oben_alt = (int8_t)lo;
        if (g->burst_t > 0) g->burst_t--;
        else if (g->burst_cd > 0) g->burst_cd--;
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
        {   /* VORHALTE (DESIGN "schlauer machen", Nutzer 2026-09-11): auf
             * die in 20 F extrapolierte Leon-Position zielen; Sprung-/
             * Teleport-Frames (>200 Delta) und Erst-Tick liefern 0. */
            int32_t vdx = pl->x - g->leon_lx, vdz = pl->z - g->leon_lz;
            if (gb_iabs(vdx) <= 200 && gb_iabs(vdz) <= 200
                && dist > 2000                 /* nah: direkt, kein Zappeln */
                && (g->leon_lx | g->leon_lz)) {
                tx += vdx * 20; tz += vdz * 20;
            }
        }
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
                    /* Nutzer-Marke F280 2026-09-11 ("nimmt den langen Weg,
                     * dreht sogar um"): das fruehere "RAMPE immer" warf den
                     * Kostenvergleich weg - Gator NW, Leon SW hiess kw 23150
                     * vs kr 40650, und er lief trotzdem nach OSTEN zum
                     * Rampen-Anlauf. Jetzt entscheiden die KOSTEN; die Rampe
                     * behaelt +2000 Praeferenz ("direkt drueber klettern"
                     * gilt, wenn sie wirklich der direkte Weg ist). */
                    g->route = (g->cross_cd == 0 && kr <= kw + 2000) ? 2 : 1;
                    g->route_zwang = (int8_t)(g->cross_cd > 0);
                    g->zone_g = (int8_t)zg; g->zone_l = (int8_t)zl;
                }
                if (g->route == 2 && g->cross_cd == 0) {
                    int64_t adx = e->x - rx, adz = e->z - rz_ein;
                    if (adx * adx + adz * adz < (int64_t)1400 * 1400) {
                        gb_cross_begin(e, g, zg == 0, pl->x);
                        g->cross_oben = 0;    /* Verfolgungs-Querung */
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
            int64_t zdx = (int64_t)tx - e->x, zdz = (int64_t)tz - e->z;
            if (g->fw_not > 0) {
                g->fw_not--;
                tx = g->dbg_tx; tz = g->dbg_tz;   /* Not-Ecke halten */
            } else if (++g->fw_t >= 90) {
                int64_t mdx = (int64_t)e->x - g->fw_x, mdz = (int64_t)e->z - g->fw_z;
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
                    g->fw_not = 60; g->route = 0;
                    {   static FILE *s_fl = NULL;
                        if (!s_fl) s_fl = s_gb_stumm ? NULL : fopen("gator_boss.log", "a");
                        if (s_fl) { fprintf(s_fl, "NOTFREI pos=(%d,%d) ecke=%d\n",
                                            e->x, e->z, bi); fflush(s_fl); }
                    }
                }
                g->fw_t = 0; g->fw_x = e->x; g->fw_z = e->z;
            }
        }
        /* ZENTRALER KANTEN-UMWEG (Nutzer-Marke 2026-09-11, dritter Fall
         * derselben Klasse: der Rampen-ANLAUF steuerte blind auf
         * (4450,-20700), waehrend der Gator westlich der Plattform im
         * Koerper-Schatten stand - Klemm-Kriechen auf x=-3137). Gilt fuer
         * JEDEN Zweig: schert die Luftlinie zum Steuerziel im Schatten der
         * Plattform, erst zur guenstigsten FREIEN Umlauf-Ecke; ist keine
         * frei (Start selbst im Schatten-Band), zur naechstgelegenen. */
        {
            int ubi = gb_kanten_umweg(e, &tx, &tz);
            if (ubi >= 0) g->dbg_zweig = (int8_t)(10 + ubi);
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
            if (!s_dl) s_dl = s_gb_stumm ? NULL : fopen("gator_boss.log", "a");
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
                re15_enemy_steer_point(e, tx, tz, 0x60);   /* zackige Wende
                                                            * (180 Grad in 21 F) */
                /* NOT-SCHNAPP (Nutzer 2026-09-11 "keine Luecke auf gleicher
                 * Ebene"): auch mitten in der Wende beisst er zu, wenn Leon
                 * in Reichweite ist - vorher brach der Zweig VOR dem Commit
                 * ab (Biss-Sweep: Leon kreiste bei dist~120 dauerhaft im
                 * toten Winkel, BISS-FAIL sz=2 NO/Nord-Ost). */
                if (pl->hit_react == 0 && e->hit_stun == 0
                    && !gb_wand_dazwischen(e, pl)
                    && (dist < 3200 || gb_maul_dist(e, pl) < 2400
                        || g->maul_kontakt > 0)) {
                    g->phase = GBP_LUNGE; g->timer = 0; g->bite_done = 0;
                    gb_se(3); e->motion = 4; e->anim_frame = 0;
                    break;
                }
                /* RUECKWAERTSGANG (Nutzer: "wenn es dem Alligator hilft mal
                 * rueckwaerts zu laufen"): Leon klebt seitlich/hinten am
                 * Koerper - rueckwaerts rausziehen und dabei drehen, statt
                 * auf der Stelle zu ruehren. */
                if (dist < 3500 && (dlt > 0x600 || dlt < -0x600)) {
                    int32_t bc = re15_cos_q12((int)e->rot_y);
                    int32_t bs = re15_sin_q12((int)e->rot_y);
                    e->x -= (int32_t)(((int64_t)bc * 44) >> 12);
                    e->z += (int32_t)(((int64_t)bs * 44) >> 12);
                }
                e->y = GB_WATER_Y;
                if (e->motion != 0) { e->motion = 0; e->anim_frame = 0; }
                e->anim_frame++;
                break;
            }
            re15_enemy_steer_point(e, tx, tz, 0x30);
                                 /* DESIGN: konstanter straffer Slew statt der
                                  * byte-true B[4]-Streuung (6..37) - "leere
                                  * Drehungen ohne Sinn muessen komplett raus" */
        }
        {   /* BURST-Start: Leon im Wasser-Sichtkegel, mittlere Distanz */
            int soll2 = ((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff;
            int dlt2  = ((soll2 - (int)e->rot_y + 0x800) & 0x0fff) - 0x800;
            if (g->burst_t == 0 && g->burst_cd == 0
                && dist > 2600 && dist < 9000
                && dlt2 <= 0x180 && dlt2 >= -0x180) {
                g->burst_t = 75; g->burst_cd = 255;
                if (!s_gb_stumm) {
                    static FILE *s_bu = NULL;
                    if (!s_bu) s_bu = s_gb_stumm ? NULL : fopen("gator_boss.log", "a");
                    if (s_bu) { fprintf(s_bu, "BURST pos=(%d,%d) dist=%d\n",
                                        e->x, e->z, (int)dist); fflush(s_bu); }
                }
            }
        }
        re15_ai_advance(e, g->burst_t ? 100 : 56);
                                 /* DESIGN: Verfolgung 56 (statt byte-true
                                  * Proxy 48), BURST 100 fuer 75 F - im
                                  * Wasser ist Leon nie sicher */
        e->y = GB_WATER_Y;
        if (e->motion != 0) { e->motion = 0; e->anim_frame = 0; }
        e->anim_frame++;

        /* Biss-Commit: byte-true Reichweiten-Test, Winkel-Gate auf 0x400
         * geoeffnet + Nah-Trigger (Session-Telemetrie 2026-09-10: er stand
         * 3 s reglos bei dist=623 vor Leon - das enge 0x180-Gate liess den
         * Schnapp nie zu, wenn Leon seitlich in der Nische stand). */
        if (pl->hit_react == 0 && e->hit_stun == 0
            && !gb_wand_dazwischen(e, pl)
            && (dist < 3200 || gb_maul_dist(e, pl) < 2400 || g->maul_kontakt > 0)) {
            /* dist-Netz 2600->3200 (Biss-Sweep 2026-09-11): seitlich am
             * Koerper zeigte der Maulpunkt weg und 2600 griff nicht. */
            /* Commit-Gate GEMESSEN 2026-09-11: der alte 0x1770er-Sichtkegel
             * (6000) startete Leer-Schnapps, deren Maul Leon im 45-F-Clip
             * nie erreichte (Messung: maul 3088->2624, kein Treffer);
             * maul<2400 = Fenster-Ankunft bei ~40/F Lunge-Vortrieb. */
            g->phase = GBP_LUNGE; g->timer = 0; g->bite_done = 0;
            /* Clip 4 = SCHNAPP-Biss - Kieferkurve GEMESSEN (EDD/EMR 2026-09-10):
             * Maul reisst ab Frame 4 auf, Peak -591 @F12, zu @F24 (45 F). Clip 3
             * oeffnet erst ab ~F54 (Peak F96/150) - der alte 40-Frame-Abbruch
             * zeigte deshalb "keinerlei Beissanimation" (Nutzer-Befund). */
            gb_se(3); e->motion = 4; e->anim_frame = 0;
        }
        break; }

    case GBP_LUNGE: {                         /* Punkt 3: Biss mit Schaden */
        re15_enemy_steer_point(e, pl->x, pl->z, 0x50);    /* Slew 0x30->0x50
                                                            * (DESIGN): der Not-
                                                            * Schnapp startet auch
                                                            * aus schraegem Winkel */
        if (gb_maul_dist(e, pl) > 1400) {
            if (dist < 1500) {
                /* Leon klebt am Koerper - das Maul (2600 voraus) ist VORBEI:
                 * rueckwaerts ziehen, bis es wieder vor Leon liegt
                 * (Nutzer-Freigabe Rueckwaertsgang, Biss-Sweep 2026-09-11). */
                int32_t bc = re15_cos_q12((int)e->rot_y);
                int32_t bs = re15_sin_q12((int)e->rot_y);
                e->x -= (int32_t)(((int64_t)bc * 44) >> 12);
                e->z += (int32_t)(((int64_t)bs * 44) >> 12);
            } else {
                re15_ai_advance(e, GB_LUNGE_SPEED);
            }
        }
        e->y = GB_WATER_Y;
        {   /* Mess-Telemetrie Biss-Fenster (Nutzer 2026-09-11: "trifft mich
             * quasi so gut wie nie") */
            static FILE *s_bl = NULL;
            if (!s_bl) s_bl = s_gb_stumm ? NULL : fopen("gator_boss.log", "a");
            if (s_bl && e->anim_frame >= 6 && e->anim_frame <= 34) {
                fprintf(s_bl, "BISS af=%d dist=%d maul=%d wand=%d hr=%d\n",
                        (int)e->anim_frame, (int)dist, gb_maul_dist(e, pl),
                        gb_wand_dazwischen(e, pl), (int)pl->hit_react);
                fflush(s_bl);
            }
        }
        if (!g->bite_done
            && e->anim_frame >= 6 && e->anim_frame <= 34   /* Fenster geweitet
                                                            * (Biss-Sweep 2026-09-11):
                                                            * 8..20 liess Leon in
                                                            * F21-42 entkommen */
            && pl->hit_react == 0
            && (gb_maul_dist(e, pl) <= 1500 || dist < 1000 || g->maul_kontakt > 0)
            && !gb_wand_dazwischen(e, pl)) {
            /* dist<1000 = Leon IM Kopf-Segment-Radius (900+Puffer): Nahkontakt
             * zaehlt als Biss (Biss-Sweep 2026-09-11, Leon auf dem Koerper). */
            /* Treffer = MAUL-Kontakt (Kopf-Segment 900 + Spieler 450 + 150
             * Puffer); das alte Zentrum-Gate s. gb_maul_dist-Messnotiz. */
            /* Fenster {19,20,21} @0x80118c68 (byte-true Mechanik) — DESIGN-Schaden:
             * attack_type 5 -> dmg_table[5] = 50 + Knockdown-Latch. EIN Biss pro
             * Lunge (bite_done): ohne den Latch traefen alle drei Fensterframes
             * (gemessen 2026-09-10: Leon 100 HP -> Game Over in <2 s). */
                            gb_biss_abschluss(e, g, pl);
        }
        e->anim_frame++;
        if (e->anim_frame > 42) {             /* Clip 4 (45 F) ausklingen lassen */
            if (e->hit_stun == 0) e->hit_stun = 0x14;      /* Fehlschnapp-Sperre
                                                            * 0x2d->0x14 (DESIGN,
                                                            * Biss-Sweep 2026-09-11):
                                                            * 45 F bisslos = Luecke */
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
        if (g->cross_cd > 0) g->cross_cd--;   /* auch im Dauer-GUARD abbauen */
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
                int zg2 = gb_zone_hyst(e->z, &g->gzone_h);
                int auf_rampe = (pl->x >= GB_RAMP_X0);
                int pend_z = 0;   /* Pendel-Achse folgt der ZIELART (Sweep-Beleg
                                   * 2026-09-11: die zg2-Achsenwahl verschob das
                                   * SUED-Kantenziel bei Hysterese-Kipp auf
                                   * gz-18500 = Rampen-Band = unerreichbar ->
                                   * Dijkstra-Fallback C3 = der C0<->C3-Flip). */
                if (auf_rampe) {              /* Nutzer-Marken F755-F936 2026-09-11
                                               * ("er findet den Weg wieder nicht"):
                                               * Leon stand an der NORDkante der
                                               * Rampe, der Gator belagerte stur
                                               * die SUEDkante seiner eigenen Zone
                                               * (dist 5700, Hochbiss unerreichbar).
                                               * Die Belagerungs-SEITE richtet sich
                                               * jetzt nach LEONS naeherer Kante;
                                               * der Seitenwechsel laeuft um WEST
                                               * (kein Ostumlauf, und KLETTERN bei
                                               * Leon-oben bleibt per Nutzer-Design
                                               * verboten). */
                    int zielzone = gb_seite_latch(g, pl->z, (GB_RAMP_Z0 + GB_RAMP_Z1) / 2);
                    if (zg2 != 2 && zg2 != zielzone && g->cross_cd == 0) {
                        /* QUER-CROSS unter dem oben stehenden Leon hindurch
                         * (NUTZER-FREIGABE 2026-09-11 "kannst du schon
                         * machen"): statt des Riesen-Westumlaufs klettert er
                         * ueber die Rampe zur Leon-Seite - der Schnapp im
                         * Vorbeigehen ist der CROSS-Biss (maul<=1500, ohne
                         * Wand-Check, v0.7.55); Leon schiebt der SEG-Push
                         * NUR mit Spieler-Wand-Klemme (v0.7.56), er kann
                         * schlimmstenfalls die offene Stufe hinab. */
                        int32_t rx = pl->x, rz_ein;
                        if (rx < GB_RAMP_X0 + GB_RING_M) rx = GB_RAMP_X0 + GB_RING_M;
                        if (rx > GB_RAMP_X1 - 700)       rx = GB_RAMP_X1 - 700;
                        rz_ein = (zg2 == 0) ? (GB_RAMP_Z0 - GB_BAHN_M)
                                            : (GB_RAMP_Z1 + GB_BAHN_M);
                        {
                            int64_t adx = e->x - rx, adz = e->z - rz_ein;
                            if (adx * adx + adz * adz < (int64_t)1400 * 1400) {
                                gb_cross_begin(e, g, zg2 == 0, pl->x);
                                g->cross_oben = 1;   /* Belagerungs-Querung */
                                break;
                            }
                        }
                        gx = rx; gz = rz_ein;   /* Anlauf; Weg via Umweg unten */
                        g->umlauf = 0;          /* kein West-Commit noetig */
                    } else {
                        gx = pl->x;
                        if (gx < GB_RAMP_X0 + GB_RING_M) gx = GB_RAMP_X0 + GB_RING_M;
                        if (gx > GB_RAMP_X1 - 700)       gx = GB_RAMP_X1 - 700;
                        gz = (zielzone == 0) ? (GB_RAMP_Z0 - GB_GPAT_M)
                                             : (GB_RAMP_Z1 + GB_GPAT_M);
                    }
                } else if (zg2 == 2) {        /* Westkanal: Plattform-Westkante */
                    pend_z = 1;                   /* laengs der Kante = z-Achse */
                    gx = GB_PLAT_X0 - GB_GPAT_M;
                    gz = pl->z;
                    if (gz < GB_PLAT_Z0) gz = GB_PLAT_Z0;
                    if (gz > GB_PLAT_Z1) gz = GB_PLAT_Z1;
                } else {                      /* Plattform-Kante auf LEONS Seite
                                               * (Sweep-Klasse Platt-NO-oben,
                                               * 2026-09-11: die Gator-Zonen-Wahl
                                               * belagerte die Suedkante, Leon
                                               * stand nordseitig - dieselbe
                                               * Luecke wie v0.7.62 bei der
                                               * Rampe; den Weg findet der
                                               * Dijkstra-Umweg). */
                    int32_t lw  = pl->x - GB_PLAT_X0;              /* Leon->Westkante */
                    int32_t lz0 = pl->z - GB_PLAT_Z0, lz1 = GB_PLAT_Z1 - pl->z;
                    int32_t lns = (lz0 < lz1) ? lz0 : lz1;
                    if (lw < lns) {
                        /* Leon westnah ODER mittig (Biss-Sweep 2026-09-11
                         * Platt-Mitte: N/S-Kante = dist 5675, unerreichbar;
                         * die Westkante = 3675 liegt in Hochbiss-Reichweite) */
                        pend_z = 1;
                        gx = GB_PLAT_X0 - GB_GPAT_M;
                        gz = pl->z;
                        if (gz < GB_PLAT_Z0) gz = GB_PLAT_Z0;
                        if (gz > GB_PLAT_Z1) gz = GB_PLAT_Z1;
                    } else {
                    int zzp = gb_seite_latch(g, pl->z, (GB_PLAT_Z0 + GB_PLAT_Z1) / 2);
                    gx = pl->x;
                    if (gx < GB_PLAT_X0) gx = GB_PLAT_X0;
                    if (gx > GB_PLAT_X1) gx = GB_PLAT_X1;
                    gz = (zzp == 0) ? (GB_PLAT_Z0 - GB_GPAT_M) : (GB_PLAT_Z1 + GB_GPAT_M);
                    }
                }
                if (g->umlauf && gb_iabs(e->z - gz) < 1500)
                    g->umlauf = 0;             /* an der neuen Kante angekommen */
                if (!s_gb_stumm) {   /* Diagnose: Kantenziel VOR Pendel/Umweg */
                    static FILE *s_gzl = NULL; static int s_gzc = 0;
                    if (!s_gzl) s_gzl = fopen("gator_boss.log", "a");
                    if (s_gzl && (++s_gzc % 30) == 0) {
                        fprintf(s_gzl, "GZIEL kante=(%d,%d) zg=%d rampe=%d\n",
                                gx, gz, zg2, auf_rampe);
                        fflush(s_gzl);
                    }
                }
                {   /* Kanten-Pendel: steht Leon still (und damit das Ziel),
                     * patrouilliert er sichtbar laengs der Kante statt zu
                     * erstarren (+-1500 alle 90 F). Die Naehe wird gegen das
                     * VERSETZTE Ziel gemessen (Nutzer-Marke 2026-09-11:
                     * Basis-Messung ergab den Drei-Radien-Deadlock 573<600
                     * Advance vs. 1222>800 Basis-Naehe - der Timer lief nie,
                     * er stand fuer immer 573 vorm Pendelpunkt). */
                    int64_t bdx, bdz;
                    if (!g->pend_dir) g->pend_dir = 1;
                    if (pend_z) {
                        gz += g->pend_dir * 1500;
                        if (gz < GB_PLAT_Z0) gz = GB_PLAT_Z0;
                        if (gz > GB_PLAT_Z1) gz = GB_PLAT_Z1;
                    } else {
                        gx += g->pend_dir * 1500;
                        if (auf_rampe) {
                            if (gx < GB_RAMP_X0 + GB_RING_M) gx = GB_RAMP_X0 + GB_RING_M;
                            if (gx > GB_RAMP_X1 - 700)       gx = GB_RAMP_X1 - 700;
                        } else {
                            if (gx < GB_PLAT_X0) gx = GB_PLAT_X0;
                            if (gx > GB_PLAT_X1) gx = GB_PLAT_X1;
                        }
                    }
                    bdx = e->x - gx; bdz = e->z - gz;
                    if (bdx * bdx + bdz * bdz < (int64_t)800 * 800)
                        if (++g->pend_t >= 90) { g->pend_t = 0; g->pend_dir = (int8_t)-g->pend_dir; }
                }
            }
            /* v0.7.64: der WEG zum Belagerungsziel laeuft ueber DIESELBE
             * Umweg-Maschine wie im CHASE - das GUARD-eigene Following
             * (ring_target) widersprach ihr dreimal (Kreiseln, SO-Sackgasse,
             * Grenz-Schwanken). */
            {
                int uh = gb_kanten_umweg(e, &gx, &gz);
                if (!s_gb_stumm) {
                    static FILE *s_gwl2 = NULL; static int s_gwc2 = 0;
                    if (!s_gwl2) s_gwl2 = fopen("gator_boss.log", "a");
                    if (s_gwl2 && (++s_gwc2 % 30) == 0) {
                        fprintf(s_gwl2, "GWEG hop=%d ziel=(%d,%d) not=%d\n",
                                uh, gx, gz, (int)g->gw_not);
                        fflush(s_gwl2);
                    }
                }
            }
            {   /* Fortschritts-Waechter auch im GUARD (Nutzer-Marke
                 * 2026-09-11: der CHASE-Waechter griff im GUARD-Deadlock
                 * nicht, 0 NOTFREI bei 11 s Stillstand): kaum Strecke in
                 * 90 F bei fernem Ziel -> 60 F naechste freie Ecke. */
                int64_t wdx = (int64_t)gx - e->x, wdz = (int64_t)gz - e->z;
                if (g->gw_not > 0) {
                    g->gw_not--;
                } else if (++g->gw_t >= 90) {
                    int64_t pdx = (int64_t)e->x - g->gw_x, pdz = (int64_t)e->z - g->gw_z;
                    if (pdx * pdx + pdz * pdz < (int64_t)300 * 300
                        && wdx * wdx + wdz * wdz > (int64_t)1500 * 1500) {
                        /* v0.7.65: Zwangs-Ziel = der PFAD-Hop des Dijkstra
                         * (die alte "naechste freie Ecke" zwang im Wechsel
                         * GEGEN den Pfad - C0<->C3-Flip im Westkanal,
                         * Einzelfall-Telemetrie Sweep 2026-09-11). */
                        int32_t hx = gx, hz = gz;
                        int bi2 = gb_kanten_umweg(e, &hx, &hz);
                        g->gw_notx = hx; g->gw_notz = hz;
                        if (bi2 < 0) bi2 = 9;   /* Ziel direkt frei */
                        g->gw_not = 60;
                        {   static FILE *s_gwl = NULL;
                            if (!s_gwl) s_gwl = s_gb_stumm ? NULL : fopen("gator_boss.log", "a");
                            if (s_gwl) { fprintf(s_gwl, "NOTFREI-G pos=(%d,%d) ecke=%d\n",
                                                 e->x, e->z, bi2); fflush(s_gwl); }
                        }
                    }
                    g->gw_t = 0; g->gw_x = e->x; g->gw_z = e->z;
                }
                if (g->gw_not > 0) { gx = g->gw_notx; gz = g->gw_notz; }
            }
            {   /* ETAPPEN-LATCH (Frame-Telemetrie 2026-09-11: das finale
                 * Ziel flippte zyklisch C0<->C3 an den Zonen-Schwellen -
                 * Wende alle 35-78 F, ewiges Kanal-Pendeln): das Steuer-Ziel
                 * wird GEHALTEN, bis es erreicht ist (<800) oder 120 F um
                 * sind. Jede Flip-Quelle unterhalb der Etappenlaenge ist
                 * damit wirkungslos - er faehrt Etappen zu Ende. */
                if (g->ziel_zt > 0
                    && gb_iabs(e->x - g->ziel_lx) + gb_iabs(e->z - g->ziel_lz) > 800) {
                    g->ziel_zt--;
                    gx = g->ziel_lx; gz = g->ziel_lz;
                } else {
                    g->ziel_lx = gx; g->ziel_lz = gz; g->ziel_zt = 120;
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
                if (!s_gd) s_gd = s_gb_stumm ? NULL : fopen("gator_boss.log", "a");
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
            if (!s_gl) s_gl = s_gb_stumm ? NULL : fopen("gator_boss.log", "a");
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
            gb_se(3); e->motion = 4; e->anim_frame = 0; g->bite_done = 0;
        }
        if (e->motion == 4) {
            e->anim_frame++;
            if (!g->bite_done && e->anim_frame >= 6 && e->anim_frame <= 34
                && pl->hit_react == 0 && dist < 3900) {   /* DESIGN: Kantenstand +
                                               * gehobener Kopf erreicht die Flaeche;
                                               * 3400->3900 + Fenster geweitet
                                               * (Biss-Sweep 2026-09-11: die
                                               * Plattform-MITTE liegt 3675 von
                                               * der besten Kanten-Position) */
                                gb_biss_abschluss(e, g, pl);
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

    case GBP_CROSS: {                         /* Punkt 8: Bogen ueber die Rampe */
        g->ct++;
        int32_t t = g->ct;                    /* 0..GB_CROSS_FRAMES */
        /* ANTI-HILFLOSIGKEIT (Nutzer 2026-09-11: "waehrend seiner Kletter-
         * phase ist der Aligator absolut hilflos" - Leon sprang die Stufe
         * hoch/runter um die starre 7-s-Bahn herum): */
        if (pl->y < -900) {
            /* (b) Leon OBEN: das Bahn-Ende verfolgt LIVE seine Spur.
             * SAFE-ZONE-SCHLIESSUNG (Nutzer 2026-09-11 "immer noch total
             * leicht ausgetrickst": Leon stand im West-Drittel x=2318,
             * die 3350er-Lande-Klemme liess jede Bahn 2100 seitlich
             * vorbeiziehen): UEBER der Rampe (t im Fenster) darf die Bahn
             * die volle Rampenbreite [2550..6500] - die 3350er-Klemme
             * sichert nur noch den LANDEPUNKT im Wasser (Ecken-Schatten). */
            int32_t zx = pl->x;
            int32_t lo = (t >= g->t_ein && t <= g->t_aus)
                           ? (GB_RAMP_X0 + 700) : 3350;
            if (zx < lo)               zx = lo;
            if (zx > GB_RAMP_X1 - 700) zx = GB_RAMP_X1 - 700;
            g->cx1 = zx;
        } else if (g->cross_oben && (t < g->t_ein || t > g->t_aus)) {
            /* (c) Leon UNTEN + Gator noch/wieder ueber Wasser: Bahn sofort
             * beenden und normal jagen statt die Restbahn abzusitzen */
            g->phase = GBP_CHASE; g->arc_vz = 0; g->pitch_vz = 0;
            g->cross_cd = 90;
            e->y = GB_WATER_Y;
            e->motion = 0; e->anim_frame = 0;
            break;
        }
        if (t >= (g->cframes ? g->cframes : GB_CROSS_FRAMES)) {
            g->phase = GBP_CHASE; g->arc_vz = 0; g->pitch_vz = 0;
            g->cross_cd = (int16_t)((pl->y < -900) ? 300 : 90);
                                             /* Leon OBEN: 10 s auf der Seite
                                              * bleiben (souveraene Belagerung
                                              * statt Dauer-Pendelei, Nutzer
                                              * 2026-09-11 "faehrt komisch
                                              * rum"); unten agil (90):
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
                    if (!s_pk) s_pk = s_gb_stumm ? NULL : fopen("gator_boss.log", "a");
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
        if (e->hit_stun == 0                  /* Mehrfach-Schnapp: hit_stun
                                               * 0x64 taktet die Rate (Anti-
                                               * Hilflosigkeit 2026-09-11) */
            && pl->hit_react == 0 && gb_maul_dist(e, pl) <= 1500) {
                            gb_biss_abschluss(e, g, pl);
        }
        break; }

    case GBP_FRESSEN: {
        /* ============ DER ECHTE RE2-FINISHER (Runde 6, gator-vollausbau.md 6.1/7b) ============
         * Ersetzt die prozedurale Wirbelbahn + fress_skip_mask + Trudel/Ausricht-Design:
         * RE2 SPIELT DATEN AB. Ablauf (jede Zahl mit @0x im Dossier):
         *   P0 (einmalig): Spieler-HP=-1; TELEPORT Spieler = Gator + RotY(yaw)*(10643,-915)
         *      (@0x8010106C-84; der RE2-Korridor laeuft entlang X, generische Form);
         *      Spieler-Yaw = Gator-Yaw (@0x80101154-5C). Leon STRAMPELT (RE2: PLD-Paar-1
         *      Clip 2, 113 F @0x80102E88-90; Port-MAPPING: Leons eigener Clip 2).
         *   P1: Gator-Clip 4 (45 F). Bei anim_frame==13 (@0x801010FC-1104): gemeinsames
         *      ANKER-PAAR (0x80015B94-Zwilling re15_clip_anchor_set_pub am AKTUELLEN
         *      Gator-Frame, Kopie in den Spieler) + Opfer-Start (Spieler+0x158=1-Analogon
         *      @0x80101148-50): Victim-Modus 4 mit EM23-OPFER-PAAR-3 CLIP 1.
         *   P2: Gator-Clip 5 (120 F Schuetteln) + Leon Opfer-Clip 1 (120 F) SYNCHRON;
         *      je Frame absolute Platzierung Anker + RotY(yaw)*(kf-Offset) fuer BEIDE
         *      (0x80015CB8-Zwilling @0x80101168-78; Leon-Schleuderbahn steckt in den
         *      Keyframes: dx 7541..10527 vorm Anker, dz -2648..+1254, y-Spitze -9177
         *      bei f60-75 aus dem POSE-Kanal - der Victim-Renderer traegt sie).
         *   P3: Gator-Clip 11 als Kau-Loop OHNE Bewegung (FUN_8001A240 @0x801011D8-E0);
         *      Leon im letzten Opfer-Frame GEPARKT (kein no_draw - er haengt im Maul).
         * SEs: Frame-Flags von Clip 5 (f2 SE1, f80 SE3) spielt der Frame-Flag-Spieler;
         * Clip 4 ist datenseitig stumm (SE-3-Zubeiss-Mapping bleibt separat, Runde 5). */
        re15_enemy_bank_t *gb23 = re15_enemy_find(e->type);
        g->timer++;
        e->y = GB_WATER_Y;
        if (g->timer == 1) {                           /* ---- P0 @0x80100FBC-10E4 ---- */
            int32_t fc0 = re15_cos_q12((int)e->rot_y), fs0 = re15_sin_q12((int)e->rot_y);
            pl->fress_skip_mask = 0;                   /* Halbkoerper-Trick entfaellt */
            pl->no_draw = 0;
            pl->x = e->x + (int32_t)(((int64_t)fc0 * 10643 + (int64_t)fs0 * (-915)) >> 12);
            pl->z = e->z + (int32_t)((-(int64_t)fs0 * 10643 + (int64_t)fc0 * (-915)) >> 12);
            pl->rot_y = e->rot_y;                      /* Yaw-Kopie @0x80101154-5C */
            pl->rot_x = 0;
            pl->motion = 2; pl->anim_frame = 0;        /* STRAMPELN: Leons Clip 2 (113 F) */
            e->motion = 4; e->anim_frame = 0;          /* Gator-Clip 4 (45 F) */
            g->gefressen = 0;
        }
        if (!g->gefressen) {                           /* ---- P1: Clip 4 bis f13 ---- */
            if (pl->anim_frame < 112) pl->anim_frame++;   /* Strampeln vorwaerts */
            if (e->anim_frame == 13 && gb23 && gb23->ok) {   /* @0x801010FC-1104 */
                /* Gemeinsames Anker-Paar am AKTUELLEN Gator-Frame (@0x80101110-34):
                 * erst der Gator mit SEINEM Clip, dann Kopie in den Spieler. */
                re15_clip_anchor_set_pub(e, &gb23->skel, &gb23->anim, 4, 13);
                pl->anchor_x = e->anchor_x; pl->anchor_z = e->anchor_z;
                if (gb23->victim_ok) {                 /* Opfer-Start (+0x158=1-Analogon) */
                    re15_player_victim_force(e->type, 1, 0);
                    g->gefressen = 1;
                    if (!s_gb_stumm) {
                        FILE *fl = fopen("gator_boss.log", "a");
                        if (fl) { fprintf(fl, "FRESS-P2 Anker=(%d,%d) yaw=%d\n",
                                          (int)e->anchor_x, (int)e->anchor_z,
                                          (int)e->rot_y); fclose(fl); }
                    }
                } else {
                    g->gefressen = 1;                  /* bankfrei (Unit-Pins): weiter ohne
                                                        * Victim-Anim, Ablauf identisch */
                }
                g->timer = 100;                        /* P2-Zeitbasis: t-100 = Schuettel-Frame */
            }
            e->anim_frame++;
        } else if (g->timer <= 220) {                  /* ---- P2: Clip 5 + Opfer-1, 120 F ---- */
            int sf = g->timer - 100;                   /* 0..120 */
            e->motion = 5;
            e->anim_frame = (uint32_t)sf;
            if (gb23 && gb23->ok) {
                /* Absolute Platzierung BEIDER aus dem gemeinsamen Anker (@0x80101168-78):
                 * Gator mit Clip 5, Leon mit Opfer-Clip 1 (Victim-Skelett = kf-Traeger). */
                re15_clip_root_motion_abs_pub(e, &gb23->skel, &gb23->anim, 5, sf);
                if (gb23->victim_ok && sf < 120) {
                    pl->rot_y = e->rot_y;              /* Yaw haelt die Drehbahn synchron */
                    pl->motion = 1; pl->anim_frame = (uint32_t)sf;
                    re15_clip_root_motion_abs_pub(pl, &gb23->skel_victim,
                                                  &gb23->anim_victim, 1, sf);
                }
            }
            if (!s_gb_stumm) {
                static FILE *s_ff = NULL;
                if (!s_ff) s_ff = fopen("gator_boss.log", "a");
                if (s_ff && (sf & 7) == 0) {
                    int32_t vb0[3];
                    re15_enemy_bone_world_pos(pl, 0, vb0);
                    fprintf(s_ff, "FSYNC2 sf=%d gaf=%u laf=%u pl=(%d,%d,%d) b0y=%d\n",
                            sf, (unsigned)e->anim_frame, (unsigned)pl->anim_frame,
                            (int)pl->x, (int)pl->y, (int)pl->z, vb0[1]);
                    fflush(s_ff);
                }
            }
        } else if (g->timer <= 340) {                  /* ---- P3: Clip 11 Kau-Loop ---- */
            e->motion = 11;
            e->anim_frame = (uint32_t)((g->timer - 221) % 30);   /* Loop ohne Bewegung
                                                                  * (FUN_8001A240) */
            if (gb23 && gb23->victim_ok) {             /* Leon im letzten Opfer-Frame parken */
                pl->motion = 1; pl->anim_frame = 119;
                re15_clip_root_motion_abs_pub(pl, &gb23->skel_victim,
                                              &gb23->anim_victim, 1, 119);
            }
        } else {
            re15_player_victim_force_end();
            g->pitch_vz = 0; g->jaw_vz = 0;
            g->phase = GBP_CHASE;                      /* der pl->hp<0-Abzug uebernimmt */
            e->motion = 0; e->anim_frame = 0;
        }
        break;
    }
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
        g->leon_lx = pl->x; g->leon_lz = pl->z;   /* Vorhalte-Vortick */
        static FILE *s_tl = NULL; static int s_tc = 0;
        if (!s_tl) s_tl = s_gb_stumm ? NULL : fopen("gator_boss.log", "a");
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
        static const struct { int32_t ofs, r; } SEG[5] = {
            { 2600, 900 }, { 4600, 700 }, { -2600, 900 }, { -4800, 750 },
            {    0, 1050 }   /* Koerpermitte (Biss-Sweep 2026-09-11: zwischen
                              * den +-2600er-Segmenten stand Leon im Loch) */
        };
        int32_t fc = re15_cos_q12((int)e->rot_y);
        int32_t fs = re15_sin_q12((int)e->rot_y);
        int32_t dy = pl->y - (e->y - GB_WATER_Y);  /* Push-Zentrum = e->y + 1200 */
        if (dy > -1500 && dy < 1500) {
            int si;
            int32_t plox = pl->x, ploz = pl->z;   /* fuer die Wand-Klemme unten */
            for (si = 0; si < 5; si++) {
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
                if (!s_ow) s_ow = s_gb_stumm ? NULL : fopen("gator_boss.log", "a");
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
/* Fuer den Death-Flow (game_step_common.c): laeuft gerade die Fress-
 * Sequenz? Dann haelt der Gameover-Exit an, bis sie zu Ende gespielt ist
 * (Nutzer-Design: Schnapp 2 + Verschlingen muessen sichtbar sein). */
int re15_gator_fressen_hold(void)
{
    int i;
    for (i = 0; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *e = &g_actors[i];
        if (re15_gator_boss_active(e) && s_gb[i].phase == GBP_FRESSEN
            && s_gb[i].timer < 215)
            return 1;
    }
    return 0;
}

int re15_gator_spine_arc_vz(const re15_actor_t *e, int bone)
{
    if (!re15_gator_boss_active(e)) return 0;
    const gb_state_t *g = &s_gb[(int)(e - g_actors)];
    if (g->arc_vz == 0 && g->pitch_vz == 0 && g->jaw_vz == 0) return 0;
    /* Nutzer-Befund 2026-09-10: "Der Bogen muss sein - unten Kopf, Ruecken
     * oben, unten Schweif" - die alte Vorzeichenwahl schweifte BEIDE Enden
     * nach OBEN (U statt Bogen). Rz-Geometrie: +X-Kette (Kopf) haengt mit
     * az>0 ab (x'->+y = PSX-unten), -X-Kette (Schwanz) mit az<0. */
    switch (bone) {
    case 0:                            return  (int)g->pitch_vz; /* Root-Neigung der Bahn */
    case 7:                            return  (int)g->jaw_vz;   /* Kiefer reisst auf (Fressen) */
    case 1: case 5: case 6:            return  (int)g->arc_vz;   /* Kopf haengt drueben ab */
    case 11: case 12: case 13:         return -(int)g->arc_vz;   /* Schweif haengt diesseits */
    default:                           return 0;
    }
}

/* Punkt 7 ENTFERNT (Nutzer 2026-09-10): die Spinnen leben wieder auf ihren
 * RDT-Wasserpositionen; da der Boss von Gegnern nicht mehr geschoben wird,
 * brauchte es weder Plattform-Sitz noch Wandflucht. */

