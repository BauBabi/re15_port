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
#include "re15_math.h"         /* Runde 41 SF N2: re15_squareroot0 (Schrittgrenze Spider-Man) */
#include "re15_enemy_ai.h"     /* Runde 41 Spur H: re15_player_is_grabbed */
#include "re15_skeleton.h"     /* Runde 41 Spur H: re15_cos_q12 / re15_sin_q12 (Gang auf die Kamera zu) */
#include "re15_engine.h"       /* Runde 41 Spur H: g_engine.frame_count (nur Messzeile RE15_HANGAR_LOG) */
#include "re15_schwanz_r41.h"  /* Runde 41 SF N1.9: Schwanz-Kette */
#include "re15_esp.h"          /* Neuaufbau ab NB2 (aus NB3/NB5): gruenes Blut (re15_esp_fx_spawn_ex, abr_port) */
#include <math.h>
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
#define BF_SM_SCHRITT     600        /* Gestaltung (N2): groesster Schritt des gehaltenen Spider-Man je Takt bis zum Wurf (h66),
                                       * Video-Ansprung h4..8 ~600 je Bild (N1.4) */
#define BF_BOGEN         1000        /* Gestaltung: Scheitel ueber der Geraden (Video h3..h8 Mitte ~25 Bildpunkte ueber h0) */
/* Zeitplan, h = Videobild (0 = erstes Bild in Cut 12). */
#define BF_H_SPRUNG_AB    2
#define BF_H_BERUEHRUNG  12
#define BF_H_WURF        72
#define BF_H_AUFSCHLAG   76
#define BF_H_AUFSPRINGEN 80          /* Video h80: Beine schon hoch, h82..86 rechts hinaus; Lauf n5 (Start h82) war 2 Bilder spaet */
#define BF_H_WEG         89          /* Neuaufbau ab NB2: Takt 89 = Bild 87, Video h87 weg (h86 noch 5266 Bildpunkte am Rand) */
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
    {   7,  0 }, {  17, 10 }, {  35, 28 }, {  51, 12 }, {  68, 27 }, { BF_H_WURF + 2, BF_WURF_BILD }, { BF_H_AUFSCHLAG + 2, 55 },
    { BF_H_GANG, 70 } };
/* Schritt 2 nach Referenz 2 (Loslassen): Video 4 h66-h70 haelt Birkin den Spider-Man noch oben links (Rumpf x ~100-170, y ~60-250),
 * h71 sinkt er links herab, h72 kopfueber links (x 60-150), h73/h74 unten links. Ref2 (84e8a4d97) schwang die Hand schon ab Bild 68
 * nach unten (Clip 0x0B B38 -> B44 Takt 70 -> 72) und liess bei Bild 70 los (3041 Einheiten von der Hand). Jetzt der Abschwung
 * 2 Takte spaeter (B27 bei Takt 68, Wurfbild B44 bei Takt 74, Aufschlag B55 bei Takt 78, Gier mit) und Loslassen im Takt
 * BF_H_LOS = 73 = Bild 71 mitten im Abschwung (B41) mit dem Tempo der Hand. GESTALTUNG nach dem Video. */
#define BF_H_LOS        (BF_H_WURF + 1)
/* Birkin-Gier relativ zum Blick auf den Spider-Man-Start (Q12, 4096 = 360 Grad). Gestaltung nach dem Video: Birkin steht
 * zur Kamera gedreht (die haltende rechte Klaue liegt dann links im Bild, Gier 1300 = Bezug - 1270,
 * Video Rumpf x 110..145 bis h70), dreht h66..72 nach links und schmettert in Richtung des Ansprungs (Klaue B50 bei Gier
 * 2570 Bild (148,188), Video h72 Rumpf ganz links (60..150), Aufschlag (115..150, 203..215)). */
static const int16_t k_birkin_gier[][2] = {
    {   0, -1270 }, {  68, -1270 }, {  74,     0 }, { BF_H_AUFSCHLAG + 2, 0 } };   /* Schritt 2: Abschwung 2 Takte spaeter */
#define BF_GANG_SCHRITT  22          /* Video h90..135: ~1000 Einheiten in 45 Bildern */
/* Runde 41 SF Neuaufbau ab NB2 (Nutzerwunsch "Schwanz: laenger, Laenge, Glieder und Baender wie im Video, Bewegung 1:1 nach
 * dem Video"): Schwanzbahn und Verdecken aus NB3-NB5 (r41/sichel 3f91e5c57) uebernommen. Die Tabelle ist je VIDEOBILD h; der
 * NB2-Takt h wird im Bild h - 2 gezeichnet (Lauf nb2neu/G2: Messzeile "h-1 F240", erstes Letterbox-Bild F243) -> die Szene
 * ruft bf_schwanz_ziel mit dem Bild des Takts (BF_BILD_VERSATZ). */
#define BF_BILD_VERSATZ (-2)
#define BF_N(t) ((int)(sizeof (t) / sizeof (t)[0]))
/* Runde 41 SF NB3 (Abnahme 1 Mangel 3): die Seitenvorgabe ist ersetzt. Der Schwanz faehrt jetzt die Bahn der Spitze aus
 * dem Video direkt nach (Tabelle 5d der Abnahme, Bildpunkte 640x480 je Videobild), FABRIK von der Wurzel
 * (re15_schwanz_r41_ziel). art 0 = Punkt auf dem Boden, 1 = Blicktiefe der Birkin-Wurzel + tiefe, 2 = verdeckt hinter
 * dem Koerper (Modellraum (-1500, -150, 200), Video h74-h100: Schwanz nicht sichtbar; vorher Haken ueber dem Ruecken).
 * Quellen: Abnahme 5d (h20-h34, h60-h70), SF N1.2 (h38-h48 Fleck x 402-430 y 289-326). */
typedef struct { int16_t h, vx, vy, art, tiefe; } bf_schwanzpunkt_t;
/* Runde 41 SF NB4 (Abnahme 2 HSF_abnahme_2.md A1-A4; PORT-WAHL nach Video 4, Bildpunkte 640x480, Ziel je VIDEOBILD h).
 * A3 Zeitversatz: der Szenentakt h wird im Spielbild h+1 gezeichnet (Dump F = h + 236, Videobild h = F - 235; die Messzeile
 * "hN Schwanzspitze" zeigt den Stand des Bilds F = N + 235). NB3 verglich Video h mit Messzeile h+1 und setzte die Tabelle so
 * 1 Bild zu spaet (Abnahme 2a: am gleichen Bild Mittel 18, max 56 Bildpunkte). Jetzt ist h der Tabelle das Videobild, der
 * Takt h setzt das Ziel fuer h+1 (bf_schwanz_ziel(b, h + 1)).
 * A4 Form: zusaetzlich ein Durchgangspunkt `via` fuer das Gelenk v.h (dort tritt der Schwanz im Video aus dem
 * Koerperumriss) und eine in die Blickrichtung gebogene Startform (re15_schwanz_r41_fuehrung): die ueberschuessige Laenge
 * liegt hinter dem Koerper, das Bild zeigt eine gerade Linie via -> Spitze. Videopunkte am Raster gelesen
 * (Lupe f 1,6, Blaetter nb4 v_schwanz_a/b): h18/h20/h22 Diagonale (316,262)/(318,263)/(325,275) -> (290,340)/(280,357)/
 * (238,408); h24 Knick (293,340) -> (205,352); h26 Knick (300,345) -> (222,290); h28 (262,340) -> (232,300); h30-h34 zwischen
 * den Knien; h58 kurzes Stueck (420-430, 315-345); h60-h70 gerade von (430,320-335) zum Boden (455,390) .. (530,410);
 * h72 Rest (390-400, 340-360); h74.. nichts.
 * A1/A2 Verdeckung: Video h36-h56 und h74-h102 kein Schwanz (Abnahme 2b: 0 kraeftig rote Bildpunkte rechts der Huefte) ->
 * Spitze und via auf dem Sehstrahl durch die Koerpermitte HINTER den Koerper (art 1, tiefe +1800 / +900).
 * art 0 = Boden, 1 = Blicktiefe der Birkin-Wurzel (Hoehe -1000) + tiefe, 2 = Modellraum hinter dem Koerper (nicht mehr benutzt). */
typedef struct { bf_schwanzpunkt_t s, v; } bf_schwanzbahn_t;   /* v.h = Gelenk des Durchgangspunkts (1..6) */
/* via-Gelenk je Eintrag nach der Erreichbarkeit (Rueckprojektion, Glied ~466 nach NB4-Verlaengerung, Lauf nb4/t1: Ansatz ->
 * Austritt 664-819 (h16-h22), 1477-1495 (h24/h26), 1960 (h28), ~1000 (h30-h34, h60-h70); Austritt -> Spitze bis 2556 (h22)). */
static const bf_schwanzbahn_t k_schwanz_bahn[] = {
    { { 16, 300, 345, 1,    0 }, { 2, 320, 272, 1, -150 } }, { { 18, 290, 340, 1,    0 }, { 2, 316, 262, 1, -150 } },
    { { 20, 280, 357, 1,    0 }, { 2, 318, 263, 1, -150 } }, { { 22, 238, 408, 0,    0 }, { 2, 312, 298, 1, -150 } },
    { { 24, 205, 352, 1,    0 }, { 3, 293, 340, 1, -150 } }, { { 26, 222, 290, 1,    0 }, { 3, 300, 345, 1, -150 } },
    { { 28, 232, 300, 1,    0 }, { 4, 262, 340, 1, -150 } }, { { 30, 318, 372, 1, -300 }, { 3, 330, 330, 1,    0 } },
    { { 32, 340, 383, 1, -300 }, { 3, 345, 335, 1,    0 } }, { { 34, 365, 375, 1, -200 }, { 3, 360, 330, 1,  200 } },
    { { 36, 395, 300, 1, 1800 }, { 3, 390, 290, 1,  900 } }, { { 56, 395, 300, 1, 1800 }, { 3, 390, 290, 1,  900 } },
    { { 58, 425, 340, 1,  700 }, { 3, 415, 300, 1,  900 } }, { { 60, 455, 390, 1,    0 }, { 4, 430, 320, 1,    0 } },
    { { 62, 470, 410, 1,    0 }, { 4, 430, 325, 1,    0 } }, { { 64, 505, 425, 0,    0 }, { 3, 430, 335, 1,    0 } },
    { { 66, 515, 408, 1,    0 }, { 3, 430, 335, 1,    0 } }, { { 68, 530, 410, 1,    0 }, { 3, 430, 330, 1,    0 } },
    { { 70, 522, 402, 1,    0 }, { 3, 430, 335, 1,    0 } }, { { 72, 395, 300, 1, 1800 }, { 3, 390, 290, 1,  900 } },
    { { 74, 395, 300, 1, 1800 }, { 3, 390, 290, 1,  900 } }, { { 102, 395, 300, 1, 1800 }, { 3, 390, 290, 1,  900 } } };
#define BF_SCHWANZ_N ((int)(sizeof k_schwanz_bahn / sizeof k_schwanz_bahn[0]))
/* NB5 (Abnahme 3 B1 "Schwanz h60-h72: oberes Stueck an der Huefte nicht sichtbar, duenner, h72 weg"): kraeftig rot im Kasten
 * x 395-435 y 270-335 h62-h72 Video 88/57/8/0/62/0, Port NB4 187/298/345/373/495/365; Schwanzkasten x 420-545 y 320-435 h64-h70
 * Video 356/544/631/523, Port 920/823/894/754 (1,4-2,6x); h72 Video leer. Jetzt: Durchgangspunkt h60-h70 500 hinter der Wurzel
 * und 10 Bildpunkte weiter zur Koerpermitte (420 statt 430: das Stueck Wurzel -> Austritt liegt hinter dem Koerperumriss), h72
 * verdeckt wie h74, Querschnitt h58-h72 k_schwanz_dicke (Q12). PORT-WAHL nach Video 4.
 * Laeufe nb5/T3 (Durchgangspunkt (420,330-338) +500) und T5 ((405,345-348) +600): oberes Stueck h62-h70 142-332 bzw. 80-309
 * (Video 0-88) - neben dem Port-Koerper liegt dort nichts, was es verdeckt. Jetzt: Durchgangspunkte wieder am Video-Austritt
 * (430,320-335, Tiefe 0 wie NB4) und die Glieder Wurzel -> Durchgangsgelenk h60-h70 ohne Querschnitt (re15_schwanz_r41_verdeckt):
 * sichtbar ist nur Austritt -> Spitze (Video ~120 Bildpunkte). */
static const int16_t k_schwanz_dicke[][2] = { { 56, 4096 }, { 58, 3195 }, { 72, 3195 }, { 74, 4096 } };   /* T3 mit 3400: h64 744
                                                                                                        * (Video 356), T4 mit 2870:
                                                                                                        * h62-h70 227-382 (317-631) */   /* T2 mit 2250: Kasten
                                                                                                        * h60-h70 56-238, Video 175-631 */
/* Gewicht des Szenenziels je Videobild (NB4: Videobild, nicht Takt; Freigabe nach dem Verdecken h102-h108). */
static const int16_t k_schwanz_gewicht[][2] = { { 15, 0 }, { 19, 4096 }, { 102, 4096 }, { 108, 0 } };


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
    int32_t  w0[3], wv[3], wvor[3]; /* Neuaufbau ab NB2: Loslassen (Lage, Handtempo je Takt), Lage im Takt davor */
} s_h = { 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
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

/* Neuaufbau ab NB2 (aus NB3): Effekte (gruenes Blut) der Szene nicht gegen den Platzhalter-Anker von Cut 12 cullen. */
int re15_bf_szene_effekte(void)
{
    return s_h.riegel && s_h.raum != 0 && (int)g_scd.work_vars[0x0A] == BF_HANGAR_CUT;
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
    re15_schwanz_r41_ziel(s_h.birkin, NULL, 0);          /* Neuaufbau ab NB2: Schwanzbahn und Verdecken aus */
    re15_schwanz_r41_dicke(s_h.birkin, 0);
    re15_schwanz_r41_verdeckt(s_h.birkin, 0);
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

/* Runde 41 SF NB3: Videopunkt (640x480; Registrierung der Abnahme 1: x_v = 2 x_psx + 1,97, y_v = 2,079 y_psx + 1,3) ->
 * Weltpunkt durch die Szenenkamera (Umkehrung von bf_punkt_im_bild). art 0 = Boden (y = -100, Schwanzradius), 1 = Blicktiefe
 * der Birkin-Wurzel (Hoehe -1000) + tiefe, 2 = hinter dem Koerper (Modellraum (-1500,-150,200)). */
static int bf_video_welt(const bf_schwanzpunkt_t *q, const re15_actor_t *b, int32_t out[3])
{
    if (q->art == 2) {
        const int32_t mdl[3] = { -1500, -150, 200 };
        re15_skel_bone_to_world(mdl, b->rot_y, b->x, b->y, b->z, out);
        return 1;
    }
    const re15_camera_cut_t *c = bf_szenenkamera();
    re15_camera_view_t v;
    if (!c || re15_camera_build_view(c, &v) != 0 || v.fov_screen_dist <= 0) return 0;
    double R[9], cp[3], dw[3];
    for (int i = 0; i < 9; i++) R[i] = (double)v.rot[i] / 4096.0;
    const double H = (double)v.fov_screen_dist;
    const double d[3] = { (((double)q->vx - 1.97) / 2.0 - 160.0) / H, (((double)q->vy - 1.3) / 2.079 - 120.0) / H, 1.0 };
    for (int k = 0; k < 3; k++) {
        cp[k] = -(R[k] * v.trans[0] + R[3 + k] * v.trans[1] + R[6 + k] * v.trans[2]);
        dw[k] = R[k] * d[0] + R[3 + k] * d[1] + R[6 + k] * d[2];
    }
    double z;
    if (q->art == 0) {
        if (dw[1] <= 1e-6) return 0;
        z = (-100.0 - cp[1]) / dw[1];
    } else if (q->art == 3) {                            /* Neuaufbau ab NB2: Punkt auf dem Sehstrahl in Blicktiefe q->tiefe */
        z = (double)q->tiefe;
    } else {
        z = R[6] * b->x + R[7] * -1000.0 + R[8] * b->z + v.trans[2] + q->tiefe;
    }
    for (int k = 0; k < 3; k++) out[k] = (int32_t)(cp[k] + dw[k] * z);
    return 1;
}


/* NB4: Weltpunkt p um (dx, dy) Bildpunkte (640x480) in der Szenenkamera verschieben (in seiner Blicktiefe). */
static void bf_bild_versatz(int32_t p[3], int dx, int dy)
{
    const re15_camera_cut_t *c = bf_szenenkamera();
    re15_camera_view_t v;
    if (!c || re15_camera_build_view(c, &v) != 0 || v.fov_screen_dist <= 0 || (dx == 0 && dy == 0)) return;
    const double vz = ((double)v.rot[6] * p[0] + (double)v.rot[7] * p[1] + (double)v.rot[8] * p[2]) / 4096.0 + v.trans[2];
    if (vz <= 1.0) return;
    const double sx = dx / 2.0 * vz / v.fov_screen_dist, sy = dy / 2.079 * vz / v.fov_screen_dist;
    for (int k = 0; k < 3; k++) p[k] += (int32_t)((v.rot[k] * sx + v.rot[3 + k] * sy) / 4096.0);
}


/* Schwanzziel je Videobild: zwischen den Tabellenpunkten im Weltraum linear (Spitze und Durchgangspunkt), Startform in die
 * Blickrichtung der Szenenkamera gebogen (NB4, Kopf k_schwanz_bahn). */
static void bf_schwanz_ziel(const re15_actor_t *b, int h)
{
    const int32_t w = bf_tab(k_schwanz_gewicht, BF_N(k_schwanz_gewicht), h);
    int32_t a[3], e[3], z[3], va[3], ve[3], vz[3], bg[3];
    int i = 0;
    if (w <= 0) { re15_schwanz_r41_ziel(s_h.birkin, NULL, 0); re15_schwanz_r41_fuehrung(s_h.birkin, NULL, 0, NULL); return; }
    while (i < BF_SCHWANZ_N - 1 && h >= k_schwanz_bahn[i + 1].s.h) i++;
    const bf_schwanzbahn_t *p0 = &k_schwanz_bahn[i], *p1 = &k_schwanz_bahn[i < BF_SCHWANZ_N - 1 ? i + 1 : i];
    if (!bf_video_welt(&p0->s, b, a) || !bf_video_welt(&p1->s, b, e) || !bf_video_welt(&p0->v, b, va) ||
        !bf_video_welt(&p1->v, b, ve)) { re15_schwanz_r41_ziel(s_h.birkin, NULL, 0); return; }
    const int span = p1->s.h - p0->s.h;
    const int u = (span > 0 && h > p0->s.h) ? ((h < p1->s.h ? h - p0->s.h : span) * 4096 / span) : 0;
    for (int k = 0; k < 3; k++) { z[k] = bf_mix(a[k], e[k], u); vz[k] = bf_mix(va[k], ve[k], u); }
    bg[0] = bg[1] = bg[2] = 0;
    {   /* Beugerichtung = Sehstrahl der Kamera zur Mitte des sichtbaren Stuecks via -> Spitze (dort liegt der Ueberschuss genau
         * in der Tiefe, das Bild zeigt eine Gerade) */
        const re15_camera_cut_t *c = bf_szenenkamera();
        if (c) {
            const double fx = (vz[0] + z[0]) * 0.5 - c->pos_x, fy = (vz[1] + z[1]) * 0.5 - c->pos_y, fz = (vz[2] + z[2]) * 0.5 - c->pos_z;
            const double l = sqrt(fx * fx + fy * fy + fz * fz);
            if (l > 1.0) { bg[0] = (int32_t)(fx * 4096.0 / l); bg[1] = (int32_t)(fy * 4096.0 / l); bg[2] = (int32_t)(fz * 4096.0 / l); }
        }
    }
    re15_schwanz_r41_fuehrung(s_h.birkin, vz, p0->v.h, bg);
    re15_schwanz_r41_ziel(s_h.birkin, z, w);
    re15_schwanz_r41_verdeckt(s_h.birkin, (h >= 60 && h <= 70) ? p0->v.h : 0);   /* NB5 B1: Huefte -> Austritt verdeckt */
}

/* Gruenes Blut (Abnahme 1 Mangel 5, Video 4): Wolke am Griff h18-h71 (Maximum h53, 750 Bildpunkte), schmale Fontaene
 * h56-h68 ueber der Hand, Spritzer am Boden bei der Flucht h84-h89, keine Lache. Effekt = RE1.5-Spinnenblut 0x00082000
 * (STAGE2 `lui a0,0x8` @0x80113b10, `ori a0,a0,0x2000` @0x80113b30, `jal 0x80019700` @0x80113b50; Sub 8 -> CLUT-Zeile
 * 486 gruen, Spur S Nachbesserung 1). Zeitplan PORT-WAHL nach dem Video. */
#define BF_BLUT_GRUEN 0x00082000u
/* Skala Q12 der Wolke (NB4 0x2400, s. bf_gruenes_blut; Lauf nb4/N4L mit 0x3000: h28-h48 1202-1324 Bildpunkte, Video 149-468).
 * NB5 (ABR3): Lauf nb5/T2 mit 0x2400 jedes 4. Bild h28-h48 ~0 gruene Bildpunkte -> 0x4000. */
#define BF_BLUT_WOLKE 0x3400                 /* Schritt 3 nach Referenz 2: Saeule/Spritzer jetzt auf Video-Staerke -> Wolke
                                              * kleiner, Summe h18-h92 nahe am Video (Lauf R13 mit 0x4000: 20144, Video 12417) */
#define BF_BLUT_BODEN 0x3000                 /* Skala Q12 der Bodenspritzer (NB4: halbe Menge) */
/* Schritt 3 nach Referenz 2: Bodenspritzer h84/h86 Ref2 21/363 gruene Bildpunkte, Video 779/1151 -> groessere Skala; Saeule
 * Skala (Ref2 0x1000). GESTALTUNG nach dem Video. */
#define BF_BLUT_BODEN2  0x4000                /* Lauf R11 mit 0x5000: h84/h86 897/2630 */
#define BF_BLUT_SAEULE  0x1800
/* NB5 (Abnahme 3 B4 "matter Nebel statt gesaettigter scharfer Quadrate"; gruen-Maske Farbe Median Video (33-42,48-58,21-37), Port
 * NB4 (29-32,50-56,14-16) bei ABR0 50/50): Mischart B + F/4 (ABR3, abr_port 4) - der Hintergrund bleibt, der gruene Anteil liegt als
 * schwacher Schleier darueber (Rechnung: Decke ~(35,45,30) + Sprite/4 ~(6,15,0) = (41,60,30)). PORT-WAHL nach dem Video. */
#define BF_BLUT_MISCH 4
#define BF_H_BLUT_ENDE   64                  /* letzter Wolken-Spawn: Lauf nb3/r3 mit Ende h71 hatte bei h72 noch 665 gruene
                                              * Bildpunkte (Effekt steht ~8 Bilder), Video h72 0 */

/* Gruenes Blut des Spider-Man (Kopf "Gruenes Blut").
 * Runde 41 SF NB4 (Abnahme 2 B6: "Wolke UEBER dem Spider-Man (nicht darunter), Nebel statt Quadrate, Fontaene als Saeule,
 * Bodenspritzer ~100 px weiter rechts, halbe Menge, weg bei h89"). Video 4 (gruene Bildpunkte g > r+14, g > b+6, g > 45,
 * Lage 5-95 %): h28 x 222-324 y 59-98, h40 243-339 / 61-84, h48 241-331 / 63-118, h53-h58 328-359 / 103-142, h66-h70
 * 312-349 / 117-137 = am Griff (Sichelhand) und darueber, rechts vom Spider-Man; Port (Lauf nb4/t7, Spawn am Spider-Man-Rumpf)
 * x 111-308 y 51-148. Jetzt: Wolke an der Sichelhand (Knochen 15) + Bildversatz (+20, -40), groessere Sprites (Skala 0x4800,
 * jedes 2. Bild - ABR0 50/50 des Spinnenbluts wie bisher), Saeule h52-h68 ueber der Hand. Boden: Video h84 x 311-380 y 371-422
 * (779 Bildpunkte), h86 405-562 (1151), h88 457-476 (170), h90 0; Port t7 h84 296-428 (1484), h86 288-444 (974), h88 315-486
 * (530), h90 27 -> Spawn unter dem fliehenden Spider-Man selbst (h81-h83), kleinere Skala, und h88 alle Effekte der Szene
 * weg (re15_esp_fx_reset; ein Blut-Effekt steht sonst ~10 Bilder, NB3). PORT-WAHL nach dem Video. */
static void bf_gruenes_blut(re15_actor_t *b, re15_actor_t *m, int h)
{
    const re15_esp_t *bank = re15_esp_room_bank();
    if (b && m && m->active && h >= 24 && h <= BF_H_BLUT_ENDE) {
        int32_t p[3];
        re15_enemy_bone_world_pos(b, BF_KLAUE, p);
        if (h % 3 == 0) {                                /* NB5: Lauf nb5/T3 jedes 2. Bild (0x4000, ABR3): Summe h18-h92 47882 gruene
                                                          * Bildpunkte, T4 jedes 4.: 5272, Video 13145 -> jedes 3.; Lauf nb4/t8 (jedes 2. Bild, 0x4800, Versatz (20,-40)): 1259-3131 Bildpunkte
                                                          * bei x 186-309 y 78-190 - Video 136-753 bei x 222-359 y 59-142 */
            int32_t q[3] = { p[0], p[1], p[2] };
            re15_esp_fx_t *fx;
            bf_bild_versatz(q, 60, -85);
            fx = re15_esp_fx_spawn_ex(bank, 0, 8, BF_BLUT_WOLKE, q[0], q[1], q[2], m->rot_y);
            if (fx) fx->abr_port = BF_BLUT_MISCH;
        }
        if (h >= 50 && h <= 60 && (h & 1 || h <= 54)) {                  /* schmale Saeule ueber der Hand (Video h53-h58; Lauf R12 bis h68: Summe 23724) */
            /* Schritt 3 nach Referenz 2: Video h53/h56/h58 im Kasten x 320-365 y 95-150 753/558/453 gruene Bildpunkte, Ref2 12/37/31
             * bei x 271-344 (Hand im NB2-Halten bei x ~252-264). Jetzt 85 statt 60 Bildpunkte rechts der Hand, ab h50, h50-h54 jedes
             * Bild, Skala BF_BLUT_SAEULE statt 0x1000 (Lauf R11 mit h52-h58 jedem Bild und 0x2800: h56/h58 1395/1822, Summe 45043).
             * GESTALTUNG nach dem Video. */
            int32_t q[3] = { p[0], p[1], p[2] };
            re15_esp_fx_t *fx;
            bf_bild_versatz(q, 85, -30 - 6 * (h - 52));
            fx = re15_esp_fx_spawn_ex(bank, 0, 8, BF_BLUT_SAEULE, q[0], q[1], q[2], m->rot_y);
            if (fx) fx->abr_port = BF_BLUT_MISCH;
        }
    }
    if (b && (h == 82 || h == 84)) {                     /* Schritt 3 (Ref2: Bild 83/85, h84 21 Bildpunkte, Video 779): ein Bild frueher */
        static const bf_schwanzpunkt_t sp1 = { 0, 345, 405, 0, 0 }, sp2 = { 0, 480, 415, 0, 0 };   /* Video h84 / h86 */
        int32_t q[3];
        if (bf_video_welt(h == 82 ? &sp1 : &sp2, b, q)) {
            /* Neuaufbau ab NB2: Bodenspritzer im Original-Mischweg (ABR0 50/50) - mit dem NB5-Schleier ABR3 waren sie unsichtbar
             * (Abnahme 4: h84/h86/h88 0 Bildpunkte, Video 779/1151/170); die Wolke bleibt der matte Schleier (Nutzerwunsch c). */
            (void)re15_esp_fx_spawn_ex(bank, 0, 8, h == 82 ? BF_BLUT_BODEN2 : BF_BLUT_BODEN, q[0], -80, q[2], 0);   /* R12 h86 1706 (Video 1151) */
        }
    }
    if (h == 88) re15_esp_fx_reset();                    /* Video h89: kein Spritzer mehr */
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
/* Runde 41 SF Neuaufbau ab NB2, Nutzer 2026-11-03: "Und die Spiderman springt auch nicht oder sowas. Er wirft sie auf den Boden in
 * Richtung Kamera wo sie fliegt." Der Spider-Man bewegt sich nach dem Loslassen NICHT selbst: er verlaesst die Sichelhand im
 * Wurfbild (Clip 0x0B B44 = BF_WURF_BILD, Takt BF_H_WURF) mit dem Tempo der Hand und fliegt passiv (Rueckenlage-Clip 0x0B, Glieder
 * schlaff) zu Boden und auf die Kamera zu. Bahn je VIDEOBILD hb nach der Messung Video 4 (tools/r41_sichel/nb2neu/flucht6.py,
 * Maske ohne Birkin, Schwarzwert heraus): h70-h73 faellt er links herab (Schwerpunkt (320,256) -> (232,261)), h74-h78 liegt er
 * links unten im Vordergrund (Schwerpunkt x 240-267, Oberkante 221-235, Unterkante am Bildrand), h79-h86 rutscht er gross
 * nach rechts vorn an der Kamera vorbei (Schwerpunkt x 311/385/345/406/495/544/584/606, Flaeche h82/h83 13011/24471), h87 weg.
 * Spalten: hb, x, y (Video 640x480), Blicktiefe (0 = Bodenpunkt unter (x,y)). Vom Loslassen bis zum ersten Punkt eine
 * quadratische Bahn, deren Anfangsrichtung das Handtempo ist. GESTALTUNG nach dem Video. */
/* Lauf nb2neu/R1 mit Bodenpunkten in Birkins Naehe (Tiefe ~5900): Flaeche h74-h81 4243-4990 (Video 5164-11883), h83 8685 (Video
 * 24471) - er bleibt zu weit weg. Das hochskalierte Bild des Nutzers zu h74 (Screenshot 100003, NCC 0,982) zeigt ihn kopfueber gross
 * VOR Birkin im Flug, h85 (Screenshot 100432) ein Glied dicht vor der Kamera rechts -> Bahnpunkte auf dem Sehstrahl in Blicktiefe
 * 4200 -> 2100 (Hoehe aus dem Strahl, nie unter dem Boden). */
/* Lauf R3 (Tiefe 4200 -> 2100): h72-h76 und h82-h85 zu gross und zu mittig (h83/h84 fuellt er die Bildmitte, Video: Glied am
 * rechten Rand) -> weiter weg (4800 -> 2800) und ab h83 schneller nach rechts hinaus. */
/* Lauf R4 (h74-h80 auf dem Sehstrahl in Tiefe 4800-4400): Lage im Bild wie das Video, aber er beruehrt den Boden nie - der Nutzer:
 * "Er wirft sie auf den Boden in Richtung Kamera". Video h79/h80 liegt er (Oberkante 266/288). Jetzt h78/h80 Bodenpunkte in
 * Blicktiefe ~5200 (Bild-y ~500, unter dem Bildrand: man sieht die hochstehenden Glieder), danach wieder auf dem Sehstrahl. */
/* Schritt 1 nach Referenz 2 (Koordinator 2026-11-03: "Im Video fliegt der Spider-Man h81-h85 ganz dicht an der Kamera vorbei, das
 * grosse orange Glied fuellt die rechte Bildhaelfte. Bei Ref2 bleibt er klein im Hintergrund"): Ref2 hatte ab h82 Blicktiefe
 * 4300 -> 2800. Video (flucht6.py, Schwarzwert heraus) h82/h83/h84/h85 Oberkante 196/196/111/120, Flaeche 13011/24471/14860/20708,
 * Schwerpunkt x 406/495/544/584; hochskaliertes Nutzerbild h85 (Screenshot 100432): Glied von y ~150 bis zum unteren Rand am
 * rechten Rand. Lauf R7 (Tiefe 3800 -> 1300, x 380-820): h81-h85 fuellte er das GANZE Bild -> Tiefe 4600 -> 2200 und weiter
 * rechts (x 370-940), so dass vor allem Glieder in der rechten Bildhaelfte stehen; Lauf R8 damit h84/h85 noch kleiner als das
 * Video-Glied -> h84-h87 Tiefe 2500 -> 1900. Weltweg Richtung Kameraposition Cut 12. */
static const int16_t k_wurf_bahn[][4] = { { 74, 230, 340, 4800 }, { 76, 250, 390, 4600 }, { 78, 270, 505, 0 }, { 80, 330, 500, 0 },
                                          { 81, 370, 450, 4600 }, { 82, 450, 380, 3800 }, { 83, 560, 340, 3200 }, { 84, 640, 290, 2500 },
                                          { 85, 720, 290, 2200 }, { 86, 820, 320, 2000 }, { 87, 940, 330, 1900 } };
static int bf_wurf_punkt(const re15_actor_t *b, int i, int32_t out[3])
{
    bf_schwanzpunkt_t q = { 0, k_wurf_bahn[i][1], k_wurf_bahn[i][2], k_wurf_bahn[i][3] > 0 ? 3 : 0, k_wurf_bahn[i][3] };
    if (!bf_video_welt(&q, b, out)) return 0;
    if (q.art == 0 || out[1] > 0) out[1] = 0;
    return 1;
}

static void bf_hangar_takt(re15_actor_t *b, re15_actor_t *m)
{
    const int h = s_h.t - BF_VORLAUF;                    /* Videobild */
    int32_t k[3];
    s_h.t++;
    {   /* Neuaufbau ab NB2: Schwanzbahn je Videobild (NB3-NB5), der Takt h wird im Bild h + BF_BILD_VERSATZ gezeichnet */
        const int hb = h + BF_BILD_VERSATZ;
        bf_schwanz_ziel(b, hb);
        re15_schwanz_r41_dicke(s_h.birkin, bf_tab(k_schwanz_dicke, BF_N(k_schwanz_dicke), hb));
        bf_gruenes_blut(b, m, hb);                       /* Neuaufbau ab NB2: gruenes Blut als matter Schleier (NB5) */
    }
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
    } else if (h < BF_H_LOS) {                                            /* gehalten bis zum Loslassen (NB2-Kopplung) */
        if (h == BF_H_BERUEHRUNG) bf_clip(m, BF_SM_GRIFF);
        else re15_r38_birkin_anim(m);                                     /* Zappeln: Clip 7 laeuft im Kreis */
        s_h.wvor[0] = m->x; s_h.wvor[1] = m->y; s_h.wvor[2] = m->z;       /* Neuaufbau: Lage vor dem Takt (Handtempo) */
        m->rot_y = bf_blick(m, b->x, b->z);
        bf_klauenziel(b, m, k);
        {   /* folgt der Hand geglaettet (halber Abstand je Takt). Runde 41 SF N2: bis zum Wurf hoechstens BF_SM_SCHRITT je
             * Takt - mit der geduckten Haltung wandert die Sichelhand beim Zugreifen (Clip 0x0B B3->B5) 860 statt 750 und der
             * Griffknochen wechselt mit Clip 7 die Lage; ohne Grenze sprang der Spider-Man bei h13 803 Einheiten zurueck. */
            int32_t d[3] = { (int32_t)(((int64_t)(k[0] - m->x) * 2048) >> 12), (int32_t)(((int64_t)(k[1] - m->y) * 2048) >> 12),
                             (int32_t)(((int64_t)(k[2] - m->z) * 2048) >> 12) };
            const int64_t q = (int64_t)d[0] * d[0] + (int64_t)d[1] * d[1] + (int64_t)d[2] * d[2];
            if (h < k_birkin_bild[4][0] && q > (int64_t)BF_SM_SCHRITT * BF_SM_SCHRITT) {
                const int32_t l = (int32_t)re15_squareroot0((uint32_t)(q > 0xFFFFFFFFll ? 0xFFFFFFFFll : q));
                for (int i = 0; i < 3; i++) d[i] = (int32_t)((int64_t)d[i] * BF_SM_SCHRITT / (l > 0 ? l : 1));
            }
            m->x += d[0]; m->y += d[1]; m->z += d[2];
            /* Runde 41 SF Neuaufbau ab NB2 (Nutzer 2026-11-03: Fangen und Wurf "vollkommen asynchron"): ab dem 4. Haltetakt
             * (Video h16) haengt der Griffknochen JEDEN Takt genau auf der Sichelhand - keine Glaettung, keine eigene Spur. NB2
             * glaettete auch im Wurf (halber Abstand je Takt) und lief der schnellen Hand h66-h71 bis 1419 Einheiten nach
             * (unit_birkin_szenen); vorher (h12-h15) zieht der Griff ihn geglaettet heran (Clipwechsel Birkins). */
            if (h >= BF_H_BERUEHRUNG + 4) { m->x = k[0]; m->y = k[1]; m->z = k[2]; }
        }
        if (m->y > 0) m->y = 0;                                           /* Hand tief im Wurf: nicht unter den Boden */
    } else if (h < BF_H_WEG) {                                            /* Neuaufbau ab NB2: passiv geworfen (Kopf k_wurf_bahn) */
        const int hb = h + BF_BILD_VERSATZ;
        int32_t p[3], z[3];
        if (h == BF_H_LOS) {                                              /* Loslassen im Abschwung mit dem Tempo der Hand */
            s_h.w0[0] = m->x; s_h.w0[1] = m->y; s_h.w0[2] = m->z;
            for (int k_ = 0; k_ < 3; k_++) s_h.wv[k_] = s_h.w0[k_] - s_h.wvor[k_];
            bf_clip(m, BF_SM_LIEGT);
        }
        bf_bild(m, BF_SM_LIEGT, (h - BF_H_LOS) * 2 > 30 ? 30 : (h - BF_H_LOS) * 2);   /* Rueckenlage, Glieder schlaff */
        if (hb < k_wurf_bahn[0][0]) {                                     /* Flug vom Loslassen zum Boden: quadratisch */
            const int h0 = BF_H_LOS + BF_BILD_VERSATZ, n_ = k_wurf_bahn[0][0] - h0;
            if (bf_wurf_punkt(b, 0, z)) {
                const int32_t u = (hb + 1 - h0) * 4096 / n_;             /* Takt h wird im Bild hb gezeichnet: Stand fuer hb+1 */
                for (int k_ = 0; k_ < 3; k_++) {
                    const int32_t c_ = s_h.w0[k_] + s_h.wv[k_] * n_ / 2;  /* Kontrollpunkt: Anfangsrichtung = Handtempo */
                    const int32_t a_ = bf_mix(s_h.w0[k_], c_, u), e_ = bf_mix(c_, z[k_], u);
                    p[k_] = bf_mix(a_, e_, u);
                }
                if (p[1] > 0) p[1] = 0;
                if (p[0] != m->x || p[2] != m->z) m->rot_y = bf_blick(m, p[0], p[2]);
                m->x = p[0]; m->y = p[1]; m->z = p[2];
            }
        } else {                                                          /* am Boden und an der Kamera vorbei */
            int i = 0;
            const int hz = hb + 1;
            while (i < BF_N(k_wurf_bahn) - 2 && hz >= k_wurf_bahn[i + 1][0]) i++;
            if (bf_wurf_punkt(b, i, p) && bf_wurf_punkt(b, i + 1, z)) {
                const int span = k_wurf_bahn[i + 1][0] - k_wurf_bahn[i][0];
                const int32_t u = hz <= k_wurf_bahn[i][0] ? 0 : hz >= k_wurf_bahn[i + 1][0] ? 4096 : (hz - k_wurf_bahn[i][0]) * 4096 / span;
                const int32_t x = bf_mix(p[0], z[0], u), y = bf_mix(p[1], z[1], u), zz = bf_mix(p[2], z[2], u);
                if (z[0] != x || z[2] != zz) m->rot_y = bf_blick(m, z[0], z[2]);
                m->x = x; m->y = y > 0 ? 0 : y; m->z = zz;
            }
        }
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
