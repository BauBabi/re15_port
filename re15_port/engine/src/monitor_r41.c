/*
 * monitor_r41.c — Runde 41 Spur "Monitor": Hangar-Szene als Ueberwachungsbild im Monitor Room (re15_monitor_r41.h).
 *
 * ⛔ PORT-SZENE. Original-Bausteine (selbst disassembliert, PSX.EXE = info/Re1.5/PSX.EXE, Datei-Offset = Adresse - 0x8000F800):
 *
 * 1. MONITOR-BILD-EFFEKT (Zone 1 Bit 0x1E). Je Bild ruft die Hauptschleife @0x8001d1d8 `jal 0x80020610`:
 *      80020614 lw   v1,DAT_800aca3c        Zone-1-Wort (Bit 0x1E = Maske 0x80000000 >> 30 = 2)
 *      8002061c andi v0,v1,0x8 / 80020620 beq  -> Bit 0x1C: FUN_80020674(1) (Wackeln, Tabelle @0x80072e70)
 *      80020638 andi v0,v1,0x4 / 8002063c beq  -> Bit 0x1D: FUN_80020674(2) (Tabelle @0x80072e90)
 *      80020640 andi v0,v1,0x2 / 80020654 beq  -> Bit 0x1E: 8002065c jal FUN_80020794 (a0 = 0, a1 = 0)
 *    FUN_80020794(0,0):
 *      800207cc lbu  v1,DAT_800b5576 ; 800207d4 lbu a2,DAT_80072eb8 (= 2) ; 800207e0 sb v1+a2 -> Phase += 2 je Bild
 *      800207e8 jal  FUN_800209ec(0,7)       Kette in OT 0, Eimer 7 (0x800aa698 + 7*4; Letterbox liegt in Eimer 4 @0x800aa6a8,
 *                                            FUN_80021a0c - der Effekt trifft also Hintergrund + Figuren, nicht Balken/Text)
 *      Schleife s0 = 0..0xef (s4 = 0xf0 @0x800207ec):
 *      800207f4 lw   v0,DAT_80072eb4 (= 2) ; 800207fc addu / 80020800 andi 0xff   s1 = (s1 + 2) & 0xff
 *      80020810 lb   a2,DAT_80072d70(s1)     Sinus-Tabelle (256 x int8, 3 .. 127 .. -127 .. 0)
 *      80020818 lw   v0,DAT_80072eb0 (= 0x40) ; 80020820 div a2,v0 / 80020848 mflo   Versatz = Sinus / 64 (gegen 0 gerundet)
 *      80020858 jal  FUN_80020894(s0, -a0 = 0, Versatz + a1 = Versatz)
 *    FUN_80020894(zeile, dx = 0, dy): 80020898 or / 8002089c beq -> dx == 0 && dy == 0: keine Kopie;
 *      Kopier-Primitiv (DR_MOVE, 0x18 Byte) Quelle x = -dx, Breite 0x140 + dx, Ziel-Zeile = zeile, Quell-Zeile = zeile + dy,
 *      unter 0 -> Zeile 0, ab 0xf0 -> 0xef; dy < 0 -> an den KOPF der Eimer-Kette (zuletzt eingefuegt = zuerst kopiert),
 *      sonst ans ENDE (in Zeilenfolge) - FUN_80020894 Ende (`addPrim`-Zweige, Decompile RE_15_Quellcode_V2/FUN_80020894.c).
 *    Genau EIN Skript setzt das Bit: ROOM5031 sub03 @0x011A8 `22 01 1e 01` (vor Cut 11, dem Sherry-Monitorbild) und
 *    @0x011E6 `22 01 1e 00` (danach) - Zensus aller RDTs (analysis/story_leon_elza/census.json, Set(1,0x1E)). Der Port
 *    zeichnet den Effekt in render_pc.c (vor Blenden/Letterbox/Text) ueber re15_monitor_r41_zeilen. Die Zweige 0x1C/0x1D
 *    (FUN_80020674, Wackeln in 1080/2040/3080/4020/5090/6030) sind NICHT Teil dieser Spur und bleiben wie bisher ungebaut.
 *
 * 2. KAMERAFUEHRUNG nach ROOM5031 sub03 (Elzas Sherry-Monitorbild, scd_annot/ROOM5031.txt):
 *      @0x01142 Set(2,7)=1 (Pad-Sperre)      @0x01194 Cut_chg 7 (Nahaufnahme eines Monitors)   @0x01196 Sleep 0x14 = 20
 *      @0x011A6 Cut_chg 11 + @0x011A8 Set(1,0x1E)=1 (Monitorbild)  ...  @0x011E6 Set(1,0x1E)=0
 *      @0x011EA Cut_chg 9 (Monitorwand)      @0x011F8 Sleep 0x32 = 50
 *      @0x01208 Cut_chg 1 + @0x0120A Cut_auto 1, @0x0120C Set(2,7)=0.
 *    PORT-WAHL: die Szene setzt in 5030/5031 NUR die Pad-Sperre (2,7), nicht die Letterbox (1,0x1B) des Originals
 *    (@0x01146): die abgenommene Hangar-Szene bringt ihre eigene Letterbox mit und blendet sie ab ihrem Start ein
 *    (bf_riegel); stuende (1,0x1B) schon, laege der Balken-Zaehler g_letterbox_level beim Szenenstart auf 0xF0 statt 0 und
 *    die ersten Szenenbilder waeren nicht mehr bitgleich zum abgenommenen Stand 23f9da5c9. Statt "Cut_chg 1" (Elzas Platz
 *    vor den Monitoren) geht die Kamera auf den Schnitt zurueck, den der Spieler beim Ausloesen hatte (er wird nicht bewegt).
 *
 * 3. MONITORBILD = die Hangar-Szene selbst (birkin_formen.c), unveraendert in ROOM50A0/50A1 Cut 12. PORT-WAHL: das Bild
 *    entsteht durch einen Raumwechsel ohne Spieler (re15_room_request_change wie eine Tuer, Schwarz + Einblenden
 *    room_common.c re15_room_transition_present) in den Hangar mit Eintritts-Schnitt 12; der Spieler steht dort an der
 *    Eintrittsstelle aus 5030 (Door_aot_set ROOM5030 slot 3 @0x00EC6: (-12117,0,4472), Gier 1024) - ausserhalb des
 *    Platzhalter-Ankers von Cut 12 (RVD rvd 39 @0x564, x/z -25000..-21000) und damit gecullt wie im abgenommenen Stand,
 *    ausserhalb der beiden Affen-Zonen des Raums (sub00 Aot_set @0x0165C x -14000..-10700 z -800..1100, @0x0172E
 *    x -20900..-18950 z -27100..-22850). Cut 12 hat keine Umschalt-Zone (nur den Anker) - der Zonen-Scan verlaesst ihn
 *    nie. Alle anderen Figuren des Raums (Affen 0x29 aus sub00) werden fuer die Dauer des Bilds abgeschaltet (keine
 *    Kill-Flags: beim naechsten echten Betreten setzt sub00 sie wie immer). Nach dem Szenenende (h135) zurueck in den
 *    Monitor Room an die gespeicherte Stelle, Eintritts-Schnitt 9 (Monitorwand), dann Schnitt des Spielers.
 *    Warum nicht im Monitor Room selbst wie das Sherry-Bild (Cut 11 zeigt dort einen entfernten Teil DESSELBEN Raums)?
 *    Der Hangar hat eigenen Hintergrund (BSS ROOM50A.BSS Cut 12), eigene Masken, Licht und Kollision; nur der Raumwechsel
 *    zeigt die abgenommene Szene bitgleich.
 *
 * 4. AUSLOESER (PORT-WAHL): erstes freies Bild im Monitor Room (Spieler Zustand 1 = Pad-Zweig @0x80073f90[1], nicht
 *    gegriffen, keine Szene, kein Pause-Wort, keine Blende, kein Raumwechsel offen), Bank-9-Bit RE15_MON41_BIT noch 0 und
 *    (3,0x4C) == 0. (3,0x4C) ist die Story-Grenze, ab der Monitor Room und Hangar nicht mehr erreichbar sind:
 *    ROOM5000 Door slot 0 @0x007FE -> 5030 nur bei Ck(3,0x4C)==0 (sonst @0x00822 -> 5110), ROOM4070 Door slot 1
 *    @0x011FE -> 50A0 nur bei Ck(3,0x4C)==0. Das Bit wird beim Wechsel in den Hangar gesetzt (Rueckkehr = erneuter
 *    Raumeintritt loest nicht wieder aus) und wird mit g_game.flags gespeichert.
 *
 * 5. RUNDE 2 (Pruefer SICHT, R1 "Figuren ploppen auf", R2 "vom Monitor-Look ist nichts zu sehen"):
 *    a) HARTER SCHNITT statt Schwarz + Einblende. Im Original schneidet ROOM5031 sub03 hart: Cut_chg 7 @0x01194 -> Cut_chg 11
 *       @0x011A6 (Monitorbild) -> Cut_chg 9 @0x011EA, ohne Blende dazwischen. Der Port-Raumwechsel des Monitorbilds laeuft deshalb
 *       ohne die Tuer-Einblende (room_common.c re15_room_transition_present: kein FUN_800217b0/FUN_800216ec-Kick, kein
 *       State-4/5-Freeze); alles andere des State-3-Rumpfs bleibt (re15_monitor_r41_harter_schnitt).
 *    b) KEIN LEERER HANGAR. Die Szene setzt ihre Figuren erst im zweiten KI-Takt nach dem Laden (birkin_formen.c
 *       re15_bf_raumtick: erster Takt Raum-Reset, zweiter bf_hangar_starten). Bis dahin zeigt der Zeichner das gemerkte Bild
 *       von Cut 7 (LOOK_STANDBILD), dann sofort das erste Szenenbild mit Birkin und Spider-Man - wie im abgenommenen Stand,
 *       wo Cut 12 im selben Takt wie die Figuren kam.
 *    c) LETZTES SZENENBILD h135: der Rueckweg wird erst im Takt NACH bf_hangar_beenden angefordert (Phase ENDE), sonst
 *       zeigte der Ladetakt des Monitor Room schon statt h135 (Runde 1: h135 fehlte).
 *    d) LETTERBOX wie das Original: ROOM5031 sub03 setzt (1,0x1B) @0x01146 VOR Cut 7 und loescht sie erst @0x01210 nach
 *       Cut_chg 1 / Cut_auto @0x01208/@0x0120A - die Balken stehen also ueber Cut 7, dem Monitorbild und Cut 9 und fahren erst
 *       beim Spieler zurueck. Der Raumlader loescht Zone 1 untere 16 Bit (@0x80039710-30, 0x1B gehoert dazu) - deshalb nach
 *       jedem der beiden Wechsel neu gesetzt (re15_monitor_r41_raum_geladen), ebenso die Pad-Sperre (2,7) (Runde 1: zwischen
 *       Laden und erstem Monitor-Takt offen). Am Szenenende loescht bf_riegel (1,27)/(2,7) - re15_monitor_r41_szene_ende setzt
 *       sie im selben Takt wieder, sonst sackte der Balken-Zaehler (FUN_80021a0c, -0x10 je Bild) kurz ab.
 *    e) MONITOR-LOOK = PORT-WAHL (render_pc.c pc_monitor_look). Der Original-Effekt (Punkt 1) bleibt, ist aber mit blossem
 *       Auge nicht zu erkennen (+-1 Zeile von 240, Pruefer-Messung Runde 1). RE2 hat kein sichtbareres Vorbild: dieselbe
 *       Zeilen-Funktion (RE2 FUN_8002ab84, Sinus-Tabelle @0x8009d9f4 = dieselben 256 Bytes wie RE1.5 @0x80072d70), einziger
 *       Aufrufer FUN_8002aa6c @0x8002ab04 (Wackel-Zweig), kein Monitor-Rahmen. Deshalb zeigt der Port die Szene IM Bildschirm
 *       des Monitors aus Cut 7 (das Standbild, das der Spieler gerade gesehen hat): das fertige Szenenbild (Hintergrund +
 *       Figuren + Zeileneffekt, vor Letterbox/Text) verkleinert in die weisse Bildschirmflaeche, dazu Bildzeilen, leicht
 *       entsaettigt und zum Rand des Schirms hin dunkler. Kein Text, kein Logo. Die Flaeche (re15_monitor_r41_schirm) ist am
 *       Hintergrundbild ROOM503x Cut 7 gemessen (Kopf der Tabelle k_schirm). Die Szene selbst bleibt unveraendert: mit
 *       RE15_MONITOR_OHNE_LOOK=1 sind ihre Bilder bitgleich zum abgenommenen Stand 23f9da5c9 (Beweis im Bericht).
 *    f) SICHERUNG (Pruefer STORY, Robustheit): startet die Szene im Hangar nicht binnen MON_START_MAX Takten (z. B. Bank
 *       nicht ladbar), geht es ohne Szene zurueck; laeuft das Monitorbild laenger als MON_BILD_MAX Takte, ebenso. PORT-WAHL.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_monitor_r41.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_scd.h"
#include "re15_enemy_ai.h"
#include "re15_birkin_formen.h"   /* re15_bf_hangar_phase (Runde 2: Standbild bis zum Szenenstart) */

extern int  re15_game_flag_get(uint8_t zone, uint8_t idx);
extern void re15_game_flag_set(uint8_t zone, uint8_t idx, int value);
extern int  re15_cine_active(void);
extern int  re15_room_transition_active(void);

#define MON_SCHNITT_MONITOR  7     /* ROOM5031 sub03 Cut_chg 7 @0x01194 */
#define MON_HALT_VOR         20    /* ROOM5031 sub03 Sleep `09 0a 14 00` @0x01196 */
#define MON_SCHNITT_WAND     9     /* ROOM5031 sub03 Cut_chg 9 @0x011EA */
#define MON_HALT_NACH        50    /* ROOM5031 sub03 Sleep `09 0a 32 00` @0x011F8 */
#define MON_SCHNITT_BILD     12    /* Hangar-Szene: BF_HANGAR_CUT (birkin_formen.c) */
#define MON_BILD_X           (-12117)   /* ROOM5030 Door slot 3 @0x00EC6, Nutzlast pos */
#define MON_BILD_Z           4472
#define MON_BILD_GIER        1024
/* PORT-WAHL (Kopf Punkt 5f, Sicherung, keine Original-Zahl): die Szene startet regulaer im 2. Takt nach dem Laden und dauert
 * BF_VORLAUF + 136 Takte (birkin_formen.c); beide Grenzen liegen weit darueber und greifen nur, wenn etwas fehlt. */
#define MON_START_MAX        60
#define MON_BILD_MAX         900

static struct {
    int      phase, t;
    unsigned raum;                 /* Monitor Room (5030/5031) */
    int32_t  x, y, z;              /* Stelle des Spielers beim Ausloesen */
    int16_t  gier;
    int      cut;                  /* Schnitt des Spielers beim Ausloesen (work_vars[0x0A]) */
    int      hart;                 /* 1 = der naechste re15_room_transition_present ist ein Monitor-Schnitt (Kopf Punkt 5a) */
} s_m;

int re15_monitor_r41_ohne_look(void)
{
    static int s = -1;
    if (s < 0) {
        const char *e = getenv("RE15_MONITOR_OHNE_LOOK");
        if (!(e && *e && *e != '0')) e = getenv("RE15_MONITOR_OHNE_ZEILEN");   /* Alias aus Runde 1 */
        s = (e && *e && *e != '0') ? 1 : 0;
    }
    return s;
}

/* Kopf Punkt 5d: Letterbox wie ROOM5031 sub03 (1,0x1B) @0x01146 .. @0x01210 - nicht im Messhaken (Bilder wie 23f9da5c9). */
static void mon_balken(int an)
{
    if (an && re15_monitor_r41_ohne_look()) return;
    re15_game_flag_set(1, 0x1B, an ? 1 : 0);
}

static int mon_raum(unsigned r)    { return (r & 0xFFFEu) == 0x5030u; }
static int hangar_raum(unsigned r) { return (r & 0xFFFEu) == 0x50A0u; }

int  re15_monitor_r41_phase(void) { return s_m.phase; }
/* Testhaken (Raumprobe ohne main.c): Zustand wie nach "Monitorbild angefordert" aus dem Monitor Room raum_mon. */
void re15_monitor_r41_testhaken_hin(unsigned raum_mon, int32_t x, int32_t y, int32_t z, int16_t gier, int cut)
{
    memset(&s_m, 0, sizeof s_m);
    s_m.phase = RE15_MON41_HIN; s_m.raum = raum_mon;
    s_m.x = x; s_m.y = y; s_m.z = z; s_m.gier = gier; s_m.cut = cut;
}
void re15_monitor_r41_reset(void) { memset(&s_m, 0, sizeof s_m); }
void re15_monitor_r41_stelle(int32_t *x, int32_t *y, int32_t *z, int16_t *gier, int *cut, unsigned *raum)
{
    if (x) *x = s_m.x;
    if (y) *y = s_m.y;
    if (z) *z = s_m.z;
    if (gier) *gier = s_m.gier;
    if (cut) *cut = s_m.cut;
    if (raum) *raum = s_m.raum;
}

int re15_monitor_r41_bild(void)
{
    return (s_m.phase == RE15_MON41_BILD || s_m.phase == RE15_MON41_ENDE) && hangar_raum(g_current_room_id);
}

int re15_monitor_r41_unterwegs(void)
{
    return (s_m.phase == RE15_MON41_HIN || s_m.phase == RE15_MON41_BILD || s_m.phase == RE15_MON41_ENDE ||
            s_m.phase == RE15_MON41_ZURUECK) && hangar_raum(g_current_room_id);   /* HIN: scd_room_setup.c markiert die Karte schon beim Laden */
}

unsigned re15_monitor_r41_raum(void) { return s_m.raum; }

int re15_monitor_r41_harter_schnitt(void)
{
    const int h = s_m.hart;
    s_m.hart = 0;
    return h;
}

int re15_monitor_r41_look(void)
{
    if (re15_monitor_r41_ohne_look()) return RE15_MON41_LOOK_AUS;
    switch (s_m.phase) {
    case RE15_MON41_VOR:
        return (g_current_room_id == s_m.raum && s_m.t > 0) ? RE15_MON41_LOOK_MERKEN : RE15_MON41_LOOK_AUS;
    case RE15_MON41_HIN:
    case RE15_MON41_BILD:
        if (!hangar_raum(g_current_room_id)) return RE15_MON41_LOOK_AUS;
        return (re15_bf_hangar_phase() == 0) ? RE15_MON41_LOOK_STANDBILD : RE15_MON41_LOOK_MONITOR;   /* 0 = BF_H_WARTEN */
    case RE15_MON41_ENDE:
        return hangar_raum(g_current_room_id) ? RE15_MON41_LOOK_MONITOR : RE15_MON41_LOOK_AUS;
    case RE15_MON41_ZURUECK:
        return RE15_MON41_LOOK_HALTEN;
    case RE15_MON41_NACH:
        return (s_m.t == 0) ? RE15_MON41_LOOK_HALTEN : RE15_MON41_LOOK_AUS;
    default:
        return RE15_MON41_LOOK_AUS;
    }
}

/* Cut_chg-Zwilling (LAB_800402a0: alter Cut @0x800402c0/@0x800402e4, work_vars[0x0C] @0x800402ec, work_vars[0x0A]
 * @0x800402fc, Zonen aus @0x800402d4), wie bf_hangar_starten / gatorszene_r41.c. */
static void mon_cut_chg(int cut)
{
    g_scd.cam_id_prev        = (uint8_t)g_scd.work_vars[0x0A];
    g_scd.cam_id             = (uint8_t)cut;
    g_scd.cam_arg2           = 0;
    g_scd.cam_arg3           = 0;
    g_scd.cam_change_pending = 1;
    g_scd.work_vars[0x0C]    = g_scd.work_vars[0x0A];
    g_scd.work_vars[0x0A]    = (int16_t)cut;
    g_scd.cut_auto_enabled   = 0;
}

static void mon_log(const char *was)
{
#ifdef RE15_PLATFORM_PC
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    fprintf(stderr, "[monitor] ROOM%04X %s: Phase %d, Spieler (%d,%d,%d) Gier %d hp %d Zustand %d, Schnitt %d (angefordert %d), "
                    "Zonen %d, Pad-Sperre %d, Effekt %d\n", (unsigned)g_current_room_id, was, s_m.phase, (int)pl->x, (int)pl->y,
            (int)pl->z, (int)pl->rot_y, (int)pl->hp, (int)pl->state, (int)g_scd.work_vars[0x0A], (int)g_scd.cam_id,
            (int)g_scd.cut_auto_enabled, re15_game_flag_get(2, 7), re15_game_flag_get(1, 0x1E));
#else
    (void)was;
#endif
}

static void mon_abbruch(const char *grund)
{
    re15_game_flag_set(1, 0x1E, 0);
    if (s_m.phase == RE15_MON41_VOR || s_m.phase == RE15_MON41_NACH) { re15_game_flag_set(2, 7, 0); mon_balken(0); }
    s_m.hart = 0;
    mon_log(grund);
    s_m.phase = RE15_MON41_AUS;
}

static int mon_ausloeser(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!mon_raum(g_current_room_id)) return 0;
    if (re15_game_flag_get(RE15_MON41_BANK, RE15_MON41_BIT)) return 0;
    if (re15_game_flag_get(3, 0x4C)) return 0;                     /* ROOM5000 @0x007FE / ROOM4070 @0x011FE */
    if (!pl->active || pl->hp <= 0 || pl->state != 1) return 0;     /* Zustand 1 = Pad-Zweig (@0x80073f90[1]) */
    if (re15_player_is_grabbed() || re15_cine_active()) return 0;
    /* Pause-Wort: nur die Sperr-Bits 0xFF000000 (Spieler/KI/Aktion/SCD/Pad, re15_scd.h RE15_PAUSE_*); die unteren Bits 7 stehen im
     * Spiel immer (Raumwechsel @0x8001cbc4 `ori 0x7`). */
    if ((g_re15_pauseflags & 0xFF000000u) != 0 || re15_room_transition_active() || g_room_change.pending) return 0;
    if (getenv("RE15_KEIN_MONITORBILD")) return 0;                  /* Testhaken: Gegenprobe ohne Monitorbild */
    return 1;
}

void re15_monitor_r41_tick(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    switch (s_m.phase) {
    case RE15_MON41_AUS:
        if (!mon_ausloeser()) return;
        s_m.raum = g_current_room_id;
        s_m.x = pl->x; s_m.y = pl->y; s_m.z = pl->z; s_m.gier = pl->rot_y;
        s_m.cut = (int)g_scd.work_vars[0x0A];
        re15_game_flag_set(2, 7, 1);                                /* ROOM5031 sub03 @0x01142 */
        mon_balken(1);                                              /* @0x01146 (Kopf Punkt 5d) */
        mon_cut_chg(MON_SCHNITT_MONITOR);                           /* @0x01194 */
        s_m.phase = RE15_MON41_VOR; s_m.t = 0;
        mon_log("Monitor Start");
        return;
    case RE15_MON41_VOR:
        if (g_current_room_id != s_m.raum) { mon_abbruch("Abbruch (Raumwechsel vor dem Monitorbild)"); return; }
        if (++s_m.t < MON_HALT_VOR) return;                         /* Sleep 20 @0x01196 */
        re15_game_flag_set(RE15_MON41_BANK, RE15_MON41_BIT, 1);
        re15_room_request_change(0x50A0u | (s_m.raum & 1u), MON_BILD_X, 0, MON_BILD_Z, MON_BILD_GIER, MON_SCHNITT_BILD);
        s_m.phase = RE15_MON41_HIN; s_m.t = 0;
        mon_log("Monitorbild angefordert");
        return;
    case RE15_MON41_HIN:
    case RE15_MON41_ZURUECK:
        return;                                                     /* re15_monitor_r41_raum_geladen */
    case RE15_MON41_BILD:
        if (!hangar_raum(g_current_room_id)) { mon_abbruch("Abbruch (Raum verlassen im Monitorbild)"); return; }
        if (s_m.t++ == 0) {
            /* erster KI-Takt nach dem Laden (harter Schnitt, Kopf Punkt 5a): nur die Szene - die Raumfiguren (Affen) aus. Die
             * Szene setzt ihre Figuren im naechsten Takt (birkin_formen.c re15_bf_raumtick); bis dahin zeigt der Zeichner das
             * Standbild von Cut 7 (Punkt 5b). */
            for (int i = RE15_ACTOR_SLOT_PLAYER + 1; i < RE15_ACTOR_MAX; i++) g_actors[i].active = 0;
            re15_game_flag_set(2, 7, 1);
            g_scd.cut_auto_enabled = 0;
            mon_log("Monitorbild");
        }
        /* Sicherung (Punkt 5f, PORT-WAHL) */
        if ((re15_bf_hangar_phase() == 0 && s_m.t > MON_START_MAX) || s_m.t > MON_BILD_MAX) {
            mon_log("Sicherung: Szene startet/endet nicht, zurueck");
            re15_monitor_r41_szene_ende();
        }
        return;
    case RE15_MON41_ENDE:
        if (!hangar_raum(g_current_room_id)) { mon_abbruch("Abbruch (Raum verlassen nach dem Monitorbild)"); return; }
        /* Kopf Punkt 5c: h135 ist gezeigt - jetzt zurueck */
        re15_game_flag_set(1, 0x1E, 0);                             /* @0x011E6 */
        re15_room_request_change(s_m.raum, s_m.x, s_m.y, s_m.z, s_m.gier, MON_SCHNITT_WAND);
        s_m.phase = RE15_MON41_ZURUECK; s_m.t = 0;
        mon_log("Monitorbild Ende, zurueck");
        return;
    case RE15_MON41_NACH:
        if (g_current_room_id != s_m.raum) { mon_abbruch("Abbruch (Raumwechsel nach dem Monitorbild)"); return; }
        if (s_m.t++ == 0) { re15_game_flag_set(2, 7, 1); g_scd.cut_auto_enabled = 0; mon_log("Monitorwand"); }
        if (s_m.t < MON_HALT_NACH) return;                          /* Sleep 50 @0x011F8 */
        mon_cut_chg(s_m.cut);                                       /* @0x01208 (Schnitt des Spielers statt Cut 1) */
        g_scd.cut_auto_enabled = 1;                                 /* Cut_auto 1 @0x0120A */
        re15_game_flag_set(2, 7, 0);                                /* @0x0120C */
        mon_balken(0);                                              /* @0x01210 (Kopf Punkt 5d) */
        s_m.phase = RE15_MON41_AUS; s_m.t = 0;
        mon_log("Monitor Ende");
        return;
    default:
        s_m.phase = RE15_MON41_AUS;
        return;
    }
}

void re15_monitor_r41_raum_geladen(void)
{
    if (s_m.phase == RE15_MON41_HIN) {
        if (!hangar_raum(g_current_room_id)) { mon_abbruch("Abbruch (falscher Raum statt Monitorbild)"); return; }
        s_m.phase = RE15_MON41_BILD; s_m.t = 0;
        g_scd.cut_auto_enabled = 0;                                 /* Cut 12 halten (Zonen aus wie Cut_chg @0x800402d4) */
        /* Monitor-Bild-Effekt an (Form ROOM5031 sub03 @0x011A8 Set(1,0x1E)=1) - ERST HIER: der Raumlader loescht die unteren 16
         * Bit von Zone 1 (FUN_800396fc @0x80039710-30, scd_room_setup.c), ein vorher gesetztes 0x1E ueberlebte den Wechsel nicht. */
        if (!re15_monitor_r41_ohne_look()) re15_game_flag_set(1, 0x1E, 1);
        re15_game_flag_set(2, 7, 1);                                /* Kopf Punkt 5d: Pad-Sperre ab dem Ladetakt */
        mon_balken(1);                                              /* (1,0x1B) @0x01146 bleibt bis @0x01210 (Punkt 5d) */
        s_m.hart = 1;                                               /* harter Schnitt Cut 7 -> Monitorbild (@0x011A6, Punkt 5a) */
        mon_log("Hangar geladen");
        return;
    }
    if (s_m.phase == RE15_MON41_ZURUECK) {
        if (g_current_room_id != s_m.raum) { mon_abbruch("Abbruch (falscher Raum statt Monitor Room)"); return; }
        s_m.phase = RE15_MON41_NACH; s_m.t = 0;
        g_scd.cut_auto_enabled = 0;                                 /* Cut 9 halten */
        re15_game_flag_set(2, 7, 1);                                /* Kopf Punkt 5d */
        mon_balken(1);
        s_m.hart = 1;                                               /* harter Schnitt Monitorbild -> Cut 9 (@0x011EA, Punkt 5a) */
        mon_log("zurueck im Monitor Room");
        return;
    }
    if (s_m.phase == RE15_MON41_VOR || s_m.phase == RE15_MON41_BILD || s_m.phase == RE15_MON41_ENDE ||
        s_m.phase == RE15_MON41_NACH)
        mon_abbruch("Abbruch (Raumwechsel)");
}

void re15_monitor_r41_szene_ende(void)
{
    if (s_m.phase != RE15_MON41_BILD) return;
    /* Kopf Punkt 5c/5d: bf_riegel(0) hat eben (2,7)/(1,27) geloescht - das Monitorbild haelt beide bis @0x0120C/@0x01210;
     * der Rueckweg folgt im naechsten Takt (Phase ENDE), damit das letzte Szenenbild h135 noch gezeigt wird. Den Zeileneffekt
     * (1,0x1E) loescht erst der Rueckweg (@0x011E6), h135 traegt ihn also noch. */
    re15_game_flag_set(2, 7, 1);
    mon_balken(1);
    g_scd.cut_auto_enabled = 0;                                     /* bf_hangar_beenden gibt die Zonen frei - Cut 12 halten */
    s_m.phase = RE15_MON41_ENDE; s_m.t = 0;
    mon_log("Szene Ende, letztes Bild");
}

/* ---- Monitor-Bild-Effekt (Punkt 1 des Kopfs) ---------------------------------------------------------------------- */
static const int8_t k_sinus[256] = {   /* DAT_80072d70 (PSX.EXE Datei 0x63570), 256 x int8 */
      3,   6,   9,  12,  15,  18,  21,  24,  28,  31,  34,  37,  40,  43,  46,  48,  51,  54,  57,  60,  63,  65,  68,  71,
     73,  76,  78,  81,  83,  85,  88,  90,  92,  94,  96,  98, 100, 102, 104, 106, 108, 109, 111, 112, 114, 115, 117, 118,
    119, 120, 121, 122, 123, 124, 124, 125, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 126, 126, 125,
    124, 124, 123, 122, 121, 120, 119, 118, 117, 115, 114, 112, 111, 109, 108, 106, 104, 102, 100,  98,  96,  94,  92,  90,
     88,  85,  83,  81,  78,  76,  73,  71,  68,  65,  63,  60,  57,  54,  51,  48,  46,  43,  40,  37,  34,  31,  28,  24,
     21,  18,  15,  12,   9,   6,   3,   0,  -3,  -6,  -9, -12, -15, -18, -21, -24, -28, -31, -34, -37, -40, -43, -46, -48,
    -51, -54, -57, -60, -63, -65, -68, -71, -73, -76, -78, -81, -83, -85, -88, -90, -92, -94, -96, -98,-100,-102,-104,-106,
   -108,-109,-111,-112,-114,-115,-117,-118,-119,-120,-121,-122,-123,-124,-124,-125,-126,-126,-127,-127,-127,-127,-127,-127,
   -127,-127,-127,-127,-127,-126,-126,-125,-124,-124,-123,-122,-121,-120,-119,-118,-117,-115,-114,-112,-111,-109,-108,-106,
   -104,-102,-100, -98, -96, -94, -92, -90, -88, -85, -83, -81, -78, -76, -73, -71, -68, -65, -63, -60, -57, -54, -51, -48,
    -46, -43, -40, -37, -34, -31, -28, -24, -21, -18, -15, -12,  -9,  -6,  -3,   0
};
#define MON_PHASE_SCHRITT  2       /* DAT_80072eb8 (lbu @0x800207d4) */
#define MON_ZEILE_SCHRITT  2       /* DAT_80072eb4 (lw @0x800207f4) */
#define MON_TEILER         0x40    /* DAT_80072eb0 (lw @0x80020818) */
#define MON_ZEILEN         0xF0    /* s4 = 0xf0 @0x800207ec */
static uint8_t s_phase;            /* DAT_800b5576 */

void re15_monitor_r41_zeilen_phase(uint8_t phase, uint8_t quelle[240])
{
    int n_kopf = 0, n_ende = 0;
    int16_t kopf_d[MON_ZEILEN], kopf_s[MON_ZEILEN], ende_d[MON_ZEILEN], ende_s[MON_ZEILEN];
    uint8_t s1 = phase;
    for (int z = 0; z < MON_ZEILEN; z++) {
        s1 = (uint8_t)(s1 + MON_ZEILE_SCHRITT);                          /* @0x800207fc / @0x80020800 */
        const int dy = (int)k_sinus[s1] / MON_TEILER;                     /* @0x80020820 div, @0x80020848 mflo (gegen 0) */
        if (dy == 0) continue;                                            /* FUN_80020894 @0x8002089c */
        int q = z + dy;
        if (q >= MON_ZEILEN) q = MON_ZEILEN - 1; else if (q < 0) q = 0;   /* 0xef / Zeile 0 */
        if (dy < 0) { kopf_d[n_kopf] = (int16_t)z; kopf_s[n_kopf] = (int16_t)q; n_kopf++; }
        else        { ende_d[n_ende] = (int16_t)z; ende_s[n_ende] = (int16_t)q; n_ende++; }
    }
    for (int z = 0; z < MON_ZEILEN; z++) quelle[z] = (uint8_t)z;
    for (int i = n_kopf - 1; i >= 0; i--) quelle[kopf_d[i]] = quelle[kopf_s[i]];   /* Kopf der Kette: zuletzt eingefuegt zuerst */
    for (int i = 0; i < n_ende; i++)      quelle[ende_d[i]] = quelle[ende_s[i]];   /* Ende der Kette: in Zeilenfolge */
}

int re15_monitor_r41_zeilen(uint8_t quelle[240])
{
    if (!re15_game_flag_get(1, 0x1E)) return 0;
    if (re15_game_flag_get(1, 0x1C) || re15_game_flag_get(1, 0x1D)) return 0;   /* FUN_80020674-Zweige: nicht gebaut (Kopf) */
    s_phase = (uint8_t)(s_phase + MON_PHASE_SCHRITT);                            /* @0x800207e0 */
    re15_monitor_r41_zeilen_phase(s_phase, quelle);
    return 1;
}

/* ---- Bildschirm des Monitors in Cut 7 (Kopf Punkt 5e, PORT-WAHL) --------------------------------------------------------
 * Gemessen (Runde 2, kein Original-Wert, sondern die Lage im Hintergrundbild): Framedump F25 nach dem Debug-Sprung in ROOM5030 und
 * ROOM5031, Cut 7 (RE15_WINDOW_SCALE=1, 320x240), Helligkeitsprofil (77R+150G+29B)/256 quer ueber die Kanten der weissen
 * Bildschirmflaeche (Flaeche ~231, Rahmen 0..80, Fase oben/rechts 115..185):
 *   links  Zeile 60: x72 0 / x73 129 / x74 237;  Zeile 165: x69 13 / x70 174 / x71 237   -> 73.5 oben, 70.5 unten
 *   rechts Zeile 60: x233 223 / x234 175;        Zeile 165: x236 230 / x237 238 (Fase)  -> 233.5 oben, 236.5 unten
 *   oben   Spalte 160: y52 134 / y53 173 / y54 230 (Spalte 230: y53 137 / y54 221)      -> 53.5
 *   unten  Spalte 160: y168 228 / y169 111;  Spalte 75: y167 231 / y168 118              -> 168.5
 * Beide Raeume zeigen in Cut 7 denselben Monitor (gleiche Werte). */
static const float k_schirm[8] = { 73.5f, 53.5f,  233.5f, 53.5f,  236.5f, 168.5f,  70.5f, 168.5f };

int re15_monitor_r41_schirm(unsigned raum, float ecken[8])
{
    if (!mon_raum(raum)) return 0;
    memcpy(ecken, k_schirm, sizeof k_schirm);
    return 1;
}

/* Messzeile (RE15_HANGAR_LOG, kein Spielverhalten): je gezeichnetem Bild im Hangar waehrend des Monitorbilds, was der Zeichner zeigt
 * (Modus re15_monitor_r41_look) und ob die Szenenfiguren da sind - Riegel gegen das Aufploppen (Pruefer SICHT Runde 1, R1). */
void re15_monitor_r41_bild_log(unsigned bild, int modus)
{
#ifdef RE15_PLATFORM_PC
    static int an = -1;
    if (an < 0) { const char *e = getenv("RE15_HANGAR_LOG"); an = (e && *e && *e != '0') ? 1 : 0; }
    if (!an || !hangar_raum(g_current_room_id)) return;
    if (s_m.phase != RE15_MON41_BILD && s_m.phase != RE15_MON41_ENDE) return;
    int b = 0, sp = 0;
    for (int i = RE15_ACTOR_SLOT_PLAYER + 1; i < RE15_ACTOR_MAX; i++) {
        if (!g_actors[i].active) continue;
        if (g_actors[i].type == 0x30u) b++;
        if (g_actors[i].type == 0x2Bu) sp++;
    }
    fprintf(stderr, "[monitor-bild] F%u ROOM%04X Phase %d Modus %d Birkin %d Spider-Man %d Szenenphase %d Letterbox %d\n", bild,
            (unsigned)g_current_room_id, s_m.phase, modus, b, sp, re15_bf_hangar_phase(), re15_game_flag_get(1, 0x1B));
#else
    (void)bild; (void)modus;
#endif
}
