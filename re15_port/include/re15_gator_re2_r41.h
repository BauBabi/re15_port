/*
 * re15_gator_re2_r41.h — Runde 41 Spur B "re2gator": das RE2-Verhalten des Alligators (EM23)
 * fuer den ROOM2090-Boss (Nutzer 2026-10-23: "Du kannst fast alles vom Aligator Fight von
 * Resident Evil 2 uebernehmen ... das gleiche Verhalten bei Animation und schreien ... an
 * gleichen Stellen beschleunigt ... die Waende ab und an wackeln ... Putz runter faellt ...
 * Kanister ... aufnehmen, GENAUSO wie im original ... Sterbe Animation mit geplatzten Kiefer").
 *
 * Quelle: info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN (@0x80100000) + info/re2leon/PSX.EXE,
 * alles selbst mit re2_disasm.py gelesen; Dossier analysis/befunde_runde41/B_re2gator.md §2.
 * Jede Konstante traegt ihre @0x-Adresse im Code. Was NICHT aus RE2 kommt, heisst hier
 * PORT-WAHL (Pool-Geometrie von ROOM2090 statt des RE2-Korridors ROOM40A0).
 *
 * Eigentuemer der Zustaende bleibt enemy_ai_boss_gator.c (Phasen, Wegfindung, Klettern ueber
 * die Rampe = Nutzer-Vorgabe); dieses Modul liefert die RE2-Bausteine je Boss-Slot.
 */
#ifndef RE15_GATOR_RE2_R41_H
#define RE15_GATOR_RE2_R41_H

#include <stdint.h>
#include "re15_actor.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- HP-Sockel (INIT @0x80100518-580: 25000 + Tabelle[0x800cfb74 & 0x40/0x20] = 300) ---- */
#define RE15_GR2_HP_SOCKEL  25000   /* `slti v0,v0,25000` @0x80101ec4 -> +0x218 |= 0x20 (geschwaecht) */
#define RE15_GR2_HP_INIT    25300   /* 25000 + [0x801045bc] = 300 (Bit 0x40 aus) */

/* ---- Hysterese Schwimmen (Clip 0) <-> Paddeln (Clip 1), Sub 1/2 ------------------------- */
#define RE15_GR2_PADDEL_AB  0x65    /* +0x1F0 >= 101 -> 0x201  `sltiu v0,v0,0x65` @0x80100b88/@0x80100ba4 */
#define RE15_GR2_SCHWIMM_AB 0x5a    /* +0x1F0 <  90  -> 0x101  `sltiu v0,v0,0x5a` @0x80100be4/@0x80100ca4 */
#define RE15_GR2_BISS_CD_INTRO 110  /* +0x238 = 110 nach dem Intro @0x8010091c-20 */
#define RE15_GR2_BISS_CD_BISS   70  /* +0x238 = 70 nach dem Chomp-Commit @0x801016b0-b4 */

/* ---- Grossreaktion R4 (@0x80101f8c-fd4) ---------------------------------------------------- */
#define RE15_GR2_R4_CD_BASIS 120    /* +0x22E = (rnd & 0x3f) + 120 @0x80101fc8-cc */

/* ---- Kanister (T2 @0x801017c4, Tabelle @0x80100024) ---------------------------------------- */
#define RE15_GR2_KAN_FENSTER_AB  8601   /* obj[1].z - z - 8601 in [0, 0x18f) @0x80100ae4-e8 */
#define RE15_GR2_KAN_FENSTER_LEN 0x18f
#define RE15_GR2_KAN_VORHALT     8100   /* Kanister >= eigene z + 8100 waehrend Stufe 2 @0x801018a4-c0 */
#define RE15_GR2_KAN_LOKAL_X     1750   /* Ziel-Lokallage im Maul @0x801019c4 / @0x80101a30 / ... */
#define RE15_GR2_KAN_LOKAL_Y     -210   /* @0x80101a4c / @0x80101ad4 / @0x80101b84 */
#define RE15_GR2_KAN_ENTFERN_F   9      /* +0x16B = 9: Kanister 9 Bilder nach R1-Start weg @0x80102394 */

typedef void (*re15_gr2_se_fn)(int se_id);           /* ENEMSE Bank 17 (gb_se des Boss-Moduls) */

/* Zuruecksetzen je Boss-Slot (Boss-INIT) + SE-Ausgang anmelden. */
void re15_gr2_reset(int slot, re15_gr2_se_fn se);
/* Jedes neue Frame-Wort des gerade gespielten Clips (Root @0x801003f0-40c: Top-5-Bits 0x28 ->
 * Ereignis 1). Der Boss-Tick ruft das aus seinem Frame-Flag-Block. */
void re15_gr2_frame_wort(int slot, uint32_t wort);
/* Ein Takt je Boss-Tick (vor dem Phasen-Switch): Ereignis-Maschine (+0x227/+0x16A), Schuetteln,
 * Rumble, Putz, Zaehler (+0x238/+0x22E), Kanister-Vorpass (nur wenn `im_chase`). */
void re15_gr2_takt(int slot, re15_actor_t *e, re15_actor_t *pl, int im_chase);

/* Verfolgungs-Clip nach der RE2-Hysterese setzen (e->motion/anim_frame) und die AUTHORED
 * Wurzelgeschwindigkeit dieses Bildes (Clip 0/1: off[f] - off[f-1], FUN_80015e7c, x-Komponente,
 * skaliert mit e->render_scale_q12) liefern. `dist` = Wurzel<->Spieler (re15_enemy_player_dist). */
int  re15_gr2_chase_clip(int slot, re15_actor_t *e, int32_t dist, int32_t *vortrieb);
void re15_gr2_biss_cd(int slot, int n);
int  re15_gr2_biss_cd_rest(int slot);
/* Nachbesserung 2 (Abnahme 1, M3) — Kanister-Aufnahme GENAU wie RE2:
 *  - Beckenzone des Boss-Moduls (gb_zone) anmelden: Gegenstueck der RE2-Zonen (PORT-WAHL wie der Dash).
 *  - hysterese_ruht: 1 = der RE2-Sub-1/2-Pre springt an der Paddel-Hysterese vorbei (Sub bleibt): Zone des
 *    Kanisters + Raumflag Bank 5 Bit 0 (@0x80100ab8-c8, @0x80100aec, @0x80100afc) oder Stufe nicht 0/7
 *    (@0x80100b20-30). Beim Aufnahmebeginn schaltet der Takt Paddeln -> Schwimmen (@0x80100b04-18 -> @0x80100c04).
 *  - biss_erlaubt: 0 waehrend Stufe 1..6 (RE2 macht die Biss-Maschine T1 nur bei Stufe 0/7 scharf, FUN_801012fc
 *    @0x80101360-70); das Boss-Modul fragt es vor jedem Biss-Start. */
void re15_gr2_zonen_quelle(int (*zone_fn)(int32_t z));
int  re15_gr2_hysterese_ruht(int slot, const re15_actor_t *e);
int  re15_gr2_biss_erlaubt(int slot);

/* RE2-LUNGE als Zonen-Dash: `zone` = aktuelle Beckenzone des Gators (gb_zone), `zone_leon` =
 * Leons Zone. Liefert 1, wenn eben eine neue Zone betreten wurde, in der Leon ist (RE2: Zonenlinie
 * +0x21A 0->1->2 @0x801009a8/@0x801009ec = Routine 0x301, geprueft VOR der Biss-Sperre +0x238).
 * ziel_vor[] = Ende der LAENGS-Bahn (x-Netto Clip 2 + 3, skaliert, um den Yaw gedreht) fuer die
 * Insel-Probe des Boss-Moduls; die Quer-Komponente (RE2-Korridor-Ecke) nimmt die Dispatcher-Klemme.
 * Nachbesserung 1 (Abnahme 0, M1): die Bahn laeuft bildweise (FUN_80015e7c/152c8-Delta) statt absolut. */
int  re15_gr2_dash_zonenwechsel(int slot, const re15_actor_t *e, int zone, int zone_leon, int32_t ziel_vor[2]);
/* SE 4 @0x80100d64/84, Clip 2 (16); `pl` = Leon (Putz-Lage, PORT-WAHL "um Leon"). */
/* Nachbesserung 2 (Abnahme 1 M2, Entscheid B7): Start-Gier mit freier RE2-Wende (Kopf/Rumpf im Wasser, Wurzel
 * ausserhalb der Klemmzone, Bahn nicht durch Leon; Drehung <= 0x400, kleinste Drehung vor Ausrichtung auf Leon) -
 * 0 = kein Dash an diesem Zonenwechsel. */
int  re15_gr2_dash_planen(int slot, const re15_actor_t *e, const re15_actor_t *pl, int32_t ziel_vor[2]);
void re15_gr2_dash_start(int slot, re15_actor_t *e, const re15_actor_t *pl);   /* dreht erst auf die Plan-Gier */
int  re15_gr2_dash_tick(int slot, re15_actor_t *e);       /* 1 = laeuft, 0 = fertig (0x101, yaw += 1024 @0x80100eb0-c0) */
int  re15_gr2_im_dash(int slot);                          /* +0x225 */
int  re15_gr2_bone_welt_pub(const re15_actor_t *e, int bone, int32_t pos[3]);   /* Messhaken: Bone-Welt mit Zeichen-Skala */
int  re15_gr2_dash_vordrehung(int slot);                  /* Nachbesserung 2: Drehung auf die Plan-Gier vor Clip 2 laeuft */

/* RE2-INTRO (Sub 0 P1/P2 @0x8010086c-948): Clip 6 (Durchbruch) -> Clip 3 (Schnapper), danach
 * +0x238 = 110 und yaw += 1024 (@0x80100938-48). Bahn-Ende (Wurzel) in ziel[] nur fuer die Telemetrie. */
void re15_gr2_intro_bahn(const re15_actor_t *e, int32_t ziel[2]);
void re15_gr2_intro_start(int slot, re15_actor_t *e);
/* Nachbesserung 2 (Abnahme 1 M2, Entscheid B7): Intro-Startlage/-gier (Pos_set-Zwilling, RE2 sub11 @SCD 0x0736), so
 * dass die ganze RE2-Bahn (Clip 6 -> 3 -> yaw += 1024) Kopf/Rumpf im Wasser haelt und der Knick auf Leon zeigt;
 * vom Boss-Modul VOR re15_gr2_intro_start beim Aggro gerufen (Szenen von Spur F/G rufen nur intro_start). */
void re15_gr2_intro_lage(int slot, re15_actor_t *e, const re15_actor_t *pl);
/* Beckengeometrie des Boss-Moduls: Aussenabstand eines Punkts zum Wasser (0 = drin), m = Rand nach innen. */
void re15_gr2_becken_quelle(int32_t (*draussen)(int32_t x, int32_t z, int32_t m));
/* Koerperlaengen-Push-Kreise des Boss-Moduls: 1 = Punkt (px,pz) liegt in einem Kreis des Gators bei (x,z,yaw). */
void re15_gr2_koerper_quelle(int (*kontakt)(int32_t x, int32_t z, int yaw, int32_t px, int32_t pz));
int  re15_gr2_intro_tick(int slot, re15_actor_t *e);      /* 1 = laeuft, 0 = fertig */
int  re15_gr2_im_intro(int slot);                         /* Messhaken: Intro laeuft (Clip 6/3) */

/* HURT-Selektor FUN @0x80101eac: 0 = Standard (SE 5, @0x80102090), 1 = Kanister-Explosion
 * (@0x80102158), 4 = Grossreaktion (SE 0, Clip 10, @0x80102bd0). `gesperrt` = Phase, in der
 * der Boss nicht flincht (CROSS/DIE, Nutzer-Design). Fuehrt +0x218 Bit 0x20 und +0x22E. */
int  re15_gr2_hurt_reaktion(int slot, re15_actor_t *e, const re15_actor_t *pl, int gesperrt);

/* R1 = Kanister-Explosionstod (@0x80102158): start einmal, dann tick je Bild; 0 = Endzustand
 * (P3 @0x80102654: Leiche haelt die Clip-9-Endpose). */
void re15_gr2_r1_start(int slot, re15_actor_t *e);
int  re15_gr2_r1_tick(int slot, re15_actor_t *e);
int  re15_gr2_r1_laeuft(int slot);

/* Zusatz-az je Bone (Root @0x8010037c-cc: +0x22A auf Part 6, +0x228 auf Part 7; T1/T2-Easing und
 * der Kiefer nach R1 FUN_80104318). Vom Spine-Haken des Boss-Moduls addiert. */
int  re15_gr2_bone_az(int slot, int bone);
/* Geplatzter Kiefer (FUN_80104318 @0x80104318-58, aus R1 P0 @0x80102354-58): 1, sobald Part-Record 7 des
 * RE2-Modellblocks gestempelt ist (Flagwort 0x4A, +0xA0 = 0x8000, +0x9A = -10, +0x70 = 0x202038) — der
 * Kiefer fliegt dann ueber die Part-Bruecke (re15_re2z_gore_part_matrix / _part_burst, re15_actor.h) als
 * dunkelrote Scherbenwolke 10/Bild nach oben und ist nach 29 Bildern weg (FUN_80028DAC @0x80028DC8-DC). */
int  re15_gr2_kiefer_geplatzt(int slot);

/* Bildschirm-Schuetteln (FUN_8002aa28/aa6c): Versatz (dx,dy) des HINTERGRUNDS fuer dieses Bild,
 * 0/0 wenn keines; Modus 2 = Bit 4 (stark), 1 = Bit 8 (schwach). */
void re15_gr2_schuettel_offset(int *dx, int *dy);
int  re15_gr2_schuettel_modus(void);
uint8_t re15_gr2_ereignis(int slot);                      /* +0x227 (Messhaken) */

/* ---- Kanister (Port-Prop aus ROOM40A0 Modell 1) ---------------------------------------------
 * setzen(x, y, z): ins Wasser legen (liegend, Dir_set 0x690/0/0xfc00 @ROOM40A0 sub05 0x048C);
 * Testhaken RE15_GATOR_KANISTER=x,z[,y] (Spur G setzt ihn ueber die Szene). */
void re15_gr2_kanister_setzen(int32_t x, int32_t y, int32_t z);
void re15_gr2_kanister_entfernen(void);
int  re15_gr2_kanister_stufe(void);                       /* +0x226: 0 frei .. 7 getragen, 13 explodiert, -1 keiner */
/* Lage fuer den Zeichner: 1 = sichtbar; pos = Welt, rot = Q12 3x3 (Welt). */
int  re15_gr2_kanister_welt(int32_t pos[3], int32_t rot[9]);
/* Umgebungs-Haken einmal auswerten (RE15_GATOR_KANISTER). */
void re15_gr2_kanister_env(void);

/* ---- Fuer Spur F/G ------------------------------------------------------------------------ */
int  re15_gr2_geschwaecht(void);     /* +0x218 & 0x20: HP < 25000 (= 300 Schaden genommen) */
int  re15_gr2_treffer(void);         /* Treffer seit INIT */
int  re15_gr2_tot(void);             /* R1-Endzustand erreicht (Leiche, +0x10E |= 0x4000 @0x80102654-64) */
int  re15_gr2_traegt(void);          /* Stufe 7 */

/* ---- Nachtrag 4: RE2-Kameraschnitte des Alligator-Kampfes (ROOM40A0) auf ROOM2090/2091 --------
 * NUTZER-VORGABE (AUFTRAG.md 22:37 "Ja, ich moechte RE2-artige Kameraschnitte"). FORM = RE2: dieselben
 * Ausloeser/Bilder/Sperren wie das RE2-Kameraskript (Dossier §9.1: 9 Rufe FUN_8002c7ac im EM23-Overlay,
 * Cut_chg nur in ROOM40A0 sub02), Sperre der Zonenkamera = RE2 0x800cfbd8 |= 0x100 (Hauptschleife
 * @0x80026394-ac) = RE1.5 DAT_800aca3c Bit 0x100 = Port g_scd.cut_auto_enabled = 0; Schnitt = RE1.5
 * Cut_chg-Zwilling (scd_vm.c op_cut_chg), Rueckkehr = Cut_old-Zwilling (FUN_8004032c).
 * ZUORDNUNG = PORT-WAHL: je RE2-Schnitt die ZONENKAMERA des Raums (RVD-Gruppen, ROOM2090: Cuts 0..7), die den
 * Kopf (Bone 6) zeigt, deren Bildmassstab H * Skala / Tiefe nicht groesser ist als der groesste RE2-Schnittmassstab
 * (Cut 0 bei Clip-6-Bild 70: H 257 / Tiefe 8898 - RE2 schneidet nie naeher heran) und deren "frontal"-Wert =
 * cos(Blickachse Bone 1 -> Bone 6, Kopf -> Kamera) dem des RE2-Schnitts am naechsten kommt (Soll je Blick in Q12,
 * gemessen mit tools/r41_re2gator/probe_kamera_re2.c an der RE2-Lage des Schnittbilds, Dossier §9.2/§9.3). */
#define RE15_GR2_BLICK_DURCHBRUCH 1   /* RE2 Cut 10: Clip 6 Bild 0..69 (SCD sub02 `29 0a` @0x00E0), soll +4092 */
#define RE15_GR2_BLICK_SEITE      2   /* RE2 Cut 0: Clip-6-Bild 70 (@0x80100894-a4), soll +1445 */
#define RE15_GR2_BLICK_ANGRIFF    3   /* RE2 Cut 1: Clip-6-Ende -> Clip 3 (@0x801008bc-c8), soll +3386 */
#define RE15_GR2_BLICK_EXPLOSION  4   /* RE2 Cut 5: R1 P0, nur wenn der aktuelle Cut den Kopf nicht zeigt
                                       * (RE2: Cut 6/13 @0x80102218-24, Kopf dort bei Bild-x -2), soll +4046 */
#define RE15_GR2_BLICK_FINISHER   5   /* RE2 Cut 7/14: FRESSEN P0 (@0x80100fbc-ff4), Opfer mit im Bild, soll +4094 */

/* Beste Zonenkamera fuer `blick` (s.o.); `opfer` (oder NULL) muss fuer FINISHER mit im Bild liegen (sonst
 * Rueckfall ohne Opfer). -1 = keine Zonenkamera zeigt den Kopf. *frontal = Ist-Wert (Q12) der Wahl. */
int  re15_gr2_kamera_wahl(int blick, const re15_actor_t *gator, const re15_actor_t *opfer, int *frontal);
/* Programm-Schnitt fuer Szenen (Spur F/G): Sperre setzen + auf die Wahl schneiden (Cut_chg-Zwilling);
 * liefert den Cut oder -1 (dann nur Sperre). `programm` erscheint in gator_boss.log ("RE2 KAMERA ..."). */
int  re15_gr2_kamera_schnitt(int blick, const re15_actor_t *gator, const re15_actor_t *opfer, const char *programm);
/* Programm-Ende: zurueck auf den Zonen-Cut vor dem Programm und Zonenkamera wieder an (Cut_old-Zwilling). */
void re15_gr2_kamera_frei(const char *programm);
int  re15_gr2_kamera_gesperrt(void);                      /* 1 = ein Programm haelt die Kamera */
/* Automatische Programme (Intro/R1/Finisher) an/aus — eine Szene, die eigene Schnitte setzt, schaltet sie aus. */
void re15_gr2_kamera_auto(int an);
/* Finisher (FRESSEN P0, Haken in enemy_ai_boss_gator.c NACH der Opfer-Platzierung): Sperre + FINISHER-Wahl. */
void re15_gr2_kamera_finisher(int slot, const re15_actor_t *gator, const re15_actor_t *opfer);
/* Messhaken: Schnittfolge seit dem Boss-INIT (cut[i] = gewaehlter Cut, -1 = Sperre ohne Schnitt, -2 = frei;
 * blick[i] = RE15_GR2_BLICK_*, 0 = frei/zurueck; clip/bild = Gator-Clip und -Bild beim Ereignis). */
int  re15_gr2_kamera_folge(int max, int *cut, int *blick, int *clip, int *bild);

/* RE2-Bildtakt (VSync-Wert der Hauptschleife DAT_800dfc1a, FUN_8002b968 @0x8002b990-98): 2 im Spiel, 4 von R1 P0
 * (@0x8010235c-64) bis R1 P3 (@0x80102668/7c) = halbe Bildrate waehrend der Kiefer-Explosion. */
int  re15_gr2_vsync(void);

/* ---- PC-Haken ------------------------------------------------------------------------------ */
/* platform/pc/src/gator_re2_pc.c: Bildbudget der Hauptschleife (main.c Bild-Deckel) = Grundbudget x VSync / 2;
 * schreibt jeden Wechsel mit SDL_GetTicks nach debug.log ("[gator-re2] VSync ..."). */
unsigned re15_gr2_pc_bildbudget(unsigned grund_ms);
extern void (*re15_gr2_raum_se_hook)(int satz);   /* ROOM40A0-Raumbank: 15 = Explosionsbrueller (@0x8010237c-88) */
/* platform/pc/src/gator_re2_pc.c: Kanister-Prop zeichnen (Aufruf aus dem Prop-Pass von main.c). */
struct re15_camera_view_s;
void re15_gr2_pc_zeichnen(const void *cam_view, int cx, int cy);
/* Nachbesserung 3 (Abnahme 2 M1, Entscheid B9): NPC-Zeichner-Haken (main.c, vor Schatten + Mesh). 1 = diesen Aktor
 * nicht zeichnen, weil der Boss-Alligator untergetaucht ist (re15_gator_boss_untergetaucht: RE2 Sce_em_set y = +3000
 * @RDT 0x25A0 bis zum Pos_set im Intro @SCD 0x0736). `gecullt` = Regions-Cull des Zeichners (nur fuer die Messzeilen).
 * Messzeilen debug.log "[gator-re2] Sicht ...": Wechsel untergetaucht/aufgetaucht mit Bild, Lage, Clip/Bild; erstes
 * gezeichnetes Bild; groesster Lagesprung zwischen zwei aufgetauchten Bildern. */
int  re15_gr2_pc_untergetaucht(const re15_actor_t *npc, int gecullt);
/* Messhaken: Zahl der gespawnten Putz-Partikel (re2fx) seit Start + Welt-Lage des letzten Spawns
 * (Nachbesserung 1, M2: die exe-Zeile "RE2 PUTZ runde ..." in gator_boss.log traegt beides). */
int  re15_gr2_putz_spawns(void);
void re15_gr2_putz_letzte(int32_t pos[3]);

/* Messhaken: Log-Zeilen (gator_boss.log, "RE2 ...") an/aus. */
void re15_gr2_log(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif /* RE15_GATOR_RE2_R41_H */
