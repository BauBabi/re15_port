/*
 * RE1.5 Rebuilt — Beta-Birkin-Formen (Kopf, Herkunft und Abgrenzung: include/re15_birkin_formen.h).
 *
 * ⛔ PORT-ERGAENZUNG nach dem 80%-Video, KEIN Auslieferungscode. Belegte Werte tragen @0x…; die uebrigen
 * Zahlen sind aus Video 1 (Complete Disc, 4:36-4:40) zurueckprojiziert oder Gestaltung und sind so markiert.
 */
#include "re15_birkin_formen.h"
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_damage.h"        /* re15_enemy_apply_hitbox, re15_enemy_bone_world_pos */
#include "re15_scd.h"           /* g_scd.work_vars[0x0A] = angezeigter Cut, re15_game_flag_set */
#include "re15_room.h"          /* g_current_room_id */
#include "re15_audio.h"        /* re15_audio_room_se */
#include "re15_camera.h"       /* Runde 41 Spur H: re15_camera_build_view (Sichttest des Ausloesers) */
#include "re15_rdt.h"          /* Runde 41 Spur H: re15_rdt_get_region_quad (Anker-Viereck = FUN_80014324) */
#include "re15_aot.h"          /* Runde 41 Spur H: re15_aot_point_in_quad (FUN_80014368) */
#include "re15_enemy_ai.h"     /* Runde 41 Spur H: re15_player_is_grabbed */
#include "re15_skeleton.h"     /* Runde 41 Spur H: re15_cos_q12 / re15_sin_q12 (Gang auf die Kamera zu) */
#include "re15_engine.h"       /* Runde 41 Spur H: g_engine.frame_count (nur Messzeile RE15_HANGAR_LOG) */
#include "re15_schwanz_r41.h"  /* Runde 41 SF N1.9: Seitenvorgabe der Schwanz-Feder-Kette in der Hangar-Szene */
#include <stdlib.h>
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

void re15_g5_boss_spawn(int slot);
int  re15_g5_boss_zustand(int *sub, int *ph, int *timer);   /* enemy_ai_boss_g5.c */                       /* enemy_ai_boss_g5.c: Ctor faellig */
int  re15_r38_birkin_anim(re15_actor_t *e);              /* enemy_ai_common.c: Clip-Vorschub des Typs */
int  re15_r38_birkin_fc(const re15_actor_t *e);          /* enemy_ai_common.c: Clip-Laenge des Typs */

/* ---- Modellwahl --------------------------------------------------------------------------- */
static int s_bf_mutiert = 0;   /* 5090: Original-Birkin hat mutiert -> Bahnsteig-Form */
const char *re15_bf_emd_datei(uint8_t type)
{
    /* Roter Affe (Video 1 4:11-4:19, tools/birkin_formen.py ROT_AFFE): nur die CLUTs von EM29 sind
     * umgefaerbt, Modell/Clips/KI bleiben die des Typs 0x29. RE15_KEIN_ROTER_AFFE=1 = Auslieferung. */
    if (type == 0x29u)
        return ((g_current_room_id & 0xFFFEu) == 0x4080u && !getenv("RE15_KEIN_ROTER_AFFE"))
               ? "EMD/EM29_ROT.EMD" : NULL;
    if (type != 0x30u) return NULL;
    const char *form = getenv("RE15_BIRKIN_FORM");
    if (form) {
        if (!strcmp(form, "original"))  return NULL;
        if (!strcmp(form, "hangar"))    return "EMD/EM30_HANGAR.EMD";
        if (!strcmp(form, "bahnsteig")) return NULL;     /* eigene Bank: re15_bf_bahnsteig_bank_faellig */
    }
    if (re15_bf_hangar_aktiv())    return "EMD/EM30_HANGAR.EMD";
    if (re15_bf_labor_aktiv())     return "EMD/EM30_HANGAR.EMD";
    return NULL;
}

/* 5090: der Original-Birkin (EM30, Auslieferung) kaempft zuerst; erst nach seiner Mutation (Clip 0x14, sub 6
 * @0x80117f80) traegt er die Bahnsteig-Form (RE2/BAHNSTEIG_G4.EMD, Lader platform/pc/main.c). */
int re15_bf_bahnsteig_bank_faellig(uint8_t type)
{
    if (type != 0x30u) return 0;
    const char *form = getenv("RE15_BIRKIN_FORM");
    if (form) return !strcmp(form, "bahnsteig");
    return re15_bf_bahnsteig_aktiv() && s_bf_mutiert;
}

int re15_bf_mutation_offen(void) { return re15_bf_bahnsteig_aktiv() && !s_bf_mutiert; }

static void (*s_bf_modell_hook)(uint8_t) = 0;
void re15_bf_modell_hook(void (*fn)(uint8_t type)) { s_bf_modell_hook = fn; }

void re15_bf_mutation_fertig(int slot)
{
    (void)slot;
    if (!re15_bf_bahnsteig_aktiv() || s_bf_mutiert) return;
    s_bf_mutiert = 1;
    if (s_bf_modell_hook) s_bf_modell_hook(0x30u);      /* Plattform laedt die 0x30-Bank neu (jetzt Bahnsteig-Form) */
}

int re15_bf_bahnsteig_aktiv(void)
{
    return (g_current_room_id & 0xFFFEu) == 0x5090u && !getenv("RE15_ENDKAMPF_NUR_G5");
}

int re15_bf_hangar_aktiv(void)
{
    return (g_current_room_id & 0xFFFEu) == 0x50A0u && !getenv("RE15_KEIN_HANGAR_BIRKIN");
}

/* Labor-Kampf (Mod: ROOM50E0 Leon, ROOM50F1 Elza, Sce_em_set Typ 0x30 aus CDEMD0, Dossier herkunft/birkin_modelle.md 1.2).
 * Nutzer 2026-11-02 (NUTZER_ENTSCHEIDE E17): der Birkin im Labor ist derselbe wie im Hangar (Complete Disc Video 4: Labor
 * 12-22 s, Hangar 45-51 s) -> dieselbe Form. Die Labor-Kameras des Videos sind ROOM50E0 Schnitt 8 (Pult) und 5 (Steg),
 * gemessen per Gradienten-Korrelation 0,60 / 0,65 gegen die BSS-Bilder (Dossier SF_sichel.md). */
int re15_bf_labor_aktiv(void)
{
    uint16_t r = (uint16_t)(g_current_room_id & 0xFFFEu);
    return r == 0x50E0u || r == 0x50F0u;
}

/* ---- Bahnsteig: Uebergabe an den G5 -------------------------------------------------------
 * Aufruf aus der Birkin-Todesmaschine, Phase 2 (enemy_ai_common.c, @0x8011a690). Der Typwechsel genuegt,
 * weil 0x36 in 5090 eine eigene Bank (RE2-EM36, PC-Lader) und eine eigene KI-Wurzel hat:
 *   - re15_g5_boss_spawn = Sce_em_set-Ctor-Pfad (@0x800421e0 `sw zero,4(s0)` -> Ctor @0x801003CC, HP 600)
 *   - grid 0x13 = die Kampfstart-Armierung (ROOM5090 sub04 Member_set(0x0c,0x13) @0x130A), damit stellt
 *     sich der G5 im naechsten Tick selbst auf (-9000,-23400) (@0x801011d0/@0x801011d8) und spawnt seine
 *     vier 0x37-Arme. */
void re15_bf_morph_zu_g5(int slot)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *e = &g_actors[slot];
    e->type = 0x36u;
    e->grid_id = 0x13u;
    e->state = 0; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->motion = 0; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;
    e->anim_blend_rate = 0; e->anim_flags &= (uint16_t)~0x80u;   /* Rate und Richtung der Form gehoeren nicht dem G5 */
    e->hp = 600;                                         /* Ctor @0x801003fc, ueberschreibt er ohnehin */
    re15_enemy_apply_hitbox(e, 0x36u);
    re15_g5_boss_spawn(slot);
}

/* ---- Hangar-Szene --------------------------------------------------------------------------
 * Runde 41 Spur H, Nachbesserung "Bild fuer Bild": der Ablauf ist aus dem 80%-Video gemessen (Version_80%/Resident Evil 1.5
 * Complete Disc Video 4 (480p Upscale), Schnitt in den Hangar bei 45.0 s; Bildnummer h = 0 ab dem Schnitt, 29.97 Bilder/s
 * = ein Spieltakt je Bild). Eichung Video -> PSX-Bild: psx = video/2 + (-1,-3) (Kantenkorrelation des Hintergrunds gegen
 * das Port-Bild Cut 12, 0.70 / 0.67, tools/r41_hangar/kalib.py). Rueckprojektion durch die RID-Kamera ROOM50A0 Cut 12
 * (pos (-13140,-3258,-26604), Ziel (-2808,-1440,-11556), H 231) auf den Boden (tools/r41_hangar/rueck.py).
 * Messwerte und Bildfolgen: analysis/befunde_runde41/H_hangar.md Abschnitt 3.
 *
 *   h 0..1   Spider-Man hockt links (Schatten (29..31,190) -> (-12470,-17540)), Birkin steht (Fuesse (191..194,205))
 *   h 2..11  Ansprung: Schatten (49,194) h4 -> (71,193) h6 -> (100,192) h8, d.h. 225 -> 360 -> 485 Einheiten je Bild
 *   h12      Beruehrung auf Brusthoehe, h12..16 hochgerissen, h16..71 ueber dem Kopf gehalten (Rumpf Bild-y 15..110)
 *   h72..76  Birkin dreht nach links und schmettert ihn vor sich auf den Boden (Lage (115..150, 203..215))
 *   h77..79  liegt, strampelt; h80..86 springt auf und rechts an der Kamera vorbei aus dem Bild
 *   h88..135 Birkin geht auf die Kamera zu (Fuesse h90 (189,204) -> h135 (190,222): ~1000 Einheiten, 22 je Bild)
 *
 * Alle Zahlen dieses Blocks sind Videomessung oder Gestaltung, KEIN Auslieferungscode (die Szene gibt es nur im Video). */
#define BF_BIRKIN_X   (-8488)        /* Video h0 Fusspunkt (194,205) */
#define BF_BIRKIN_Z  (-21651)
#define BF_SM_X      (-12470)        /* Video h0..2 Schatten (29..31,190) */
#define BF_SM_Z      (-17540)
#define BF_HANGAR_CUT    12          /* Video: die Szene laeuft vollstaendig in Cut 12 */

/* Clips. Birkin EM30: 0x0B = GRAB-THROW (ACT[5] @0x80117d30), Wurf bei Bild 0x2C; 1 = Gang (WALK @0x80117220); 0 = Stand
 * (INIT @0x8011683c). Spider-Man EM2B (Bank 0): 0x16 = Sprung (B[11] @0x80113560 `+0x94 = 0x16`), 7 = gekruemmte Reaktion
 * (HURT @0x801148e8), 0x0B = Rueckenlage mit Aufstehen (Katalog tools/r41_hangar/clipkatalog.py: B0..30 liegt, B31..39
 * richtet sich auf). Die Rollen in der Szene sind Gestaltung. Voraussetzung an jedes EM30-Ersatzmodell: Knochen 15 =
 * haltende Hand (im Sichel-Modell die Sichelhand), Clips 0/1/0x0B wie EM30; Spider-Man EM2B: Knochen 20 = Griffpunkt. */
#define BF_CLIP_GRIFF    0x0B
#define BF_WURF_BILD     0x2C
/* Haltende Hand = SICHELHAND, Knochen 15 (Kette 13-14-15 am Brustknochen 8, rel z -988 = rechte Seite; im Sichel-Modell
 * EM30_HANGAR traegt Mesh 15 die Klinge). Runde 41 Spur SF Nachbesserung 1 (Dossier SF_sichel.md N1.2): das Video haelt
 * den Spider-Man ab h16 bis zum Wurf mit dem erhobenen Bild-linken = rechten Arm; das rot-weisse Glied, das h18-h28
 * Bild-links bis zum Boden haengt, ist der Schwanz (wandert h30-h34 zwischen die Knie, h38-h70 nach Bild-rechts). */
#define BF_KLAUE         15
#define BF_SM_SPRUNG     0x16
#define BF_SM_GRIFF      7
#define BF_SM_LIEGT      0x0B
/* Griffpunkt am Spider-Man (PORT-WAHL nach Videomessung, Dossier SF_sichel.md N1.3): die Sichelhand haelt ihn am Bein -
 * EM2B-Knochen 20 (Knie der Beinkette 19-20-21 an der Wurzel; Clip 7 rel (859,-1045,-369): ~750 unter dem Becken, ~860 vor
 * ihm). Der Knochen liegt jeden Takt GENAU auf der Hand (bf_klauenziel), kein seitlicher Versatz. Damit steht der Rumpf
 * ueber der Hand und von Birkin weg: Video h16..70 Spider-Man oben links, oben am Bildrand abgeschnitten (Rumpf Bild
 * (110..140, 40..65)); mit dem Becken auf der Hand (Rumpf ~1370 tiefer) lag er 30..40 Bildpunkte zu tief.
 * Vorher (Spur H): Aktor-y = Klaue + 1200 und 1000 seitlich (BF_HALTE_SEITE) - der Spider-Man hing neben der Hand. */
#define BF_SM_GRIFF_KNOCHEN 20
/* Ansprung (Gestaltung nach Videomessung N1.4, Bahn tools/r41_sichel/bahn.py): Video h2->h3 hebt der Spider-Man in EINEM
 * Bild ab (Mitte Bild-y 131 -> 110), fliegt dann fast eben und gleich schnell (Bild-x 35, 43, 59, 74, 92, 111 je Bild h3..h8)
 * und prallt bei h10..11 an. Waagerecht: kurzes Anlaufen (erste 30 % quadratisch), dann gleichmaessig bis zur Hand
 * (kein Abbremsen vor dem Aufprall, vorher smoothstep; Lauf n1 mit 20 %: Bild-x h4 57 statt 43, zu frueh -> 30 %);
 * senkrecht: Bogen mit fruehem Scheitel bei 1/3 der Bahn. */
#define BF_BOGEN         1000        /* Gestaltung: Scheitel ueber der Geraden (Video h3..h8 Mitte ~25 Bildpunkte ueber h0) */
/* Zeitplan, h = Videobild (0 = erstes Bild in Cut 12). */
#define BF_H_SPRUNG_AB    2
#define BF_H_BERUEHRUNG  12
#define BF_H_WURF        72
#define BF_H_AUFSCHLAG   76
#define BF_H_AUFSPRINGEN 80          /* Video h80: Beine schon hoch, h82..86 rechts hinaus; Lauf n5 (Start h82) war 2 Bilder spaet */
#define BF_H_WEG         86
#define BF_H_GANG        90
#define BF_H_ENDE       135
/* Birkin-Clip 0x0B Bild je Videobild (stueckweise linear): Griff beginnt h7 (Klaue bei h12 auf Brusthoehe B5, ueber dem
 * Kopf B10 bei h17). Das Video haelt 53 Bilder (h17..70), der Clip hat fuer das Halten nur B10..37 (Klaue oben,
 * -3586..-3796). Ein langsamerer Vorschub (0.57 Bild/Takt) liess die Pose in der Aufnahme nur alle 3..4 Bilder wechseln
 * (Lauf n4, gd_0281..0299) - ruckelig. Deshalb volle Rate mit Pendel im Halte-Abschnitt; Wurf B44 bei h72 (Wurfbild
 * 0x2C), Aufschlag B55 bei h76, Aufrichten bis B70 bei h90, dann Ueberblendung in den Gang.
 * Runde 41 SF N1.3: Pendel nur noch B10..28. Die Sichelhand liegt bei Gier Bezug-1270 in B10..24 bei Bild-x 131..136,
 * B28 143, B34 154, B37 158 (Rechnung tools/r41_sichel/hand.py); das Video haelt den Spider-Man bei x 113 (h40) -> 136
 * (h60), also links und nach rechts wandernd -> B10->28 (h17..35), zurueck ->12 (h51), wieder ->27 (h66). */
#define BF_BILD_N 8
static const int16_t k_birkin_bild[BF_BILD_N][2] = {
    {   7,  0 }, {  17, 10 }, {  35, 28 }, {  51, 12 }, {  66, 27 }, { BF_H_WURF, BF_WURF_BILD }, { BF_H_AUFSCHLAG, 55 },
    { BF_H_GANG, 70 } };
/* Birkin-Gier relativ zum Blick auf den Spider-Man-Start (Q12, 4096 = 360 Grad). Gestaltung nach dem Video: Birkin steht
 * zur Kamera gedreht (die haltende rechte Klaue liegt dann links im Bild, Gier 1300 = Bezug - 1270,
 * Video Rumpf x 110..145 bis h70), dreht h66..72 nach links und schmettert in Richtung des Ansprungs (Klaue B50 bei Gier
 * 2570 Bild (148,188), Video h72 Rumpf ganz links (60..150), Aufschlag (115..150, 203..215)). */
static const int16_t k_birkin_gier[][2] = {
    {   0, -1270 }, {  66, -1270 }, {  72,     0 }, { BF_H_AUFSCHLAG, 0 } };
#define BF_GANG_SCHRITT  22          /* Video h90..135: ~1000 Einheiten in 45 Bildern */
/* Schwanz-Seitenvorgabe (Runde 41 SF N1.9, PORT-WAHL nach Videomessung; Feder-Kette engine/src/schwanz_r41.c):
 * Video 4 Hangar, rote Flecken unter der Huefte: h18-h24 Bild-links bis zum Boden (x 223-331), h26-h28 links oben
 * (x 230-263), h30-h34 zwischen den Knien (x 327-374), h38-h48 Bild-rechts auf Hufthoehe (x 402-430), h60-h70 Bild-rechts
 * bis zum Boden. Birkin steht zur Kamera: Bild-links = seine rechte Seite. In Clip 0x0B liegt die Schwanzspitze auf der
 * linken Seite (Modell z +1500..+1850, B5..B28) -> Vorgabe 4000 nach rechts ab dem Aufprall (die Kette ist dann gestreckt,
 * Spitze gemessen z < -800, test_schwanz_r41; 3200 erreichte nur -721), ab h27 zurueck,
 * Mitte bei h30, kurz ueber die Ruhelage hinaus (Schwung), ab h46 frei. Werte in Einheiten an der Spitze. */
static const int16_t k_schwanz_seite[][2] = {
    { 11, 0 }, { 16, 4000 }, { 27, 4000 }, { 30, 0 }, { 36, -900 }, { 46, 0 } };

/* ---- Runde 41 Spur H "hangar": Ausloeser, Riegel, Kamera ----------------------------------------
 * GEMESSEN (Dossier analysis/befunde_runde41/H_hangar.md 1.2): Cut 12 ist in ROOM50A0/50A1 (beide RDT bytegleich) NICHT
 * erreichbar - keine RVD-Zone schaltet nach Cut 12 (einzige Saetze mit Cut 12/13 sind die Platzhalter rvd 39 @0x564 /
 * rvd 40 @0x578, Kasten x/z -25000..-21000), kein Cut_chg im SCD. Die alte Szene wartete auf Cut 12 und stand deshalb nie
 * an; Birkin und Spider-Man standen ab Raumeintritt mit Clip 0 Bild 0 im Raum (Lauf v1: 900 Bilder st=0 mo=0 af=0).
 * Derselbe Platzhalter ist das Anker-Viereck von Cut 12 (erster RVD-Satz mit cam_from 12, FUN_80014324 @0x80014324 /
 * FUN_800142f4 -> DAT_800ac794): jede Figur ausserhalb x/z -25000..-21000 wird in Cut 12 gecullt (main.c, Region-Cull).
 *
 * PORT-WAHL (Gestaltung, kein Original): die Szene ist eine Zwischensequenz in Cut 12 wie im 80%-Video (Complete Disc 4
 * 0:46-0:51: Letterbox-Balken, kein Spieler im Bild, Birkin kommt am Ende auf die Kamera zu).
 *   - Birkin und Spider-Man gibt es erst ab dem Ausloeser (vorher kein Aktor, kein Schatten, keine Kollision).
 *   - Ausloeser: der angeforderte Schnitt (g_scd.cam_id, RVD-Scan @0x80014300 schreibt nur ihn) zeigt eine der beiden
 *     Szenenfiguren: Lage im Anker-Viereck des Schnitts UND ein Punkt der Figur im Bild (bf_szene_im_schnitt). ROOM50A0:
 *     Cut 4, 5 (beide) und 6 (Birkin); Weg aus 5030 (Nordtuer, Cut 0) loest in Cut 4 aus, aus 4070 (Westtuer, Cut 11) in Cut 6.
 *   - Riegel wie die Original-Zwischensequenzen: Set(2,7) + Set(1,27) (Form ROOM1150 sub08 @0x01114/@0x01118; Pad-Sperre
 *     @0x800304f4-0x80030500, Letterbox FUN_80021a0c), Ende Set(2,7,0) + Set(1,27,0) (@0x012E4/@0x012E8).
 *   - Kamera: Cut_chg-Zwilling auf Cut 12 (LAB_800402a0: work_vars[0x0C] @0x800402ec, work_vars[0x0A] @0x800402fc,
 *     Zonen aus @0x800402d4), am Ende Cut_chg auf den angeforderten Schnitt des Spielers + Zonen an (Form ROOM1150 sub08
 *     `29 05` @0x012C4, Cut_auto `3c 01` @0x012E2), wie gatorszene_r41.c kam_uebergabe.
 *   - Der Spieler muss frei sein (Zustand 1, nicht gegriffen, keine andere Zwischensequenz) - sonst wartet der Ausloeser;
 *     bis dahin gibt es keine Figuren, die starr stehen koennten. */
#define BF_VORLAUF       6           /* Standbilder des Kamera-Applys (kamerahalt_r41.c KH_STANDBILDER): Videobild h0 = Takt 6 */
#define BF_KOPF_DY       2000        /* Gestaltung: Kopfpunkt des Sichttests ueber dem Fuss */

/* Phasen (re15_bf_hangar_phase). */
enum { BF_H_WARTEN = 0, BF_H_VORLAUF, BF_H_SPRUNG, BF_H_GRIFF, BF_H_STURZ, BF_H_LIEGT, BF_H_ABGANG, BF_H_FREI };

static struct {
    uint32_t raum;
    int      birkin, sm;           /* Slots, -1 = nicht gesetzt */
    int      phase;                /* BF_H_* */
    int      t;
    int32_t  sx, sz;               /* Sprungstart */
    int32_t  vy;                   /* Sturz */
    int      birkin_start;         /* Birkin-Clip laeuft */
    int      riegel;               /* 1 = die Szene haelt Set(2,7)/Set(1,27) */
    int      cut_zurueck;          /* angeforderter Schnitt beim Ausloesen = Ziel am Szenenende */
    int16_t  gier0;                /* Birkins Blick auf den Spider-Man-Start (Bezug von k_birkin_gier) */
    int32_t  lx, lz;               /* Spider-Man: Aufschlagstelle */
    int32_t  ex, ez;               /* Spider-Man: Fluchtziel neben der Kamera */
    int32_t  ziel[3];              /* geglaettetes Klauenziel des Ansprungs */
} s_h = { 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 0 } };
static int s_szene_gespielt = 0;
static struct { int slot, phase, t; } s_rs = { -1, 0, 0 };   /* Rueckstoss-Szene 5090, s. unten */
static int s_kam_g5 = 0;       /* G5-Auftritt: Kamera aufs Zugende halten (Slot+1, 0 = aus) */
static void bf_kamera(int cut);
static void bf_kamera_setzen(int cut);
static void bf_kamera_frei(void);
static void bf_zucken(re15_actor_t *e);

/* Riegel der Hangar-Szene (Form s. Kopf "Spur H"). */
static void bf_riegel(int an)
{
    re15_game_flag_set(2, 7, an ? 1 : 0);                /* ROOM1150 sub08 @0x01114 / @0x012E4 */
    re15_game_flag_set(1, 27, an ? 1 : 0);               /* ROOM1150 sub08 @0x01118 / @0x012E8 */
    s_h.riegel = an ? 1 : 0;
}

void re15_bf_reset(void)
{
    s_bf_mutiert = 0; s_kam_g5 = 0;
    re15_bb_reset();
    s_rs.slot = -1; s_rs.phase = 0; s_rs.t = 0;
    /* Raumwechsel mitten in der Hangar-Szene (nur ueber das Debug-Menue moeglich, der Spieler steht im Riegel): den
     * Riegel nicht mitnehmen, sonst bliebe der Spieler im naechsten Raum gesperrt. */
    if (s_h.riegel) { bf_riegel(0); g_scd.cut_auto_enabled = 1; }
    re15_schwanz_r41_reset();                            /* Runde 41 SF N1.9: Feder-Ketten und Vorgaben des Raums */
    s_h.raum = 0; s_h.birkin = s_h.sm = -1; s_h.phase = BF_H_WARTEN; s_h.t = 0; s_h.birkin_start = 0;
    s_h.cut_zurueck = 0;
}

int re15_bf_hangar_phase(void) { return s_h.phase; }
int re15_bf_hangar_slot(int birkin) { return birkin ? s_h.birkin : s_h.sm; }
int re15_bf_hangar_riegel(void) { return s_h.riegel; }
void re15_bf_hangar_vergessen(void) { s_szene_gespielt = 0; }

static void bf_rueckstoss_tick(void);
int re15_bf_szene_besitzt(int slot)
{
    if (slot >= 0 && slot == s_rs.slot) return 1;       /* Rueckstoss-Szene 5090 */
    if (slot >= 0 && re15_bb_pose_aktiv() && g_actors[slot].type == 0x30u) return 1;   /* Messhaken: Pose steht fest */
    if (s_h.raum == 0 || slot < 0) return 0;
    if (slot == s_h.sm) return 1;                       /* der Spider-Man gehoert bis zum Raumwechsel der Szene */
    return slot == s_h.birkin && s_h.phase < BF_H_FREI;     /* bis zum Szenenende fuehrt der Zeitplan */
}

/* Region-Cull-Ausnahme fuer den Zeichner (main.c): waehrend die Szene Cut 12 haelt, zeigt sie ihre beiden Figuren, obwohl
 * deren Lage ausserhalb des Platzhalter-Ankers von Cut 12 liegt (rvd 39 @0x564). PORT-WAHL, s. Kopf "Spur H". */
int re15_bf_szene_zeigt(const re15_actor_t *a)
{
    if (!a || !s_h.riegel || s_h.raum == 0) return 0;
    if ((int)g_scd.work_vars[0x0A] != BF_HANGAR_CUT) return 0;
    const int slot = (int)(a - g_actors);
    return slot >= 1 && (slot == s_h.birkin || slot == s_h.sm);
}

static int16_t bf_blick(const re15_actor_t *e, int32_t tx, int32_t tz)
{
    /* dieselbe Peilung wie re15_enemy_steer_point (enemy_ai_common.c) */
    return (int16_t)(((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff);
}

static int bf_neuer_aktor(uint8_t type, int32_t x, int32_t z, uint8_t grid)
{
    int frei = -1;
    for (int i = RE15_ACTOR_MAX - 1; i >= 1; i--)        /* rueckwaerts wie der Alligator (main.c) */
        if (!g_actors[i].active) { frei = i; break; }
    if (frei < 0) return -1;
    re15_actor_t *a = &g_actors[frei];
    memset(a, 0, sizeof *a);
    a->active = 1; a->type = type;
    a->x = x; a->y = 0; a->z = z;
    a->grid_id = grid;
    a->em_flag_id = 0xFF;                                /* kein Kill-Persist */
    re15_enemy_apply_hitbox(a, type);
    return frei;
}

static void bf_clip(re15_actor_t *e, uint8_t c) { e->motion = c; e->anim_frame = 0; e->anim_frac = 7; }

/* ---- Hangar: Sichttest, Start, Ende (Spur H, s. Kopf) ------------------------------------------------------------ */
/* Bildpunkt eines Weltpunkts in der RID-Kamera `cut` (Projektion wie der Zeichner: sx = 160 + H*x/z, H = fov>>7,
 * camera_common.c re15_camera_build_view = FUN_80053ca4). 1 = vor der Kamera und im 320x240-Bild. */
static int bf_punkt_im_bild(const re15_camera_cut_t *c, int32_t x, int32_t y, int32_t z)
{
    re15_camera_view_t v;
    if (re15_camera_build_view(c, &v) != 0) return 0;
    int64_t vx = (((int64_t)v.rot[0] * x + (int64_t)v.rot[1] * y + (int64_t)v.rot[2] * z) >> 12) + v.trans[0];
    int64_t vy = (((int64_t)v.rot[3] * x + (int64_t)v.rot[4] * y + (int64_t)v.rot[5] * z) >> 12) + v.trans[1];
    int64_t vz = (((int64_t)v.rot[6] * x + (int64_t)v.rot[7] * y + (int64_t)v.rot[8] * z) >> 12) + v.trans[2];
    if (vz <= 0) return 0;
    int64_t sx = 160 + vx * v.fov_screen_dist / vz, sy = 120 + vy * v.fov_screen_dist / vz;
    return sx >= 0 && sx < 320 && sy >= 0 && sy < 240;
}

/* Waere eine Szenenfigur (Fuss- oder Kopfpunkt) im Schnitt `cut` zu sehen? Lage im Anker-Viereck des Schnitts (derselbe Test
 * wie der Region-Cull des Zeichners, FUN_80014324 + FUN_80014368) UND ein Punkt im Bild. */
static int bf_figur_im_schnitt(int cut, int32_t x, int32_t z)
{
    int16_t qx[4], qz[4];
    if (!g_room_rdt_ok || !g_room_rdt.cuts || cut < 0 || cut >= g_room_rdt.cut_count) return 0;
    if (re15_rdt_get_region_quad(&g_room_rdt, cut, qx, qz) && !re15_aot_point_in_quad(x, z, qx, qz)) return 0;
    const re15_camera_cut_t *c = &g_room_rdt.cuts[cut];
    return bf_punkt_im_bild(c, x, 0, z) || bf_punkt_im_bild(c, x, -BF_KOPF_DY, z);
}

int re15_bf_szene_im_schnitt(int cut)
{
    if (cut == BF_HANGAR_CUT) return 0;                  /* der Szenenschnitt selbst loest nicht aus */
    return bf_figur_im_schnitt(cut, BF_BIRKIN_X, BF_BIRKIN_Z) || bf_figur_im_schnitt(cut, BF_SM_X, BF_SM_Z);
}

/* Ausloeser: angeforderter Schnitt zeigt die Szene, Spieler frei. */
static int bf_hangar_ausloeser(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!re15_bf_szene_im_schnitt((int)g_scd.cam_id)) return 0;
    if (pl->hp <= 0 || pl->state != 1) return 0;         /* Zustand 1 = Pad-Zweig (@0x80073f90[1]), kein Treffer/Tod/Skript */
    if (re15_player_is_grabbed() || re15_cine_active()) return 0;
    return 1;
}

/* Birkin an seinem Szenenplatz. grid 0x10 = Form-2-Bit (@0x801164e8 sperrt die HP-Mutation), Nibble 0 -> INIT-HUB
 * (@0x80116880-8b8): ein gewoehnlicher Angreifer, sobald die Szene ihn freigibt (state 0). */
static int bf_hangar_birkin_setzen(void)
{
    int s = bf_neuer_aktor(0x30u, BF_BIRKIN_X, BF_BIRKIN_Z, 0x10u);
    if (s < 0) return -1;
    re15_actor_t *b = &g_actors[s];
    b->hp = 300;                                         /* INIT-Wert @0x8011690c, laeuft im INIT erneut */
    b->rot_y = bf_blick(b, BF_SM_X, BF_SM_Z);
    bf_clip(b, 0);                                       /* Clip 0 wie INIT @0x8011683c */
    return s;
}

/* Stueckweise lineare Tabelle (h, Wert), aussen gehalten. */
static int bf_tab(const int16_t (*t)[2], int n, int h)
{
    if (h <= t[0][0]) return t[0][1];
    for (int i = 1; i < n; i++)
        if (h <= t[i][0]) return t[i - 1][1] + (t[i][1] - t[i - 1][1]) * (h - t[i - 1][0]) / (t[i][0] - t[i - 1][0]);
    return t[n - 1][1];
}
/* Glatter Verlauf 0..4096 (smoothstep), u in Q12. */
static int32_t bf_glatt(int32_t u)
{
    if (u <= 0) return 0;
    if (u >= 4096) return 4096;
    return (int32_t)(((int64_t)u * u * (3 * 4096 - 2 * u)) >> 24);
}
static int32_t bf_mix(int32_t a, int32_t b, int32_t w) { return a + (int32_t)(((int64_t)(b - a) * w) >> 12); }
/* Clip-Bild direkt setzen (Zeitplan); die Ueberblendung laeuft wie im Vorschub weiter ab (re15_birkin_anim: +0x8f--). */
static void bf_bild(re15_actor_t *e, uint8_t clip, int bild)
{
    int fc;
    if (e->motion != clip) { e->motion = clip; e->anim_frac = 7; }
    else if (e->anim_frac > 0) e->anim_frac--;
    fc = re15_r38_birkin_fc(e);
    if (bild < 0) bild = 0;
    if (bild > fc - 1) bild = fc - 1;
    e->anim_frame = (uint8_t)bild;
}

static const re15_camera_cut_t *bf_szenenkamera(void)
{
    return (g_room_rdt_ok && g_room_rdt.cuts && BF_HANGAR_CUT < g_room_rdt.cut_count) ? &g_room_rdt.cuts[BF_HANGAR_CUT] : NULL;
}

static void bf_hangar_starten(void)
{
    s_h.birkin = bf_hangar_birkin_setzen();
    s_h.sm = (s_h.birkin >= 0) ? bf_neuer_aktor(0x2Bu, BF_SM_X, BF_SM_Z, 0x00u) : -1;
    if (s_h.birkin < 0 || s_h.sm < 0) {                  /* kein Platz: ohne Szene, Birkin (falls gesetzt) greift an */
        if (s_h.birkin >= 0) g_actors[s_h.birkin].state = 0;
        s_h.sm = -1; s_h.phase = BF_H_FREI; s_szene_gespielt = 1;
        return;
    }
    re15_actor_t *b = &g_actors[s_h.birkin], *m = &g_actors[s_h.sm];
    s_h.gier0 = b->rot_y;
    b->rot_y = (int16_t)((s_h.gier0 + bf_tab(k_birkin_gier, 4, 0)) & 0x0fff);
    m->rot_y = bf_blick(m, b->x, b->z);
    m->hp = 100;
    bf_clip(m, BF_SM_SPRUNG); m->anim_frac = 0;          /* hockt: Bild 0 des Sprungs (Video h0) */
    s_h.cut_zurueck = (int)g_scd.cam_id;                 /* der Schnitt, den der RVD-Scan gerade angefordert hat */
    bf_riegel(1);
    /* Cut_chg-Zwilling (LAB_800402a0) auf den Szenenschnitt */
    g_scd.cam_id_prev        = (uint8_t)g_scd.work_vars[0x0A];   /* @0x800402c0 / @0x800402e4 */
    g_scd.cam_id             = (uint8_t)BF_HANGAR_CUT;
    g_scd.cam_arg2           = 0;
    g_scd.cam_arg3           = 0;
    g_scd.cam_change_pending = 1;
    g_scd.work_vars[0x0C]    = g_scd.work_vars[0x0A];            /* @0x800402ec */
    g_scd.work_vars[0x0A]    = (int16_t)BF_HANGAR_CUT;           /* @0x800402fc */
    g_scd.cut_auto_enabled   = 0;                                /* @0x800402d4 `ori ...,0x100` */
    s_h.phase = BF_H_VORLAUF; s_h.t = 0;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[hangar] ROOM%04X Szene Start: Schnitt %d angefordert -> Cut %d, Birkin Slot %d, Spider-Man Slot %d, "
                    "Spieler (%d,%d)\n", (unsigned)g_current_room_id, s_h.cut_zurueck, BF_HANGAR_CUT, s_h.birkin, s_h.sm,
            (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z);
#endif
}

/* Szenenende: Schnitt zurueck auf den Schnitt des Spielers + Zonen an (ROOM1150 sub08 `29 05` @0x012C4, `3c 01` @0x012E2),
 * Riegel auf (@0x012E4/@0x012E8). Birkin geht in seine 0x30-KI (state 0 -> INIT -> HUB). */
static void bf_hangar_beenden(void)
{
    if (s_h.riegel) {
        const int cut = s_h.cut_zurueck;
        g_scd.cam_id_prev        = (uint8_t)g_scd.work_vars[0x0A];
        g_scd.cam_id             = (uint8_t)cut;
        g_scd.cam_arg2           = 0;
        g_scd.cam_arg3           = 0;
        g_scd.cam_change_pending = 1;
        g_scd.work_vars[0x0C]    = g_scd.work_vars[0x0A];
        g_scd.work_vars[0x0A]    = (int16_t)cut;
        g_scd.cut_auto_enabled   = 1;
        bf_riegel(0);
    }
    if (s_h.birkin >= 0 && g_actors[s_h.birkin].active && g_actors[s_h.birkin].type == 0x30u) {
        re15_actor_t *b = &g_actors[s_h.birkin];
        b->state = 0; b->sub_state_1 = b->sub_state_2 = b->sub_state_3 = 0;
    }
    if (s_h.sm >= 0 && g_actors[s_h.sm].active && g_actors[s_h.sm].type == 0x2Bu) g_actors[s_h.sm].active = 0;
    re15_schwanz_r41_seite(s_h.birkin, 0);
    s_h.sm = -1;
    s_h.phase = BF_H_FREI;
    s_szene_gespielt = 1;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[hangar] ROOM%04X Szene Ende: Cut %d, Birkin Zustand %d\n", (unsigned)g_current_room_id,
            s_h.cut_zurueck, (s_h.birkin >= 0) ? (int)g_actors[s_h.birkin].state : -1);
#endif
}

/* Aktorlage des Spider-Man, bei der sein Griffknochen (BF_SM_GRIFF_KNOCHEN, aktuelle Pose und Gier) genau auf Birkins
 * Sichelhand (Knochen 15, aktuelles Bild) liegt. Kein Versatz: Hand - (Griffknochen - Wurzel). */
static void bf_klauenziel(re15_actor_t *b, re15_actor_t *m, int32_t z[3])
{
    int32_t g[3];
    re15_enemy_bone_world_pos(b, BF_KLAUE, z);
    re15_enemy_bone_world_pos(m, BF_SM_GRIFF_KNOCHEN, g);
    z[0] -= g[0] - m->x;
    z[1] -= g[1] - m->y;
    z[2] -= g[2] - m->z;
}
/* Waagerechter Bahnanteil des Ansprungs (Q12): erste 30 % quadratisch anlaufen, dann gleich schnell (Kopf BF_BOGEN). */
static int32_t bf_anlauf(int32_t lin)
{
    const int32_t a = 1229;                              /* 0.3 in Q12: Video h2..h4 nur 15 % des Wegs (Bild-x 28 -> 43 von 28..125) */
    if (lin <= 0) return 0;
    if (lin >= 4096) return 4096;
    if (lin < a) return (int32_t)(((int64_t)lin * lin / (2 * a)) * 4096 / (4096 - a / 2));
    return (int32_t)((int64_t)(lin - a / 2) * 4096 / (4096 - a / 2));
}
/* Hoehenbogen des Ansprungs (Q12, 0..4096): 27/4 * u * (1-u)^2, Scheitel bei u = 1/3 (frueher Hochsprung wie Video h2->h3). */
static int32_t bf_bogen(int32_t lin)
{
    const int64_t r = 4096 - lin;
    if (lin <= 0 || lin >= 4096) return 0;
    return (int32_t)(27 * (int64_t)lin * r / 4096 * r / 4096 / 4);
}

/* Ein Takt der Hangar-Szene (Zeitplan nach dem Video, Kopf "Hangar-Szene"). */
static void bf_hangar_takt(re15_actor_t *b, re15_actor_t *m)
{
    const int h = s_h.t - BF_VORLAUF;                    /* Videobild */
    int32_t k[3];
    s_h.t++;
    re15_schwanz_r41_seite(s_h.birkin, bf_tab(k_schwanz_seite, 6, h));
    /* --- Birkin: Clip, Gier ------------------------------------------------------------------------------------- */
    if (h < BF_H_GANG) {
        if (h < k_birkin_bild[0][0]) re15_r38_birkin_anim(b);             /* Stand, Clip 0 laeuft */
        else bf_bild(b, BF_CLIP_GRIFF, bf_tab(k_birkin_bild, BF_BILD_N, h));
        b->rot_y = (int16_t)((s_h.gier0 + bf_tab(k_birkin_gier, 4, h)) & 0x0fff);
        if (h > BF_H_AUFSCHLAG) {                                         /* Video h77..90: richtet sich auf, dreht zur Kamera */
            const re15_camera_cut_t *c = bf_szenenkamera();
            if (c) {
                int d = (bf_blick(b, c->pos_x, c->pos_z) - s_h.gier0) & 0x0fff;
                if (d > 0x800) d -= 0x1000;
                b->rot_y = (int16_t)((s_h.gier0 + d * (h - BF_H_AUFSCHLAG) / (BF_H_GANG - BF_H_AUFSCHLAG)) & 0x0fff);
            }
        }
    } else {                                                              /* Gang auf die Kamera zu (Video h88..135) */
        const re15_camera_cut_t *c = bf_szenenkamera();
        if (h == BF_H_GANG) { bf_clip(b, 1); b->anim_frac = 0xf; }        /* WALK-Clip 1 wie @0x80117220 */
        else re15_r38_birkin_anim(b);
        if (c) {
            int d = (bf_blick(b, c->pos_x, c->pos_z) - b->rot_y) & 0x0fff;
            if (d > 0x800) d -= 0x1000;
            if (d > 0x20) d = 0x20;                                       /* Lenkrate 0x20 wie WALK @0x80117268 */
            if (d < -0x20) d = -0x20;
            b->rot_y = (int16_t)((b->rot_y + d) & 0x0fff);
        }
        b->x += (int32_t)(((int64_t)re15_cos_q12(b->rot_y) * BF_GANG_SCHRITT) >> 12);
        b->z -= (int32_t)(((int64_t)re15_sin_q12(b->rot_y) * BF_GANG_SCHRITT) >> 12);
    }
#ifdef RE15_PLATFORM_PC
    {   static int log = -1;
        if (log < 0) log = getenv("RE15_HANGAR_LOG") ? 1 : 0;   /* Messzeile je Szenentakt, kein Verhalten */
        if (log) fprintf(stderr, "[hangar-takt] h%d F%u B clip %d bild %d gier %d (%d,%d) SM %s\n", h,
                         (unsigned)g_engine.frame_count, b->motion, b->anim_frame, b->rot_y, (int)b->x, (int)b->z,
                         m ? "da" : "weg");
        if (log && m) fprintf(stderr, "[hangar-takt] h%d SM clip %d bild %d (%d,%d,%d) gier %d\n", h, m->motion,
                              m->anim_frame, (int)m->x, (int)m->y, (int)m->z, m->rot_y);
        {   int32_t sp[3];                                  /* Runde 41 SF N1.9: Schwanzspitze (Stand des letzten Zeichnens) */
            if (log && re15_schwanz_r41_gelenk(s_h.birkin, 7, sp))
                fprintf(stderr, "[hangar-takt] h%d Schwanzspitze (%d,%d,%d)\n", h, (int)sp[0], (int)sp[1], (int)sp[2]); } }
#endif
    /* --- Spider-Man ------------------------------------------------------------------------------------------- */
    if (!m) return;
    if (h < BF_H_SPRUNG_AB) {                                             /* hockt (Video h0..1) */
        bf_bild(m, BF_SM_SPRUNG, h < 0 ? 0 : h);
        m->rot_y = bf_blick(m, b->x, b->z);
    } else if (h < BF_H_BERUEHRUNG) {                                     /* Ansprung h2..11: Bahn auf die Sichelhand */
        const int32_t lin = (int32_t)((h - BF_H_SPRUNG_AB + 1) * 4096 / (BF_H_BERUEHRUNG - BF_H_SPRUNG_AB));
        const int32_t u = bf_anlauf(lin);
        bf_bild(m, BF_SM_SPRUNG, BF_H_SPRUNG_AB + (h - BF_H_SPRUNG_AB) * 12 / 10);   /* Sprung-Clip B2..14 ueber 10 Bilder */
        m->rot_y = bf_blick(m, b->x, b->z);
        /* Das Handziel springt beim Clipwechsel Birkins (Clip 0 -> 0x0B bei h7, Lauf Unit-Test: 1246 Einheiten in einem
         * Takt); die Bahn zielt deshalb auf ein geglaettetes Ziel (halber Abstand je Takt). */
        bf_klauenziel(b, m, k);
        if (h == BF_H_SPRUNG_AB) { s_h.ziel[0] = k[0]; s_h.ziel[1] = k[1]; s_h.ziel[2] = k[2]; }
        for (int i = 0; i < 3; i++) s_h.ziel[i] = bf_mix(s_h.ziel[i], k[i], 2048);
        m->x = bf_mix(BF_SM_X, s_h.ziel[0], u);
        m->z = bf_mix(BF_SM_Z, s_h.ziel[2], u);
        m->y = bf_mix(0, s_h.ziel[1], u) - (int32_t)(((int64_t)BF_BOGEN * bf_bogen(lin)) >> 12);
    } else if (h < BF_H_AUFSCHLAG - 2) {                                  /* gehalten, Wurf bis kurz vor dem Aufschlag */
        if (h == BF_H_BERUEHRUNG) bf_clip(m, BF_SM_GRIFF);
        else re15_r38_birkin_anim(m);                                     /* Zappeln: Clip 7 laeuft im Kreis */
        m->rot_y = bf_blick(m, b->x, b->z);
        bf_klauenziel(b, m, k);
        m->x = bf_mix(m->x, k[0], 2048);                                  /* folgt der Hand geglaettet (halber Abstand je Takt) */
        m->y = bf_mix(m->y, k[1], 2048);
        m->z = bf_mix(m->z, k[2], 2048);
        if (m->y > 0) m->y = 0;                                           /* Hand tief im Wurf: nicht unter den Boden */
    } else if (h < BF_H_AUFSPRINGEN) {                                    /* Aufschlag + am Boden (Video h74..81) */
        if (h == BF_H_AUFSCHLAG - 2) { bf_clip(m, BF_SM_LIEGT); s_h.lx = m->x; s_h.lz = m->z; }
        bf_bild(m, BF_SM_LIEGT, 20 + (h - (BF_H_AUFSCHLAG - 2)) * 19 / (BF_H_AUFSPRINGEN - (BF_H_AUFSCHLAG - 2)));
        m->y = (h >= BF_H_AUFSCHLAG) ? 0 : bf_mix(m->y, 0, 2048);        /* zwei Takte bis auf den Boden */
    } else if (h < BF_H_WEG) {                                            /* springt auf und rechts an der Kamera vorbei */
        const int32_t lin = (int32_t)((h - BF_H_AUFSPRINGEN + 1) * 4096 / (BF_H_WEG - BF_H_AUFSPRINGEN));
        if (h == BF_H_AUFSPRINGEN) {
            const re15_camera_cut_t *c = bf_szenenkamera();
            /* Ziel am rechten Bildrand 3500 vor der Kamera (Bild-x 338, Boden): Video h82..86 Bein bei x 220..290 -> 240..320
             * -> 300..320, danach aus dem Bild. Kamera-Raum (2727, ., 3500) -> Welt (-9069,0,-25486) = Kamera + (4071,.,1118). */
            s_h.ex = c ? c->pos_x + 4071 : s_h.lx + 1300;
            s_h.ez = c ? c->pos_z + 1118 : s_h.lz - 5200;
            m->rot_y = bf_blick(m, s_h.ex, s_h.ez);
        }
        bf_bild(m, BF_SM_SPRUNG, 4 + (h - BF_H_AUFSPRINGEN) * 2);
        m->x = bf_mix(s_h.lx, s_h.ex, bf_glatt(lin));
        m->z = bf_mix(s_h.lz, s_h.ez, bf_glatt(lin));
        m->y = -(int32_t)(((int64_t)1600 * 4 * lin * (4096 - lin)) >> 24);   /* Gestaltung: Bogen 1600 */
    } else if (m->active) {                                               /* aus dem Bild: Spider-Man ist weg */
        m->active = 0;
        s_h.sm = -1;
    }
}

void re15_bf_raumtick(void)
{
    re15_bb_pose_tick();                                 /* Messhaken der Sichtpruefung, im Normalpfad stumm */
    /* MESSHAKEN RE15_BIRKIN_HP_AT=<n>: n KI-Ticks nach Raumeintritt in 5090 die HP des 0x30 auf 0 (Sichtpruefung
     * der Rueckstoss-Szene ohne Durchspielen). Der Debug-Sprung setzt keinen Tuer-Schnitt, die Zonen laufen dann
     * nie; RE15_BIRKIN_CUT=<n> stellt beim Raumeintritt den Schnitt (12 = Kampfstart sub01 @0x1290). Stumm im Normalpfad. */
    { static int at = -2; static unsigned raum = 0; static int t = 0;
      if (at == -2) { const char *v = getenv("RE15_BIRKIN_HP_AT"); at = v ? atoi(v) : -1; }
      if (at >= 0 && re15_bf_bahnsteig_aktiv()) {
          if (raum != g_current_room_id) {
              const char *c = getenv("RE15_BIRKIN_CUT");
              raum = g_current_room_id; t = 0;
              if (c) bf_kamera_setzen(atoi(c));          /* Zonen bleiben an wie nach dem echten Kampfstart */
          }
          if (++t == at || (t == 2 * at && s_bf_mutiert))
              for (int i = 1; i < RE15_ACTOR_MAX; i++)
                  if (g_actors[i].active && g_actors[i].type == 0x30u && g_actors[i].state == 1) {
                      g_actors[i].hp = 0; g_actors[i].state = 3; g_actors[i].sub_state_3 = 0; }   /* = Todes-Commit nach Treffer */
      } }
    bf_rueckstoss_tick();
    /* G5-AUFTRITT (wie RE2): die Kamera bleibt aufs Zugende gerichtet, waehrend zuerst die vier Tentakel
     * hereingreifen ([T1] @0x80101214-2e0) und dann den Koerper nachziehen ([T2] Root-Spur +7014).
     * GESTALTUNG (Abnahme 2): die Szenenkamera endet mit [T2], nicht mit Sub 2. Sub 2 dauert ohne Treffer 1126 Bilder
     * (37,5 s), der Spieler hat dabei volle Kontrolle und sah sich oft nicht, ab [T6] (+3946) fuellte der Koerper Cut 11
     * ganz. Ab [T3] laufen die Zonen wieder, der Schnitt kommt aus der Spielerlage (bf_schnitt_aus_lage). */
    if (s_kam_g5) {
        int sub = -1, ph = 0;
        if (!re15_bf_bahnsteig_aktiv()) { bf_kamera_frei(); s_kam_g5 = 0; }
        else if (re15_g5_boss_zustand(&sub, &ph, NULL) && (sub != 2 || ph >= 3)) {
            bf_kamera_setzen(re15_bf_schnitt_aus_lage(g_actors[RE15_ACTOR_SLOT_PLAYER].x));
            bf_kamera_frei(); s_kam_g5 = 0;
        }
        else bf_kamera(0x0B);
    }
    if (!re15_bf_hangar_aktiv()) { if (s_h.raum) re15_bf_reset(); return; }
    if (s_h.raum != g_current_room_id) {
        re15_bf_reset();
        s_h.raum = g_current_room_id;
        if (s_szene_gespielt) {
            /* Wiedereintritt nach der Szene: kein Spider-Man, Birkin sofort als Angreifer (Zustand 0 -> INIT -> HUB). */
            s_h.birkin = bf_hangar_birkin_setzen();
            s_h.phase = BF_H_FREI;
        }
        return;                                          /* sonst: noch keine Figuren, Phase WARTEN */
    }
    if (s_h.phase == BF_H_WARTEN) {
        if (!bf_hangar_ausloeser()) return;
        bf_hangar_starten();
        return;
    }
    if (s_h.phase == BF_H_FREI) return;
    re15_actor_t *b = (s_h.birkin >= 0) ? &g_actors[s_h.birkin] : NULL;
    re15_actor_t *m = (s_h.sm >= 0) ? &g_actors[s_h.sm] : NULL;
    if (!b || !b->active || b->type != 0x30u || (m && (!m->active || m->type != 0x2Bu))) {   /* Figur weg: Szene frei */
        bf_hangar_beenden();
        return;
    }
    if (!re15_enemy_find(0x30u) || (m && !re15_enemy_find(0x2Bu))) return;   /* Banken noch nicht geladen (Lader main.c) */
    bf_hangar_takt(b, m);
    {   const int h = s_h.t - 1 - BF_VORLAUF;            /* Diagnose-Phase des eben gespielten Videobilds */
        s_h.phase = (h < 0) ? BF_H_VORLAUF : (h < BF_H_BERUEHRUNG) ? BF_H_SPRUNG : (h < BF_H_WURF) ? BF_H_GRIFF
                  : (h < BF_H_AUFSCHLAG) ? BF_H_STURZ : (h < BF_H_WEG) ? BF_H_LIEGT : BF_H_ABGANG;
        if (h >= BF_H_ENDE) bf_hangar_beenden(); }
}

/* ---- Bahnsteig: Rueckstoss-Szene Bahnsteig-Form -> G5 -------------------------------------
 * Nutzer 2026-10-30: "Man schiesst Birkin energiemaessig runter. Dann kommt eine Cutscene, wie Birkin mit
 * Schuessen nach hinten aus dem Zug rausgedraengt wird. Dann kommt er mit den Tentakeln in der finalen Form
 * wieder rein - genauso wie in RE2."
 * Ablauf (Gestaltung, keine Original-Szene; ausgeloest beim Todeseintritt des 0x30 in 5090/5091 statt der
 * Kollaps-Kette @0x8011a5d8):
 *   1 SCHIESSEN  der Spieler zielt und feuert selbst (Eingabe R1+Quadrat ueber re15_bf_pad, also der echte
 *                Schuss-Ablauf mit Animation/Ton/Munition), Blick auf Birkin; Birkin zuckt (Flinch-Clip 7,
 *                HURT @0x8011a2e0) und weicht Richtung Zugende zurueck.
 *   2 HINAUS     Birkin passiert das Westende des Wagens (Eckbloecke x -5683, Luecke z -24974..-21990 -
 *                derselbe Weg, auf dem der RE2-G5 hereinkommt, enemy_ai_boss_g5.c @0x801011d0) und
 *                verschwindet; Pause mit Gebruell.
 *   3 ZURUECK    Typwechsel zum G5 (re15_bf_morph_zu_g5): dessen RE2-Auftritt von (-9000,-23400) samt der
 *                vier Tentakel uebernimmt. */
#define BF_RS_AUSGANG_X  (-5683)      /* Eckbloecke Westende ROOM5090 (SCA [21]/[25]) */
#define BF_RS_AUSGANG_Z  (-23482)     /* Mitte der Luecke z -24974..-21990 */
#define BF_RS_MIN_TICKS  90           /* Gestaltung: mindestens 3 s Beschuss */
#define BF_RS_PAUSE      45           /* Gestaltung: Pause hinter dem Zugende */
/* Kameras ROOM5090 (RID): Cut 12 (18526,-24660)->(1496,-20370) = Kampfkamera (sub04 Cut_chg 0x0C @0x1342),
 * Cut 11 (7626,-25360)->(-1504,-21970) = Blick auf das Zugende (Westende x -5683; dieselbe Kamera wie das
 * Kreatur-Standbild V2 1:56.3, SIFT-Beleg analysis/befunde_runde41/R_bahnsteig.md). RE2 room7040 hat eigene Kameras, die der RE1.5-Wagen nicht teilt. */
#define BF_RS_ZUGENDE_KAM (-2500)     /* Gestaltung: ab hier schneidet die Szene aufs Zugende */
/* Zuckt die Form unter dem Beschuss: mit der G4-Bank Clip 8 ab Bild 5 (@0x80107224), sonst EM30 Clip 7 (HURT
 * @0x8011a2e0). */
static void bf_zucken(re15_actor_t *e)
{
    e->anim_flags &= (uint16_t)~0x80u;                   /* Form kann gerade rueckwaerts gelaufen sein */
    if (re15_bb_bank_da(e)) { e->motion = 8; e->anim_frame = 5; e->anim_frac = 7; e->anim_blend_rate = 0x200; }   /* a3 @0x80107244 */
    else { bf_clip(e, 7); e->anim_frac = 0xf; }
}

static void bf_kamera_setzen(int cut)
{
    if ((int)g_scd.work_vars[0x0A] != cut || g_scd.cam_id != cut) { g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 1; }
}

/* Szenen-Kamera. ⛔ Abnahme 1 M9: ROOM5090 sub04 laesst die Zonen an (Cut_auto 01 @0x134C). Stand der Spieler in einer Zone,
 * deren Wechsel aus dem Szenen-Cut fuehrt (RVD @0x490 Cut 11 -> 12, x 2800..4200; @0x4b8 Cut 12 -> 11, x 1203..2703; @0x4cc
 * Cut 12 -> 13, x 14000..15400), setzte der Scan cam_id zurueck und die Szene wieder hin, je Bild: jedes Bild ein Apply,
 * Kamerahalt samt 6 Bilddauern, 4 Bilder je Sekunde, bis zu zwei Minuten Standbild. Wie Cut_chg schaltet die Szene die
 * Zonen ab (Bit 0x100 @0x800402d4, Scan-Gate @0x8001cce0-e4, Port cut_auto_enabled = 0, cut_chg_zwilling in
 * gatorszene_r41.c) und gibt sie am Ende wieder frei (FUN_8004032c @0x8004032c). */
static void bf_kamera(int cut)
{
    g_scd.cut_auto_enabled = 0;
    bf_kamera_setzen(cut);
}

static void bf_kamera_frei(void) { g_scd.cut_auto_enabled = 1; }

/* Schnitt zur Spielerlage ROOM5090/5091, Abnahme 2. Die RVD-Zonen sind Streifen, sie loesen nur aus dem passenden Schnitt aus:
 * @0x490 11->12 x 2800..4200, @0x4b8 12->11 x 1203..2703, @0x4cc 12->13 x 14000..15400, @0x4f4 13->12 x 12249..13873, @0x508
 * 13->14 x 20250..21750, @0x530 14->13 x 18502..20252. Ein fester Cut 12 am Szenenende liess Leon oestlich von x 15400 ohne Zone
 * und ohne Bild (Lauf z_ende F2102). Cut_old (`lbu a0,0x3f7b(a0)` @0x8004033c, `sh a0,0xfe4(at)` @0x80040364) hilft hier
 * nicht: der Spieler bewegt sich waehrend der Szene frei, der gemerkte Schnitt passt dann nicht mehr. Die Grenzen liegen so, dass kein
 * Streifen des gewaehlten Schnitts sofort zurueckschaltet. */
int re15_bf_schnitt_aus_lage(int32_t x)
{
    if (x < 2750)  return 0x0B;
    if (x < 14000) return 0x0C;
    if (x < 20250) return 0x0D;
    return 0x0E;
}

int re15_bf_rueckstoss_phase(void) { return s_rs.slot >= 0 ? s_rs.phase : 0; }

void re15_bf_rueckstoss_start(int slot)
{
    if (s_rs.slot >= 0) return;
    s_rs.slot = slot; s_rs.phase = 1; s_rs.t = 0;
    re15_actor_t *e = &g_actors[slot];
    e->hp = 1;                                           /* bleibt "am Ende", stirbt aber nicht */
    bf_zucken(e);
    bf_kamera(0x0C);
}

uint16_t re15_bf_pad(uint16_t pad, uint16_t *pressed)
{
    if (s_rs.slot < 0 || s_rs.phase != 1) return pad;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[s_rs.slot];
    pl->rot_y = bf_blick(pl, e->x, e->z);                /* Blick auf Birkin */
    if (pressed) *pressed = (uint16_t)(((s_rs.t % 20) == 0) ? 0x8000u : 0u);   /* Quadrat-Flanke: Nachladen */
    return (uint16_t)(0x0800u | 0x8000u);                /* RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE, keine Richtung */
}

static void bf_rueckstoss_tick(void)
{
    if (s_rs.slot < 0) return;
    re15_actor_t *e = &g_actors[s_rs.slot];
    if (!e->active || !re15_bf_bahnsteig_aktiv()) { s_rs.slot = -1; bf_kamera_frei(); return; }
    s_rs.t++;
    e->state = 2; e->hp = 1;                             /* Treffer aendern nichts mehr am Ablauf */
    switch (s_rs.phase) {
    case 1: {
        if (re15_r38_birkin_anim(e)) bf_zucken(e);       /* Zucken bei jedem Treffer: Clip im Kreis */
        int32_t dx = BF_RS_AUSGANG_X - 900 - e->x, dz = BF_RS_AUSGANG_Z - e->z;
        int32_t rest = (int32_t)(dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        int32_t ticks = BF_RS_MIN_TICKS - s_rs.t; if (ticks < 20) ticks = 20;
        int32_t schritt = rest / ticks; if (schritt < 40) schritt = 40;    /* Gestaltung */
        if (rest > 0) {
            e->x += (int32_t)((int64_t)dx * schritt / rest);
            e->z += (int32_t)((int64_t)dz * schritt / rest);
        }
        bf_kamera(e->x < BF_RS_ZUGENDE_KAM ? 0x0B : 0x0C);
        /* rueckwaerts: Gesicht zum Spieler */
        e->rot_y = bf_blick(e, g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        if (e->x <= BF_RS_AUSGANG_X - 600 && s_rs.t >= BF_RS_MIN_TICKS) {
            s_rs.phase = 2; s_rs.t = 0;
            e->no_draw = 1;
            re15_audio_room_se(10);                      /* Gebruell (Birkin-Todes-SE 10, @0x8011a744) */
        }
        break;
    }
    case 2:
        bf_kamera(0x0B);
        if (s_rs.t >= BF_RS_PAUSE) {
            int slot = s_rs.slot;
            s_rs.slot = -1; s_rs.phase = 0;
            e->no_draw = 0;
            re15_bf_morph_zu_g5(slot);
            s_kam_g5 = slot + 1;                         /* Auftritt auf Cut 11 zeigen (bf_g5_kamera_tick) */
        }
        break;
    default: break;
    }
}
