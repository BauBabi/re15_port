/*
 * re15_wassersprung_r41.h — Runde 41 Spur A "wasser20b0", Punkt 2: der Wassereffekt beim Sprung von der Kante
 * ins Wasser (Spieler-Landung) und die Wasser-Schritte der NPCs.
 *
 * Nutzer (AUFTRAG Runde 41): "In STAGE2 fehlt noch der Wassereffekt vom rein springen von der Kante in das
 * Wasser. Auch die NPCs teilen die gleichen Effekte wenn sie durch das Wasser gehen."
 * Dossier analysis/befunde_runde41/A_wasser20b0.md (Punkt 2).
 *
 * ORIGINAL (info/Re1.5/PSX.EXE, selbst disassembliert):
 *
 *  (a) LANDUNG der Kletter-/Sprung-Maschine, Phase 4 @0x800386d4 (climb_common.c case 4):
 *        +0x38 := +0x1ba (Bodenhoehe)                           `lh v0,442(v1)` / `sw v0,56(v1)` @0x80038724-2c
 *        v1 = Spieler+0x88 (Wasserstempel, s16)                 `lh v1,-13604(v1)` @0x8003873c (0x800acadc)
 *        a0 = Spieler+0x38 (y)                                  `lw a0,-13684(a0)` @0x80038744 (0x800aca8c)
 *        v1 = (Stempel < y)                                     `slt v1,v1,a0`    @0x8003874c
 *        beq v1,zero -> keine Partikel                          @0x80038758
 *        Schleife s1 = 4..1 (@0x8003875c `ori s1,zero,0x4`, @0x800387c4-d0 Zaehler), je Durchlauf ZWEI Spawns:
 *          a0 = 0x06033000 (`lui a0,0x603` / `ori a0,a0,0x3000` @0x8003876c-70) = Effekt 6, Sub 3, Skala 0x3000
 *          a1 = +0x6a (Gier) + ((s1-2) << 8)                   @0x8003877c-98 (s0 = (s1&0xff)-2 << 8)
 *          a2 = Spieler+0x20 (Entity-Matrix: T = Lage)         `addiu s3,a2,25` @0x80038760 (a2 = 0x800aca5b -> 0x800aca74)
 *          a3 = 0x8007428c = (0,0,0,0) (Nullversatz)           @0x80038764-68 (`read 0x8007428c 4 --w 4`: 0,0,0,0)
 *          jal 0x80019700                                      @0x80038794
 *          zweiter Spawn gleich, a1 += 2048                     `addiu a1,a1,2048` @0x800387c0, jal @0x800387bc
 *        => 8 Plaetze, Gier rot_y + {512, 2560, 256, 2304, 0, 2048, -256, 1792}, nur wenn der Boden unter
 *           dem Wasserspiegel liegt (Stempel < y; PSX y nach unten). Der Stempel des Vorbild-Scans steht in der
 *           Landung noch (Loeschung erst @0x80031da4 NACH dem Executor-jalr @0x80031cb4 in FUN_80031c44).
 *
 *  (b) NPC-SCHRITTE: die NPC-Geh-Subs 4/5 (FUN_800512xx / 0x800515xx) lesen das Bildwort *(+0x168)
 *        `lw v0,360(v0)` / `lw v1,0(v0)` / `andi v0,v1,0x4000` @0x80051278-88 (Sub 4) bzw. @0x800515e4-f4 (Sub 5)
 *        und rufen bei gesetztem Bit FUN_80045630(a0, 7 - 3*((Wort>>12)&1)): a0 = 0 (Sub 4 @0x80051290) bzw.
 *        a0 = 1 (Sub 5 @0x800515fc), jal @0x800512a8 / @0x80051614; Phase 2 @0x800512d4-308 / @0x800519dc-a10 /
 *        @0x80051bd4-c08 analog. FUN_80045630 fuehrt VOR der Typweiche denselben Effekt-Zweig wie beim Spieler
 *        (re15_wasser_r38.h: Bodentyp FUN_800437d4, Bit 0x80 -> 0x06050D00 an Part[fuss]+0x40 mit Gier +0x6a,
 *        sonst Stempel +0x88 != 0 -> 0x06032000 an Part[fuss-1]+0x40 mit Gier 0x800, Versatz (0,100,0)) und nimmt
 *        fuer Typ >= 0x40 (`sltiu v0,v0,0x40` @0x80045730) den NPC-Zweig: Tonindex = a0 + (Boden & 0x7f) - 8
 *        (`addiu s3,a0,-8` @0x8004575c), ohne das Objekt-Override (@0x80045770-90).
 *        Der Stempel +0x88 wird je Entity im Entity-Takt NACH dem Dispatch geloescht (`sh zero,136(v0)`
 *        @0x8001a59c hinter `jalr` @0x8001a588) und vom AUTO-Pass (FUN_800436a8, Pool 2 @0x80043720-28) neu
 *        gestempelt — der Schritt sieht den Stempel des Vorbilds, wie beim Spieler.
 *
 *  (c) NACHBESSERUNG 1 (M2): +0x168 zeigt auf das Bildwort der Bank, mit der der Sub anim_set (f314) ruft. Die
 *        NPC-Subs {4,5,9} und die Executor-Subs {2,6} laden das Paar +0x170/+0x174 (`lw a0,368(v0)` /
 *        `lw a1,372(v0)` vor `jal 0x8001f314` @0x800512bc-c0, @0x80051714-18, @0x80051884-88) = die OWN-Bank
 *        (dir[3]/dir[4], FUN_80022300 @0x800224b8/@0x800224c8; Port b->anim_own, re15_actor_uses_own_bank). Deren
 *        Geh-Clip 5 (30 Bilder) TRAEGT Aufsetzer: Typen 0x40/0x45/0x49 Bild 7 (Wort 0x6xxx -> Fuss 7) und 22
 *        (0x5xxx -> Fuss 4), Lauf-Clip 0 (22 Bilder) Bilder 0/12; Typen 0x42/0x4d Clip 5 Bilder 8 (0x4xxx) / 22
 *        (0x7xxx), Clip 0 Bilder 0/11 (beide Archive CDEMD0/CDEMD1 byte-gleich; Dossier Nachbesserung 1). Der
 *        Bau-Stand las die Container-Bank (dir[1], Clip 5 = 20-Bild-Kniebeuge ohne Marke) — daher kein Schritt.
 *        Nur Typ 0x4b (Wurzelhoehe -1250 = Kind) traegt in keiner Bank eine Marke: dort gilt die PORT-WAHL
 *        (NUTZER-VORGABE Nachtrag 3 / E1 "alle NPCs"): Aufsetzer = Bild, in dem der Fuss-Teil 4/7 nach der
 *        Schwungphase erstmals seine Tiefstlage erreicht (Pose-y im obersten Zehntel der Spanne) — an den
 *        markierten Typen belegt (0x40-Familie Clip 5: Marken 7/22, Regel 7/20; Clip 0: Marken 0/12, Regel 0/11).
 *        Die Rueckwaerts-Subs 7/8 laden das LOCO-Paar +0x84/+0x16c (`lw a0,132(v0)` / `lw a1,364(v0)` @0x80051a20-24 /
 *        @0x80051c18-1c), Clip 1 (+0x94 = 1 @0x8005196c).
 *
 *  (e) NACHBESSERUNG 1, dritte Fortsetzung 2026-10-24: Marken tragen in den ausgelieferten Baenken NUR die
 *        Fortbewegungs-Clips (Sonde npc_marken.txt, alle NPC-Typen 0x40/0x42/0x45/0x47/0x49/0x4b/0x4d, beide
 *        Archive): OWN c0 + c5, LOCO/Container c0 + c1. Die PORT-WAHL ersetzt nur FEHLENDE Marken eines solchen
 *        Clips: 0x4b (alle) und 0x47 aus CDEMD1 (OWN c5). Jeder andere Clip ohne Marke (INIT-Pose LOCO c2 @0x8011d978,
 *        Gesten, Kante/Schub) gibt wie im Original keinen Schritt. Die Ableitung liest den Keyframe direkt aus dem
 *        Bildwort - re15_compute_actor_kf schriebe je Bild eine RE15_ANIM_TRACE-Zeile (integration_r37_roy_leon1170).
 *
 *  (d) NACHBESSERUNG 1, Fortsetzung 2026-10-24: ALLE Aufrufe von FUN_80045630 aus den NPC-Subs (Tabelle @0x80076ca0;
 *        ghidra1_V2 XREF[24] von FUN_80045630): Sub 4 @0x800512a8 (Phase 1) / @0x80051304 (Phase 2), Sub 5 @0x80051614 /
 *        @0x80051670, Sub 7 @0x80051a0c, Sub 8 @0x80051c04, Sub 9 (Drehen) @0x80051e88, Sub 0x12/0x16 (Kante, Phase 1)
 *        @0x800525d4, Sub 0x13 (Schub, Phase 1) @0x80052930; a0 = 1 nur Sub 5 Phase 1 (@0x800515fc), sonst 0. Lage im Takt:
 *        Sub 4/5 Phase 2 testen VOR Kurs und Vorschub (@0x800512d4 vor @0x8005133c/@0x80051344, @0x80051640 vor
 *        @0x800516a8/@0x800516b0), Sub 7/8 NACH dem Vorschub (@0x800519c8 -> @0x800519dc, @0x80051bc0 -> @0x80051bd4),
 *        Sub 9 NACH dem Kurs (@0x80051e44 -> @0x80051e58), Kante/Schub direkt vor anim_set (@0x800525f0 / @0x8005294c).
 *        Gelesen werden *(+0x168) und Part[fuss] (+0x188 + fuss*0xac @0x8004567c-a8) des GEZEICHNETEN Bilds, auch wenn
 *        der Sub im selben Takt schon +0x94/+0x95 neu gesetzt, gedreht oder geschoben hat -> Port: Schnappschuss je
 *        Aktor am Bildanfang (wassersprung_r41.c s_gez). Die STAGE-Overlays rufen FUN_80045630 nur aus Typ-Handlern
 *        (STAGE1 @0x80111da4/@0x8011c4f0, STAGE3 @0x80110884/@0x80116180, STAGE4 @0x80111820/@0x801162dc/@0x801166b4,
 *        STAGE5 @0x801119a0/@0x8011645c/@0x80116834; STAGE2 keinen) - nicht Teil der NPC-Geh-Subs.
 *
 *  (f) NACHBESSERUNG 2 (2026-10-24), M1 - SCHRITT-TON DER NPCs: FUN_80045630 bildet a0 + (Boden & 0x7f) (`andi v0,s0,0x7f` /
 *        `addu a0,s3,v0` @0x80045718-24) und zieht fuer Typ >= 0x40 acht ab (`sltiu v0,v0,0x40` @0x80045730, `addiu s3,a0,-8`
 *        @0x8004575c). Hoerbar ist ein Satz nur mit Stimme (e[3] & 0x1f) - 16 + (fuss != 4) in 0..7 (`lbu v0,3(a2)` @0x800457a0,
 *        `andi s1,v0,0x1f` @0x800457a8, `addiu s1,s1,-16` @0x800457ac, `xori v0,v0,0x4` @0x800457b4; FUN_800458d4 pumpt 8 Saetze
 *        @0x800b2420: `ori s4,zero,0x8` @0x800458dc, `addiu s1,s1,-18` @0x8004590c, `bne s4,zero` @0x800459e4) und mit einem Ton,
 *        dessen VAG != 0 ist (SsUtKeyOnV `lhu a0,22(v0)` @0x80059f2c, `bne a0,zero` @0x80059f8c). Die Beta-Raeume fuellen die
 *        NPC-Zeilen fast nie (52 NPC-Raeume ohne Stempel-Zweig: 15 von 280 Faellen hoerbar, Dossier N2.1). BETA -> RETAIL: das
 *        RE2-Gegenstueck FUN_8005c040 (RE2 Leon) indiziert JEDEN Typ mit a0 + (Boden & 0x7f) - ohne Typ-Versatz (`andi v1,t0,0x7f`
 *        @0x8005c44c, `addu s5,s5,v1` @0x8005c458; der Typ waehlt nur die Stimme, `sltiu v0,v0,0x10` @0x8005c0ec) - und spielt
 *        einen leeren Satz nicht (`beq v0,v1` @0x8005c470). Port: Original-Zeile, wenn sie im Original hoerbar ist, sonst die
 *        Retail-Zeile; ist auch die leer, kein Ton.
 *     M2 - LAGE (Entscheid A-E4): FUN_80045630 ruft FUN_80045a64(Entity+0x34) (`jal 0x80045a64` / `addiu a0,a0,52`
 *        @0x80045830-34): Kamera = *(RDT+0x24) + DAT_800b0fe4*0x20 (gezeigter Cut), Abstand /250 (`0x10624dd3` @0x80045b3c-4c,
 *        Deckel 0x7f @0x80045b50-5c), Seite ueber FUN_80045d6c; DAT_800b2824 = (Bit 0x800 ? 0x89 : PAN) (`sh a0` @0x80045ce4),
 *        DAT_800b2826 = (Bit 0x800 ? PAN : 0x89) (`sh a1` @0x80045cd4), beide minus ATT[Abstand], `& 0x7f`. FUN_800458d4 reicht
 *        sie als voll (sp+24, +0x0e @0x8004598c-9c) / volr (sp+28, +0x10 @0x800459a0-b8) an SsUtKeyOnV @0x80059d3c: gleich ->
 *        Pan 0x40 (@0x80059e10), volr < voll -> Pan (volr<<6)/voll (@0x80059e30-6c), sonst 0x7f - (voll<<6)/volr
 *        (@0x80059e78-bc); Tastenpegel = der groessere (@0x80059e20 / @0x80059e64 / @0x80059eb0). SpuVmKeyOnNow @0x80055d10:
 *        Pegel * Master * Prog * Ton (@0x80055d24-80), Tasten-Pan < 0x40 -> rechts * Pan / 63 (@0x80055f58-88), sonst links *
 *        (0x7f - Pan) / 63 (@0x80055f8c-...). Port: eigene byte-genaue Nachbildung von FUN_80045a64 in wassersprung_r41.c
 *        mit dem Winkel des Originals: FUN_80045d6c ruft catan @0x800658fc = CORDIC (12 Stufen, Winkel @0x80078c90 =
 *        511,302,159,81,41,20,10,5,3,1,0,0). rotor_common.c (Rotor) rechnet den Winkel mit einer ATAN-Tabelle (Abweichung bis
 *        +-7/4096 = eine Pan-Stufe, gemessen A_nb2/cordic_vgl.py) - dort nicht geaendert (OFFEN, Dossier N2.7). Pegel im
 *        linearen Mischer-Modell des Ports (wie se_play_layers, ohne SPU-Quadrat).
 */
#ifndef RE15_WASSERSPRUNG_R41_H
#define RE15_WASSERSPRUNG_R41_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_WASSERSPRUNG_R41_CODE      0x06033000u   /* `lui a0,0x603` / `ori a0,a0,0x3000` @0x8003876c-70 */
#define RE15_WASSERSPRUNG_R41_PLAETZE   8             /* s1 = 4..1 x 2 Spawns @0x8003875c / @0x800387c4-d0   */
#define RE15_WASSERSPRUNG_R41_NPC_TON   8             /* `addiu s3,a0,-8` @0x8004575c                          */
#define RE15_WASSERSPRUNG_R41_STIMME_BASIS 16         /* `addiu s1,s1,-16` @0x800457ac (Stimme = (e[3]&0x1f)-16+(fuss!=4)) */
#define RE15_WASSERSPRUNG_R41_STIMMEN   8             /* 8 Saetze @0x800b2420: `ori s4,zero,0x8` @0x800458dc        */
#define RE15_WASSERSPRUNG_R41_LAGE_MITTE 0x40         /* Pan bei voll == volr `ori v0,zero,0x40` @0x80059e10         */
#define RE15_WASSERSPRUNG_R41_PAN_TEILER 0x3f         /* /63: `lui 0x410` / `ori 0x4105` @0x80055f68-6c             */

/* (a) Landung: Spieler p auf Bodenhoehe (p->y = +0x1ba), stempel = Spieler+0x88 des Vorbild-Scans, t_y = y der
 * Entity-Matrix +0x20 (zuletzt gezeichnete Lage, vor dem Bodenschreiben @0x8003872c). Spawnt die 8 Plaetze bei
 * (p->x, t_y, p->z), wenn stempel < p->y. Liefert die Zahl der Spawn-Aufrufe (0 oder 8). */
int re15_wassersprung_r41_landung(const re15_actor_t *p, int16_t stempel, int32_t t_y);

/* (b) NPC-Schritt: FUN_80045630(a0, fuss) fuer einen Aktor vom Typ >= 0x40 (Effekt-Zweig + Ton -8).
 * rdt = Raum (Bodentyp), fuss = 7/4 (Part-Index). Liefert den Tonindex (oder -1 ohne Ton). */
int re15_wassersprung_r41_npc_schritt(re15_actor_t *e, int a0, int fuss, const void *rdt);

/* Haken in den NPC-Subs (Lagen s. Kopf (d)): Bildwort-Test + Schritt wie @0x80051278-a8. Liefert 1, wenn ein
 * Schritt lag. Liest Bank/Clip/Bild/Lage des am Bildanfang festgehaltenen GEZEICHNETEN Bilds; ohne Bildanfang
 * (Riegel, die den Sub direkt rufen) den aktuellen Zustand - dann VOR dem Bildvorschub dieses Takts rufen. */
int re15_wassersprung_r41_npc_takt(re15_actor_t *e, int a0);

/* Die Bank des Kanals, den der laufende Sub des Aktors spielt (Own / Loco / Container, s. Kopf (c)); liefert
 * re15_emd_animation_t* (als void*) und optional das Skelett. NULL ohne geladene Bank. */
const void *re15_wassersprung_r41_npc_bank(const re15_actor_t *e, const void **out_skel);

/* Aufsetzer-Bilder eines Clips: aus den Marken (Bit 0x4000; Fuss 7 - 3*((Wort>>12)&1)) oder - nur wenn der Clip
 * KEINE Marke traegt UND ein Fortbewegungs-Clip ist (Kopf (e)) - aus der PORT-WAHL-Regel (Kopf (c)); *portwahl_out = 1
 * im zweiten Fall. Liefert die Zahl (0 fuer jeden anderen Clip ohne Marke). */
int re15_wassersprung_r41_aufsetzer(const void *anim, const void *skel, uint8_t type, int clip,
                                    int bilder[4], int fuesse[4], int *portwahl_out);
/* Nur fuer Riegel: die PORT-WAHL-Regel UNABHAENGIG von den Marken auf einen Clip anwenden (Regelpruefung). */
int re15_wassersprung_r41_regel(const void *anim, const void *skel, uint8_t type, int clip, int bilder[4], int fuesse[4]);

/* Je Bild (Bildanfang, vor allen KI-Takten): Zustand des gezeichneten Bilds je NPC festhalten (Kopf (d)) und die
 * Folge-Plaetze der NPC-Schritte nachfuehren (Flags-Bit 0x04 @0x80019f54-a4). */
void re15_wassersprung_r41_bildanfang(void);

typedef struct {
    unsigned landungen;        /* Landungen mit Spawns                      */
    unsigned landung_trocken;  /* Landungen ohne (Stempel >= y)             */
    unsigned npc_schritte;     /* NPC-Schritte gesamt                       */
    unsigned npc_boden;        /* davon Zweig Bit 0x80 (0x06050D00)         */
    unsigned npc_stempel;      /* davon Zweig Stempel (0x06032000)          */
    unsigned plaetze;          /* gespawnte Plaetze gesamt                  */
    int32_t  letzter_anker[3];
    int16_t  letzte_gier[RE15_WASSERSPRUNG_R41_PLAETZE];
    int      letzter_ton;
    unsigned portwahl_schritte;  /* Schritte aus der PORT-WAHL-Regel (Clips ohne Marke, Typ 0x4b) */
    unsigned letzter_portwahl;   /* 1 = der letzte Schritt kam aus der Regel                    */
    uint8_t  letzter_typ, letzter_clip, letztes_bild, letzter_fuss;
    /* Nachbesserung 2 (M1/M2): letzter_ton = GESPIELTE Zeile (-1 = kein Ton), letzter_ton_orig = Zeile des Originals (-8)  */
    int      letzter_ton_orig;
    uint8_t  letzte_quelle;      /* 0 = Original hoerbar, 1 = RE2-Retail-Zeile (FUN_8005c040), 2 = stumm (beide leer)     */
    unsigned npc_ton_orig, npc_ton_retail, npc_ton_stumm;
    int      letzte_lage[2];     /* DAT_800b2824 / DAT_800b2826 (FUN_80045a64), -1 ohne Kamera                         */
    int      letzter_abstand;    /* Abstand/250 (0..127)                                                             */
    int      letzter_pegel[2];   /* Port-Pegel L/R (Q15) des zuletzt GESPIELTEN NPC-Schritts (re15_wassersprung_r41_lage_pegel) */
} re15_wassersprung_r41_mess_t;
const re15_wassersprung_r41_mess_t *re15_wassersprung_r41_mess(void);

/* Nachbesserung 2, M2 (Kopf (f)): FUN_80045a64 fuer die Lage pos (Entity+0x34) mit dem gezeigten Cut g_scd.work_vars[0x0A]
 * (= DAT_800b0fe4) des Raums rdt. lage[0] = DAT_800b2824 (voll), lage[1] = DAT_800b2826 (volr), *abstand = Abstand/250 (0..127).
 * Liefert 0 ohne Kamera (kein RDT / Cut ausserhalb). */
int re15_wassersprung_r41_lage(const void *rdt, const int32_t pos[3], int lage[2], int *abstand);
/* Nur fuer Riegel: die PAN/ATT-Bytes (256, @0x80074728..0x800747a8+0x80), mit denen re15_wassersprung_r41_lage rechnet. */
const uint8_t *re15_wassersprung_r41_lage_tabelle(void);
/* Fuer den Schritt-Ton (audio_pc.c re15_audio_footstep, Haken Runde 41 Spur A): die Lage des GERADE gespielten NPC-Schritts
 * abholen (1 = vorhanden; danach geloescht). Spieler-Schritte haben keine (OFFEN, Dossier N2.2). */
int re15_wassersprung_r41_lage_nehmen(int lage[2]);
/* Pegel nach SsUtKeyOnV/SpuVmKeyOnNow (Kopf (f)): *vol = Ton-Pegel des Ports bei Tastenpegel 0x7f (Q15) -> links; Rueckgabe
 * rechts (>= 1, 0 hiesse im Mischer "Mono"). diag != 0: Messzeile "[foot] NPC-Lage ..." auf stderr. */
int re15_wassersprung_r41_lage_pegel(int *vol, const int lage[2], int diag);
void re15_wassersprung_r41_reset(void);

#endif /* RE15_WASSERSPRUNG_R41_H */
