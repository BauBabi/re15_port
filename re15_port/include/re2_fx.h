/**
 * @file re2_fx.h
 * @brief Runde 34 VERTRAG V3 (C0, BAUPLAN §3.0): die RE2-FX-MASCHINE (RE2-Retail-Effektpool) —
 *        Schnittstelle fuer den Saeure-/Brand-Aufschlag der Granaten 0x0A/0x0B (E8) und das
 *        Bodenfeuer (Op 48 + Bodenflammen).
 *
 * STAND Spur D (analysis/befunde_runde34_granaten/bau_d.md): Bank-Registrierung, Spawner, Pumpe,
 * RNG-Strom und die Ops 0/1/2/19/25/27/28/29/30/40/46/48/49/50/58/64 sind in engine/src/re2_fx.c
 * umgesetzt; der PC-Zeichner liegt in platform/pc/src/re2fx_pc.c. Die Vertrags-Deklarationen
 * unten (C0) stehen woertlich; alles Weitere ist ADDITIV darunter.
 *
 * Alle Adressen = RE2-PSX.EXE (info/re2leon/PSX.EXE), sofern nicht anders genannt.
 *   Pool       0x800D8CF0, Schritt 0x7C, 96 Plaetze (Spawner FUN_8001cbe8 `addiu t2,zero,96`)
 *   Spawner    FUN_8001BF10 / FUN_8001cbe8: a0 = Bank<<24 | Sub<<16 | Skala, Sub&7 = Skript,
 *              Sub>>3 = CLUT-Zeile (@0x8001bf1c-c0)
 *   Pumpe      FUN_8001d300 (einziger Aufrufer `jal 0x8001d300` @0x80026980, NACH der
 *              Gegner-Schleife 0x800267c0-0x80026930)
 *   Op-Tabelle @0x8009D868 (`lw v0,-10136(at)` + `jalr`), u.a. [40] 0x80020758, [48] 0x80020F3C,
 *              [49] 0x800215C8
 */
#ifndef RE2_FX_H
#define RE2_FX_H

#include <stdint.h>
#include <stddef.h>

/** Bank-Registrierung der RE2-CORE00.ESP = FUN_8001bca0 (Skripttabelle = Kopf + (2*ca + cb + 2)*4:
 *  `srl a0,v0,16` / `andi v0,v0,0xffff` / `sll v0,v0,1` / `addu` / `addiu v0,v0,2` / `sll v0,v0,2`
 *  @0x8001bd08-1c). raw/size = shared_assets/RE2/CORE00.ESP (8572 B, Ids `03 05 00 01 02 06 07 04`),
 *  vom Aufrufer gehalten (nicht kopiert). Rueckgabe 0 = registriert, < 0 = Fehler.
 *  (C0-Stub lieferte -1; seit Spur D umgesetzt.) */
int  re2fx_register_core(const uint8_t *raw, size_t size);

/** Alle Plaetze frei (Raumwechsel / neues Spiel / Tests). (C0-Stub; seit Spur D umgesetzt.) */
void re2fx_reset(void);

/** Aufschlag-Einstieg (E8, Port-Zuordnung der Uebergabe Granatenplatz -> RE2-FX-Platz): spawnt den
 *  Platz, dessen Op B 48 (Brand, re2_art 1, @0x80020F3C) bzw. 49 (Saeure, re2_art 2, @0x800215C8)
 *  seine Phase 0 im Aufschlagbild laeuft. re2_art = RE2-Art-Byte +0x1B (Id - 9 @0x8001f1a8-b8;
 *  Aufschlag-Op = 47 + Art); q = Granaten-Weltlage (re15_esp_fx_t.wpos), gier = Granaten-Gier
 *  (slot+0x2e). Gebunden an re15_esp_aufschlag_hook (include/re15_esp.h). (C0-Stub; seit Spur D umgesetzt.) */
void re2fx_aufschlag(int re2_art, const int32_t q[3], int16_t gier);

/** Ein Spielbild der Pumpe FUN_8001d300 (Update-Pass Op A, Draw-Pass mit 0x4000-Befoerderung,
 *  FUN_8001d68c: Weltlage FUN_8001d894, Op B, Physik, Anim). Im Port direkt hinter dem
 *  RE1.5-ESP-Tick (Spur C1). (C0-Stub; seit Spur D umgesetzt.) */
void re2fx_tick(void);

/** Schadens-Applier des Bodenfeuers (Op 40 @0x80020758 -> `jal 0x800470c0` @0x800207bc).
 *  Signatur = re15_re2_gl_apply (include/re15_damage.h, V2b): p = &Pruefpunkt (s32, y - 100),
 *  gier = Platz +0x22, box = Kopie {-600,0,300,150} von @0x80010910, hitcode 0x2002000A.
 *  Rueckgabe != 0 = Treffer -> Op 50 (@0x800207c4-cc). Vorgabe NULL (= kein Schaden); die
 *  Plattform bindet ihn an re15_re2_gl_apply, sobald Spur B gemergt ist. */
extern int  (*re2fx_applier)(const int32_t p[3], int16_t gier, const int16_t box[4], uint32_t hitcode);

/** SE-Haken der RE2-FX-Maschine = FUN_8005ba28-Analogon (a0 = code, a1 = Lagezeiger):
 *  Op 49 0x01130001 (`lui a0,0x113` / `ori a0,a0,0x1` @0x80021678-7c, `jal 0x8005ba28` @0x800216ac,
 *  a1 = sp+16), Op 48 0x01120001 (`jal 0x8005ba28` @0x8002102c, a1 = Platz+0x60). Die Toene liegen
 *  in RE1.5 selbst: ARMS10 / ARMS11 Satz 10 (E9). Vorgabe NULL (= stumm); Bindung Plattform. */
extern void (*re2fx_se_hook)(uint32_t code, const int32_t pos[3]);

/* =====================================================================================
 * ADDITIV (Spur D) — Platz-Zugriff fuer den Zeichner, Spawner, Mess- und Sondenhaken.
 * ===================================================================================== */

/** Pool-Masse: 96 Plaetze (`addiu t2,zero,96` @0x8001cbe8 / @0x8001bf10), Schritt 0x7C (124,
 *  `addiu t0,t0,-124` @0x8001cc44; Pumpe `addiu v0,v0,124` @0x8001d4f4). */
#define RE2FX_PLAETZE      96
#define RE2FX_PLATZ_BYTES  0x7C

/** Byte-Abbild des RE2-Platzes i (0x800D8CF0 + i*0x7C), Offsets wie im Original:
 *  +0x00 Op A, +0x01 Op B, +0x02/+0x03 step[2]/[3], +0x04/+0x06 u16 Aspekt X/Y,
 *  +0x08..+0x0A s8 Beschleunigung, +0x0B Lebensdauer/Step-Delta, +0x0C/+0x0E/+0x10 s16 Geschw.,
 *  +0x12 u16 Step-Status/Phase, +0x14 u16 TPage-OR (Flammen: Boden als u32 +0x14..+0x17),
 *  +0x16 u16 Zufallsbreite/Zaehler, +0x18 u16 STATUS, +0x1A Eltern-Platz, +0x1B Art/Zustand,
 *  +0x1C Bank, +0x1D Besitzer, +0x1E Sub, +0x1F Step-Index, +0x20 Anim-Countdown, +0x21 Anim-Index,
 *  +0x22 s16 Gier, +0x24/26/28 s16 lokal, +0x2A u16 TPage, +0x2C/2E/30 s16 Versatz, +0x32 u16 CLUT,
 *  +0x34/36/38 s16 Weltlage, +0x3A u16 Skala, +0x3C/3E/40 Vorbild-Lage, +0x42 s16 Zaehler,
 *  +0x44/+0x48 Zusatz (+0x4A s16 Schadensrecht der Flammen), +0x4C..+0x6B MATRIX (m[3][3] s16,
 *  t[3] s32 ab +0x60). Die Zeiger +0x70 (Anim-Tabelle) / +0x74 (UV-Tabelle) / +0x78 (Step)
 *  stehen im Port als DATEI-OFFSETS (u32 LE) in die registrierte CORE00.ESP (re2fx_esp_daten).
 *  +0x6C (Matrix-Zeiger) steht NICHT im Abbild (Port-Zeiger, nur fuer Status 0x800 gelesen).
 *  NULL fuer i ausserhalb 0..95. */
const uint8_t *re2fx_platz(int i);

/** NUR SONDEN: beschreibbarer Zugriff auf das Platz-Abbild (Zustand fuer gezielte Taktproben setzen). */
uint8_t *re2fx_platz_sonde(int i);

/** Die registrierte CORE00.ESP (NULL vor re2fx_register_core). */
const uint8_t *re2fx_esp_daten(size_t *groesse);

/** FUN_8001cbe8 (aufgeschobener Spawn, Status 0x4000 @0x8001cc80-84) bzw. FUN_8001bf10 (sofort
 *  lebendig, Status 0xA003 @0x8001bfa8-ac). a0 = Bank<<24 | Sub<<16 | Skala, a1 = Gier (+0x22),
 *  mtx = 32-Byte-MATRIX-Abbild (wird nach +0x4C kopiert und als +0x6C-Zeiger gehalten), ofs = 4 x s16
 *  (SVECTOR, nach +0x2C..+0x33) oder NULL. Rueckgabe Platz 0..95, 0xFF = Pool voll (@0x8001cc70),
 *  0xFE = voll beim Mehrteil-Spawn (@0x8001ce10), -1 = Bank nicht registriert (Port-Schutz). */
int re2fx_spawn(uint32_t a0, int16_t a1, const uint8_t mtx[32], const int16_t ofs[4]);
int re2fx_spawn_sofort(uint32_t a0, int16_t a1, const uint8_t mtx[32], const int16_t ofs[4]);

/** Die Einheitsmatrix 0x8009DB44 (`00 10 00 00 00 00 00 00 00 10 …`) als 32-Byte-Abbild. */
extern const uint8_t re2fx_einheitsmatrix[32];

/** O-VB1: die GL-Waffenknochen-Basis B (Q12, zeilenweise m[r][c]), mit der der Aufschlag-Platz seine
 *  Matrix M.rot = RotY(gier) * B bildet (Herleitung bau_d.md §1.4). Fuer Sonden. */
void re2fx_gl_basis(int16_t out[9]);

/** Messhaken O-VB2 (nur Sonden): ersetzt die Port-Abbildung von FUN_8004fba0 (Rueckgabe = Boden,
 *  *kontakt = DAT_800DCBC8 != 0). NULL = Abbildung auf die RE1.5-Raumdaten (bau_d.md §2.4). */
extern int32_t (*re2fx_boden_hook)(const int32_t p[3], int r, uint32_t mask, int a3, int *kontakt);
/** Messhaken Wasser (nur Sonden): ersetzt re15_aot_water_at (FUN_800527b4-Zwilling). NULL = echt. */
extern int32_t (*re2fx_wasser_hook)(int32_t x, int32_t z);

/** NUR SONDEN (Nachbesserung N1/N2): ruft die Port-Abbildung von FUN_8004fba0 direkt (bei gesetztem
 *  re2fx_boden_hook den Haken). p = Pruefpunkt (s32 x/y/z), r = Rand (Flammen 2), mask/a3 wie das Original
 *  (a3 != 0 = Objekt-Schleife aus). Rueckgabe = DAT_800C3B7C, *kontakt = DAT_800DCBC8 (Bit 1 = Grundebene
 *  oder P in Form/Objekt, Bit 2 = P ueber der Unterkante einer beruehrten Form). Abbildung: bau_d.md §N1. */
int32_t re2fx_boden_sonde(const int32_t p[3], int r, uint32_t mask, int a3, int *kontakt);

/** Bezugsebene des Bodentests (Grundebene statt RE2-y 0) — PORT-ZUORDNUNG fuer RE1.5-Raeume mit Boden
 *  != 0, gesetzt von der Granate (Routine 31) unmittelbar vor re2fx_aufschlag; re2fx_reset -> 0. */
void re2fx_boden_basis_setzen(int32_t y);

/** Diagnose: Zahl der Aufrufe eines nicht umgesetzten Ops (0 = alle erreichten Ops umgesetzt). */
unsigned re2fx_op_unbekannt(void);
/** Diagnose: Zahl der Op-Aufrufe je Op-Nummer seit re2fx_reset (Sonden). */
unsigned re2fx_op_zaehler(int op);


/* ---- Billboards (FUN_80077924 Schleife + FUN_80077ed0 Paketbau) ---------------------------------
 * Plattformneutral: rechnet je sichtbarem Platz die POLY_FT4-Quads (Bildschirm-Rechteck, UV, CLUT,
 * TPage, Code) wie das Original; der PC-Zeichner (platform/pc/src/re2fx_pc.c) und die Offscreen-
 * Sonde setzen sie nur noch um. */
#include "re15_camera.h"
typedef struct {
    int      platz;               /* Pool-Index (Schleife 95 -> 0, @0x800779e8)              */
    int16_t  x0, y0, x1, y1;      /* Paket-Worte 2/4/6/8 (@0x800780f0-80078100)             */
    uint8_t  u0, v0, u1, v1;      /* Worte 3/5/7/9 (@0x8007813c-4c)                          */
    uint16_t clut, tpage;         /* +0x32 / +0x2A (`lhu v0,50` / `lhu v1,42` @0x80077f38-3c) */
    uint8_t  code;                /* 0x2C, 0x2E bei Status 0x1000 (@0x80077a44-50)            */
    int32_t  sz;                  /* SZ3 nach Klemme 0x7FFF (@0x80077f64-74)                  */
    int32_t  vz;                  /* View-Z (Sortierschluessel des Ports wie pc_draw_effects) */
} re2fx_quad_t;

/** Alle Quads des aktuellen Pools fuer eine Kamera. cam/cx/cy = Projektion wie pc_draw_effects
 *  (byte-true RTPS), camf = u16 Kamera-Satz +2 >> 7 (RE2: `lhu v0,102(v1) / srl v0,v0,7` @0x800779ac-bc,
 *  RE1.5-Raum: pc_fx_camf()), Region = FUN_8002c820-Test (@0x80077a30) auf das Viereck des aktiven
 *  Cuts (has_region 0 = nicht cullen). Rueckgabe = Zahl der Quads (hoechstens max). */
int re2fx_quads(const re15_camera_view_t *cam, int cx, int cy, int camf,
                int has_region, const int16_t rxs[4], const int16_t rzs[4],
                re2fx_quad_t *out, int max);

#endif /* RE2_FX_H */
