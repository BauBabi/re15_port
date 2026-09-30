/*
 * fx_plattform_pc.h — Runde 34 (Granaten), Spur C (Plattform): die fensterlos pruefbaren Teile
 * des PC-Effektpfads. Dossier: analysis/befunde_runde34_granaten/bau_c.md, Plan BAUPLAN §3.3.
 *
 * Alles hier ist bewusst ohne SDL: die Unit-Sonde probe_r34_plattform linkt diese Datei gegen
 * re15_engine + re15_test_support und prueft Takt, Haken-Bindung, Ton-Weiche, Licht-Latch und
 * die TEX.TIM-Effektseiten, ohne ein Fenster zu oeffnen. main.c ruft die Funktionen an den
 * Stellen, die der Original-Hauptlauf FUN_8001c6e8 vorgibt (Adressen je Funktion).
 */
#ifndef FX_PLATTFORM_PC_H
#define FX_PLATTFORM_PC_H

#include <stdint.h>
#include <stddef.h>
#include "re15_light.h"
#include "re15_esp.h"
#include "re15_tim.h"

/* ===== C1 — ESP-Takt HINTER dem Spielschritt (E10) =========================================
 * Original-Hauptlauf (RE1.5 PSX.EXE, selbst disassembliert):
 *   8001ce04  jal 0x8001a50c      Gegner
 *   8001ce0c  jal 0x80031c44      Spieler inkl. Waffen-FSM (spawnt Muendung/Huelse/Granate)
 *   8001ce2c  jal 0x80019e20      ESP-Tick
 *   8001ce34  jal 0x8001db28      Item-Modal
 *   8001ce60  lbu v0,21336(v0)    Licht-Latch-Leser (0x800b5358)
 * Im Port lief der ESP-Tick im SCD-30-Hz-Zweig VOR re15_game_step — jede im Spielschritt
 * gespawnte Partikel tickte ein Bild zu spaet. Neu: der SCD-Zweig gibt den Takt nur FREI
 * (re15_pc_fx_takt_setzen(1) an der alten Stelle, 0 zu Beginn der Zweig-Kette), und
 * re15_pc_fx_takt() laeuft direkt hinter re15_game_step: erst der RE1.5-ESP-Tick, dann die
 * RE2-FX-Pumpe (RE2: Gegner-Schleife 0x800267c0-0x80026930 vor `jal 0x8001d300` @0x80026980). */
void re15_pc_fx_takt_setzen(int frei);
/** Ist der Takt dieses Bilds freigegeben (und noch nicht gelaufen)? */
int  re15_pc_fx_takt_frei(void);
/** Einmal je freigegebenem Bild: ESP-Tick + RE2-FX-Tick; Freigabe verbraucht. 1 = lief. */
int  re15_pc_fx_takt(void);

/* ===== C8 — Harness RE15_FORCE_AUFSCHLAG="<re2_art>@<bild>[,<re2_art>@<bild>...]" ===========
 * Mess-Haken (kein Spielverhalten, Muster RE15_FORCE_SPLAT): ruft im Spielbild <bild> vor dem
 * ESP-/RE2-FX-Takt re2fx_aufschlag(re2_art, q, gier) mit q = 1500 vor Leon auf Bodenhoehe
 * (lokal +x = Blickrichtung, RotMatrix(0, rot_y, 0) wie K1 ROTY) und gier = Leons rot_y —
 * die Sichtabnahme des Saeure- (2) / Brand-Aufschlags (1) vor dem Merge von Spur A. Die 1500 ist
 * eine HARNESS-WAHL (Abstand vor der Kamera), keine Original-Konstante.
 * Reine Parse-/Rechenhilfe fuer main.c und die Sonde: liefert fuer `bild` die Art (1/2) und q,
 * 0 = kein Eintrag fuer dieses Bild. */
#define RE15_PC_FORCE_AUFSCHLAG_ABSTAND 1500

/* ===== Integration W6 — Harness RE15_FORCE_EXPLOSION="<art>@<bild>:<slot>[,...]" ==============
 * Mess-Haken (kein Spielverhalten, Muster RE15_FORCE_AUFSCHLAG): im Spielbild <bild> vor dem
 * ESP-/RE2-FX-Takt die Explosion einer Granate der Art 2/3/4 so, als laege sie am Gegner <slot>
 * (Routine 31: P = Lage mit y - 500, FUN_80012d60(500, &P, Art); Art 3/4 zusaetzlich der RE2-
 * Aufschlag, E8). Zweck: die Sichtabnahme der RE2-Part-Farben (Hund-/Spinnen-Tod Zeile 10/11)
 * unabhaengig von der Wurfgeometrie — Wurf und Flug nimmt integration_r34_granaten ab. */
int re15_pc_force_explosion_eintrag(const char *spec, unsigned bild, int *art, int *slot);
#include "re15_actor.h"
int re15_pc_force_explosion(int art, const re15_actor_t *ziel);
int re15_pc_force_aufschlag_eintrag(const char *spec, unsigned bild, int32_t px, int32_t py,
                                    int32_t pz, int16_t rot_y, int *re2_art, int32_t q[3]);

/* ===== C2 — ESP-Zeichnen: Sichtbarkeit, Lage, CLUT/TPAGE, Zellgroesse =======================
 * Zeichenschleife FUN_80053240 / Sprite-Bau FUN_800534c4 (RE1.5 PSX.EXE, selbst disassembliert):
 *   800532f0  addiu s0,s0,-132        Platz 95 -> 0 (s0 startet bei Pool+96*0x84+0x6c)
 *   800532fc  andi v0,v1,0x1 / beq    aktiv?
 *   80053308  andi v0,v1,0x2 / beq    sichtbar?
 *   80053314  lh v0,-68(s0) (+0x28), 80053324 lh -66 (+0x2a), 80053330 lh -64 (+0x2c)
 *             -> Regions-Test FUN_80014368 @0x80053334 mit der WELTLAGE slot+0x28
 *   8005350c  addiu v0,a1,40          RTPS auf slot+0x28 (lwc2 @0x80053514/18, RTPS @0x80053534)
 *   80053538  lhu v0,50(a1)           CLUT = slot+0x32   (<<16 in fp, Wort 3 des POLY_FT4)
 *   8005353c  lhu v1,48(a1)           TPAGE = slot+0x30  (<<16 in s7, Wort 5)
 *   800535d0  lhu v0,4(a1)            defW = slot+0x04 (Zeilenkopie +0x04), mult mit step16
 *   800535e0  lhu v0,6(a1)            defH = slot+0x06
 * Die Kettenverknuepfung @0x80053778-0x800537b0 haengt die n Quads eines Platzes IN Reihenfolge
 * vor den bisherigen Kopf des OT-Eimers (SZ3>>6 @0x80053620): innerhalb eines Platzes Quad 0
 * zuerst (unten), zwischen Plaetzen im selben Eimer der spaeter gebaute (kleinerer Index) zuerst.
 * Der Port reiht Plaetze 0 -> 95 und Quads 0 -> n-1 in eine STABIL nach Tiefe sortierte Liste
 * (render_pc.c) — bei gleichem Schluessel ergibt das genau diese Reihenfolge. */

/** Sichtbar genau dann, wenn Flags Bit 0 UND Bit 1 (@0x800532fc-0c). Plaetze ohne Row-VM
 *  (Altpfad, flags nie gefuehrt) bleiben sichtbar wie bisher. */
int      re15_pc_esp_sichtbar(const re15_esp_fx_t *f);
/** Die Weltlage, die das Original projiziert und cullt (slot+0x28/2a/2c, V1a). Solange Spur A
 *  wpos nicht fuellt (alle drei 0), Rueckfall auf die bisherige Port-Lage x + xlat. */
void     re15_pc_esp_weltlage(const re15_esp_fx_t *f, int32_t out[3]);
/** CLUT-Wort slot+0x32: der gefuehrte Wert der Row-VM-Plaetze, sonst die Spawner-Saat aus dem
 *  EFF-Kopf (FUN_80019700: hdr u16 @+4 + ((sub & 0xff) >> 3) * 0x40). 0 = unbekannt. */
uint16_t re15_pc_esp_clut(const re15_esp_fx_t *f);
/** TPAGE-Wort slot+0x30: gefuehrt bzw. Saat hdr u16 @+6. 0 = unbekannt. */
uint16_t re15_pc_esp_tpage(const re15_esp_fx_t *f);
/** defW/defH (Q12) = slot+0x04/+0x06 der Zeilenkopie (@0x800535d0/@0x800535e0, lhu). Plaetze
 *  ohne Row-VM haben keine Zeile: 0x1000 wie bisher. */
void     re15_pc_esp_defwh(const re15_esp_fx_t *f, int32_t *w, int32_t *h);

/* ===== C2 — die zwei Effektseiten der GLOBAL-Bank aus DATA/TEX.TIM ===========================
 * CORE00.ESP-Kopfworte CLUT/TPAGE (Datei eff_start+4/+6, gelesen): Id 3 `11 78 1e 00` @0x00C,
 * Id 8 `11 79 1e 00` @0x62C, Id 0 `51 79 1f 00` @0x828, Id 2 `51 7a 1f 00` @0xF04,
 * Id 4 `d1 7a 1f 00` @0x172C.
 * TPAGE 0x1e -> VRAM (896,256), 0x1f -> (960,256) (psx-spx: x = (tp & 0xf)*64, y = (tp>>4 & 1)*256).
 * DATA/TEX.TIM: Kopf `10 00 00 00 08 00 00 00` (4 bpp + CLUT); CLUT-Block @0x08
 * `0c 06 00 00 00 01 e0 01 20 00 18 00` = VRAM (256,480) 32x24, Eintraege ab @0x14; Bild-Block @0x614
 * `0c 80 02 00 00 00 00 00 40 01 00 01` = VRAM (0,0) 320 hw x 256, Pixel ab @0x620.
 * Die Halbwort-Spalten 192..319 liegen im VRAM bei (896..1023,256)
 * — gemessen bitgleich gegen die ShowVRAM-Grundwahrheit (tools/tex_tim_effect_slice.py
 * --verify-vram: 0/32768 Abweichungen bei Quellspalte 192; derselbe Weg liefert seit 2026-08-21
 * effect8_fire.tim, gepinnt in test_1090_fire_pin). Die Effekt-CLUTs liegen in Blockspalte 16
 * (x 272, CLUT-Wort & 0x3f = 0x11), Zeilen 480..494 werden benutzt (Rauch 480, Rauch sub 8..15 481,
 * Feuerball 483, Feuer 484, Blut 485, Muendung 489, Zweitblitz 490, Huelse 491, Granate 492).
 * Hochgeladen werden die Zeilen 480..495 (16 Paletten -> Textur 256 x 4096): jede Seite dekodiert
 * mit JEDER Palette, der Zeichner waehlt die Palette ueber das CLUT-Wort des Platzes. */
#define RE15_PC_FX_CLUT_X      272
#define RE15_PC_FX_CLUT_Y0     480
#define RE15_PC_FX_CLUT_ZEILEN 16
typedef struct {
    uint16_t pix[64 * 256];                         /* 4 bpp, 256 x 256 Texel = 64 Halbworte je Zeile */
    uint16_t clut[RE15_PC_FX_CLUT_ZEILEN * 16];     /* Zeilen 480..495, je 16 Eintraege ab x 272      */
} re15_pc_fx_seite_t;
/** Seite `tpage` (0x1e / 0x1f) aus dem TEX.TIM-Puffer schneiden und als 4-bpp-TIM beschreiben
 *  (tim zeigt in `seite`, die also leben muss, solange tim benutzt wird). 0 = ok, < 0 = Fehler. */
int re15_pc_fx_seite_bauen(const uint8_t *tex, size_t n, uint16_t tpage,
                           re15_pc_fx_seite_t *seite, re15_tim_t *tim);
/** Liegt das CLUT-Wort in der hochgeladenen Palettenspanne der Effektseiten? */
int re15_pc_fx_seite_clut_ok(uint16_t clut);

/* ===== C3 — Licht-Latch-Leser (E11, K2 LICHT_LESER) =========================================
 * RE1.5 PSX.EXE, Hauptlauf hinter ESP-Tick und Item-Modal (selbst disassembliert):
 *   8001ce60  lbu v0,21336(v0)        Latch 0x800b5358; 0 -> weiter bei 0x8001d088
 *   8001ce7c  lh v1,4068(v1)          aktiver Cut 0x800b0fe4; Satz = [RDT+0x2c] + Cut*40
 *   8001cea0  jal 0x8004ee38          Kopie des Satzes (40 B, a2 = 0x28) nach [0x800ac77c]
 *   8001cebc  ori v0,zero,0x4b0       Vektor (1200, ?, 0) im Scratchpad 0x1f80002c/30
 *   8001cecc  jal 0x8004f008          = RotMatrix(0, Spieler +0x6a, 0) (@0x8004f038 jal 0x80068098)
 *                                       + ApplyMatrix (@0x8004f048 jal 0x800661c0, MVMVA sf=1)
 *   8001cef8  sb zero,3(v0)           Licht 2 Typ := 0 (Punktlicht)        -> type_flags[2]
 *   8001cf20  lbu 10 / sltiu 0xd2 / sb  Farbe R := max(R, 0xD2)             -> colors[2][0]
 *   8001cf5c  lbu 11 / sltiu 0x8c / sb  G := max(G, 0x8C)                  -> colors[2][1]
 *   8001cf98  lbu 12 / sltiu 0x50 / sb  B := max(B, 0x50)                  -> colors[2][2]
 *   8001cfe4  sh v1,28(v0)            x := Spieler-x (lhu 0x800aca88) + gedrehtes x -> positions[2][0]
 *   8001d018  addiu v1,v1,-800 / 8001d01c sh v1,30(v0)   y := Spieler-y - 800  -> positions[2][1]
 *   8001d058  sh v1,32(v0)            z := Spieler-z (lhu 0x800aca90) + gedrehtes z -> positions[2][2]
 *   8001d080  ori v1,zero,0x1770 / 8001d084 sh v1,38(v0)  Helligkeit 6000    -> brightness[2]
 *   8001d09c.. jal 0x8001e8c8          Spieler, dann Schleife ueber die Entity-Liste (@0x8001d0e8-164):
 *                                       beide rufen je Figur FUN_80053fc0 (@0x8001e94c) = Lichtrechnung
 *   8001d1ac  jal 0x8004ee38          Satz aus der Kopie zurueck
 *   8001d1b4  sb zero,0(s0)           Latch := 0
 *   8001d1b8  jal 0x80039ca0          ERST DANACH (Nachbesserung H7) die Tabelle 0x800af33c (Zaehler
 *                                       0x800afbb7, Eintraege aus FUN_80039b2c; Katalog "NPC render"),
 *   8001d1c0  jal 0x8002c18c          dann die Objekte (Props) -> beide sehen das Licht NICHT.
 *   Welche Port-Objekte FUN_80039ca0 entsprechen, ist OFFEN (bau_c.md NACHBESSERUNG); die NPC-Typen
 *   0x40..0x4D sind Entitaeten der Liste 0x800acc2c = Figuren-Schleife MIT Licht.
 * Port: re15_pc_licht_latch_anwenden() vor dem Licht-Kontext des Spielers, _zurueck() hinter der
 * Figuren-Schleife (Gegner/NPC) und vor den Props. */
#define RE15_PC_LICHT_VOR       0x4b0   /* @0x8001cebc: 1200 vor Leon (lokal +x = Blickrichtung) */
#define RE15_PC_LICHT_HOEHE     800     /* @0x8001d018: addiu -800                               */
#define RE15_PC_LICHT_R_MIN     0xd2    /* @0x8001cf28 */
#define RE15_PC_LICHT_G_MIN     0x8c    /* @0x8001cf64 */
#define RE15_PC_LICHT_B_MIN     0x50    /* @0x8001cfa0 */
#define RE15_PC_LICHT_HELL      0x1770  /* @0x8001d080 */
/** Reine Rechnung: Licht 2 des Satzes wie @0x8001cef8-0x8001d084 umstellen. */
void re15_pc_licht_latch_rechnen(re15_light_cut_t *cut, int32_t px, int32_t py, int32_t pz,
                                 int16_t rot_y);
/** Latch gesetzt (g_re15_licht_latch)? Dann Satz `cut` sichern und umstellen. 1 = umgestellt. */
int  re15_pc_licht_latch_anwenden(re15_light_set_t *ls, int cut, int32_t px, int32_t py, int32_t pz,
                                  int16_t rot_y);
/** Satz zurueckschreiben (falls umgestellt) und Latch := 0, wenn er stand (@0x8001d16c-b4). */
void re15_pc_licht_latch_zurueck(re15_light_set_t *ls);

/* ===== C4 — Ton-Weiche und Haken-Bindung ===================================================
 * RE1.5 FUN_80045024 (selbst disassembliert): Bank = a0 >> 24 (`srl v1,a0,24` @0x80045028),
 * Satz = (a0 >> 16) & 0xff (`srl v0,a0,16` / `andi s4,v0,0xff` @0x80045078-7c), Byte0 = Lage-Flag
 * (`andi a0,a0,0xff` @0x80045080), Byte1 wird im Kopf NICHT gelesen. Bank-Tor `sltiu v0,v1,0x6`
 * @0x80045094, Sprungtabelle @0x80010e70: [1] 0x800450d0 ARMS (0x801fcd00), [2] 0x800450e4 snd0,
 * [3] 0x800450f8 snd1, [4] 0x8004511c CORE (0x801fbd00), [5] 0x80045130 = snd0, [0] 0x801fdd00
 * (im Port nicht resident). Satz-Tor < 0x21 (`sltiu v0,s4,0x21` @0x800450bc/d0/e4/11c), snd1 < 0x19
 * (@0x800450f8). Bank-Wahl = re15_audio_se_bank_kind (include/re15_audio.h). Lage-Flag ohne Wirkung
 * wie bei allen Port-SEs (FUN_80045a64-Zweig nicht portiert, BAUPLAN O5). */
/** Satz der ESP-SE-Codes (Byte2) und Bankart (re15_se_bank_kind_t); Rueckgabe 0 = verworfen. */
int  re15_pc_esp_se_weiche(uint32_t code, int *satz);
/** RE2-FX-SE-Codes -> RE1.5-Zusatzbank (E9): 0x01130001 (Saeure, RE2 @0x80021678-7c) -> ARMS10
 *  Satz 10, 0x01120001 (Brand, RE2 @0x80020fd4/@0x80021028) -> ARMS11 Satz 10. RE2-Spieler
 *  FUN_8005ba28: Bank = a0 >> 24 (@0x8005ba30), Satz = (a0 >> 16) & 0xff (@0x8005ba7c-80),
 *  Satzwort = [0x800dbb78 + Bank*4] + Satz*4 (@0x8005ba8c-94). Die RE2-Saetze (ARMS0B Satz 19 /
 *  ARMS0A Satz 18 = `00 00 33 20`) und die RE1.5-Saetze (ARMS10/11.EDH @0x28 = `00 00 33 20`) zeigen
 *  auf dasselbe Tonattribut Prog 0 Ton 3 und dieselbe VAG 3 (VB md5 39cec979 / 46833b5e bytegleich,
 *  Saeure-GP §12). Rueckgabe 1 = bekannt (arms_id/satz gesetzt), 0 = unbekannt (stumm). */
int  re15_pc_re2fx_se_weiche(uint32_t code, int *arms_id, int *satz);
#define RE15_PC_RE2FX_SE_SAEURE 0x01130001u   /* RE2 `lui a0,0x113` / `ori a0,a0,0x1` @0x80021678-7c */
#define RE15_PC_RE2FX_SE_BRAND  0x01120001u   /* RE2 `lui a0,0x112` @0x80020fd4 / `ori a0,a0,0x1` @0x80021028 */
#define RE15_PC_ARMS_SAEURE     0x10          /* SOUND/ARMS10 = RE2 ARMS0B (Saeure)               */
#define RE15_PC_ARMS_BRAND      0x11          /* SOUND/ARMS11 = RE2 ARMS0A (Brand)                */
#define RE15_PC_ARMS_AUFSCHLAG_SATZ 10        /* ARMS10/11.EDH @0x28 = `00 00 33 20`               */
/** Gebunden an re15_esp_se_hook (FUN_80045024-Analogon der ESP-Routinen 29/31). */
void re15_pc_esp_se(uint32_t code, const int32_t pos[3]);
/** Gebunden an re2fx_se_hook (FUN_8005ba28-Analogon der RE2-FX-Maschine, Op 48/49). */
void re15_pc_re2fx_se(uint32_t code, const int32_t pos[3]);
/** Alle Runde-34-Haken binden: re15_esp_se_hook, re15_esp_aufschlag_hook = re2fx_aufschlag,
 *  re2fx_se_hook, re2fx_applier = re15_re2_gl_apply. Einmal beim Start (main.c). */
void re15_pc_r34_haken_binden(void);
/** Zusatzbank-SE (audio_pc.c): SOUND/ARMS<id>.EDH/.VB einmal laden (neben der Bank der
 *  ausgeruesteten Waffe) und Satz `satz` spielen. */
void re15_audio_arms_zusatz_se(int arms_id, int satz);

/* ===== Zeichenkamera des Effektpasses — fuer re2fx_pc_draw() (Spur D) ======================
 * Integration Runde 34 (W3): EIN Weg. main.c uebergibt die Ansicht DIESES Bilds direkt an
 * re2fx_pc_set_ansicht (re2fx_pc.h) — mit genau den Werten, die pc_draw_effects bekommt (cam_view,
 * Bildmitte, Regions-Viereck des angezeigten Cuts = DAT_800ac790-Analogon, camf = RDT-Kamera
 * +0x62 >> 7 des angezeigten Cuts wie FUN_800534c4 `srl fp,v0,7` @0x800532e4) — und macht sie nach
 * re2fx_pc_draw wieder ungueltig. Die fruehere Zwischenablage re15_pc_fx_kamera_* hatte keinen Leser
 * und ist entfallen. */

/* ===== C7 — RE2-Part-Farbwort +0x70 (O-VB3, V4) ============================================
 * O-VB3 GEKLAERT (RE2 PSX.EXE, selbst disassembliert): die Entity-Hauptschleife ruft fuer JEDE
 * Entity den generischen Part-Zeichner — `lw a1,408(s0)` (Part-Feld +0x198) @0x80026894,
 * `jal 0x80027160` @0x8002689c (Schleife 0x800267c0-0x80026930 = alle Gegnertypen, nicht nur
 * Zombies). Je Part: `jal 0x80027434` @0x8002738c/@0x800273d4 (a1 = Part-Record, Schritt 172),
 * darin `lw fp,112(s1)` @0x80027900 (= Part +0x70, das Farbwort), `addu a2,fp,zero` @0x80027ae0 ->
 * `jal 0x80027bec` @0x80027aec -> `sw a2,16(sp)` @0x80027c08 -> `lwc2 $6,0(v0)` (0xc8460000,
 * RGBC) @0x80027c2c -> NCCT: out = RGBC * (BK + LCM*LLM*N). Die Tinte SKALIERT also das
 * Beleuchtungsergebnis (neutral 0x808080, FUN_80028368.c:55) — fuer Hund, Spinne usw. genau wie
 * fuer den Zombie. main.c wendet sie fuer Zombies mit aktiver Gore-Bruecke schon an
 * (RE2_GORE_TINT, `re15_re2z_gore_resolve`); diese Funktion liefert die Tinte fuer die UEBRIGEN
 * RE2-KI-Aktoren (re15_ai_re2_for_type), deren Parts Spur B (V4) in re2z_part_tint[] schreibt.
 * re2_rig = 1: die gezeichnete Bank traegt das RE2-Skelett (reines RE2-EMD oder Hybrid) -> Bone i =
 * Part i; re2_rig = 0: RE1.5-Skelett (Rueckfall ohne RE2-Archiv) -> Bone = re2_hybrid_perm[Part]
 * (Integration W6, Gegenpruefung C H3). n = Bones der Bank, alle 20 Part-Woerter (re15_actor.h).
 * Ein Wort 0 = nie geseedet (RE2 seedet JEDEN Part mit 0x808080) -> neutral. Rueckgabe 1 =
 * mindestens ein Part nicht neutral. */
#include "re15_actor.h"
int re15_pc_re2_part_tint(const re15_actor_t *e, int n, int re2_rig, uint32_t *out_tint, int out_n);

#endif /* FX_PLATTFORM_PC_H */
