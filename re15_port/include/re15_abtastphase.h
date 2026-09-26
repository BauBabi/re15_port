/* re15_abtastphase.h — die ABTASTPHASE der texturierten Dreiecke im PC-Zeichner.
 *
 * ====================================================================================
 * WORUM ES GEHT (Nutzer-Befund, zweimal gemeldet)
 * ====================================================================================
 * "Der Cursor hat einen schraeg versetzten Schatten" (ROOM11F0, Cut 10, das
 * Boiler-Room-Raetsel).
 *
 * ERSTE ANTWORT (2026-09-26, v0.8.13) WAR FALSCH. Dort wurde hier -0.5 eingetragen,
 * begruendet mit psx-spx graphicsprocessingunitgpu.md:303-305 ("Vertex & Texcoord
 * specify the upper-left edge") und belegt nur durch eine Zaehlung heller Zeilen in
 * unserem EIGENEN Bild. Es gab keine PSX-Gegenmessung. Der Nutzer sah den Schatten
 * weiterhin. Die Gegenmessung (unten) zeigt: -0.5 ist SCHLECHTER als gar keine
 * Verschiebung und verschiebt zusaetzlich den ganzen Cursor um 1 Pixel nach links oben.
 *
 * ====================================================================================
 * 1. DER SCHATTEN IST ORIGINAL-KUNST — er steckt in der TEXTUR
 * ====================================================================================
 * Cursor-TIM: ROOM11F0.RDT @Datei 0x018DAC (magic 0x10, flags 0x09 = 8bpp + CLUT,
 * CLUT 256x1 @VRAM(0,480), Pixel 64 Words x 256 = 128x256). Selbst ausgelesen:
 *   Index 1 = 0x0160 (  0, 88,  0)  dunkelgruen  — Schatten des Rahmens
 *   Index 2 = 0x00E8 ( 64, 56,  0)  dunkeloliv   — Schatten des Kreuzes
 *   Index 3 = 0x03E1 (  8,248,  0)  hellgruen    — Rahmen
 *   Index 4 = 0x035B (216,208,  0)  gelb         — Kreuz
 *   Index 5 = 0x47FE (240,248,136)  hellgelb     — Kreuz-Kern
 * Senkrechter Kreuzarm: hell auf u 59..63, Schatten auf u 64..68 (= +5u).
 * Waagerechter Kreuzarm: hell auf v 38..40, Schatten auf v 41..44 (= +3v).
 * Der Schlagschatten ist also nach RECHTS UND UNTEN gebacken — genau der "schraege
 * Versatz". Er gehoert ins Bild und wird NICHT entfernt.
 *
 * ====================================================================================
 * 2. DIE GRUNDWAHRHEIT — ein echter PSX-Lauf, nicht unser eigenes Bild
 * ====================================================================================
 * Genau die fehlte bisher. DuckStation, MZD-Disc, Debug-Menue -> JUMP 0x11F, dann zu
 * Fuss ans Bedienfeld (AOT slot 1, sce=3, ROOM11F0.RDT @Datei 0x00D64) und QUADRAT,
 * bis sub16 @0x015A2 durch ist und `Cut_chg 0x0A` @0x015C0 greift. Belegt im
 * Savestate: aktiver Cut (work_vars[0x0A] = DAT_800B0FE4) = 10, RVD-Abschaltbit
 * DAT_800ACA3C = 0x100, residente RDT = ROOM11F0.
 * Abzug: analysis/grundwahrheit/psx_room11f0_cut10.png (320x240, aus dem
 * Savestate-VRAM; Anzeige-Ursprung x=443/y=0, per Hintergrund-Korrelation gegen
 * unseren eigenen Cut-10-Abzug bestimmt, mittlere Abweichung 2.69).
 * Savestate: analysis/grundwahrheit/psx_room11f0_cut10.sav
 *
 * WAS DIE PSX ZEIGT (Rohwerte aus dem Framebuffer, Prim-Modulation x1.111 gemessen:
 * CLUT (216,208,0) -> gerendert (240,232,0), CLUT (64,56,0) -> (64..72,56,0)):
 *   senkrechter Kreuzarm  HELL  auf Spalte x=160, SCHATTEN auf x=161
 *   waagerechter Kreuzarm HELL  auf Zeile  y=118, SCHATTEN auf y=119
 * Die PSX zeichnet den schraeg versetzten Schatten also SELBST. Die Frage war nie
 * "wie weg", sondern "warum sieht er bei uns anders aus" — und die Antwort ist:
 * unser Kreuz war UEBERHAUPT NICHT HELL, es bestand nur aus der Schattenfarbe.
 *
 * ====================================================================================
 * 3. DIE MESSUNG — Phasen-Fahrt gegen die Grundwahrheit
 * ====================================================================================
 * Zwei unabhaengige Masse, beide ueber RE15_FRAMEDUMP (echter Renderpfad):
 *   (A) Cursor-Kasten x142..176 / y104..134 = 1085 Zellen, jede gegen die 6
 *       CLUT-Farben klassifiziert -> "Zellen gleich".
 *   (B) das GANZE Raetselbrett x60..235 / y60..195 = 23936 Pixel (Cursor + 10
 *       Hebel-Props, anderes TIM, andere Geometrie) -> "Pixel mit Abweichung >16".
 *
 *   Phase   (A) Zellen gleich   (B) abweichende Pixel
 *   -1.000     853 / 1085            1527
 *   -0.500     887 / 1085            1335   <- v0.8.13, SCHLECHTER als 0
 *   -0.250     999 / 1085             975
 *    0.000    1033 / 1085             785   <- Stand VOR v0.8.13
 *   +0.125    1044 / 1085             711
 *   +0.1875   1055 / 1085             700
 *   +0.250    1072 / 1085             670
 *   +0.3125   1073 / 1085             654
 *   +0.375    1085 / 1085             634   <- PIXELGLEICH mit der PSX
 *   +0.4375   1058 / 1085             649
 *   +0.500    1058 / 1085             640
 *   +0.625     961 / 1085             861
 *   +0.750     938 / 1085             938
 * Beide Masse haben ihr Minimum bei +0.375, und dort ist der Cursor-Kasten
 * ZELLE FUER ZELLE identisch mit dem PSX-Bild (1085/1085).
 *
 * ====================================================================================
 * 4. WARUM UEBERHAUPT EINE PHASE — und was +0.375 bedeutet
 * ====================================================================================
 * SDL_RenderGeometry tastet die Textur an der PIXELMITTE ab: fuer Pixel p liegt der
 * Abtastpunkt in Scheitelkoordinaten bei p + 0.5. Verschiebt man die Scheitel um +a,
 * liegt er bei p + 0.5 - a. Aus den PSX-Pixeln laesst sich der Abtastpunkt der PSX
 * eingrenzen (Quadspalte x159->x162 mit u55->u73, also du/dx = 6; Quadzeile
 * y117->y120 mit v34->v46, also dv/dy = 4; Geometrie aus RE15_TRILOG des echten Laufs):
 *   x=160 hell (u in 59..63) und x=161 Schatten (u in 64..68)  ->  Phase in [-1/3, +1/6]
 *   y=118 hell (v in 38..40) und y=119 Schatten (v in 41..44)  ->  Phase in [ 0,   +1/2]
 * Schnittmenge [0, +1/6] fuer den Abtastpunkt, also a = 0.5 - [0,1/6] = [1/3, 1/2].
 * +0.375 = 3/8 liegt in diesem Intervall und ist der gemessene Bestwert beider Masse.
 *
 * ⛔ EHRLICH: das ist eine EICHUNG des Wirtsrasterers (SDL/GPU) gegen ein echtes
 * PSX-Bild, KEINE aus einem Disassemblat gelesene Konstante — SDL_RenderGeometry ist
 * nicht die PSX-GPU, es gibt dafuer keine @0x-Adresse. Die Belege sind der
 * Texturversatz (@Datei 0x018DAC), der PSX-Abzug (analysis/grundwahrheit/) und die
 * Fahrt oben. Gemessen ist eine Szene (ROOM11F0 Cut 10, Nadir-Kamera, 11 Props,
 * 2 TIMs); ob +0.375 auch fuer stark perspektivische Figuren-Quads das Optimum ist,
 * ist NICHT gemessen. Was gemessen ist: -0.5 ist dort schlechter als 0, und +0.375
 * ist besser als beide.
 *
 * RE15_UVPHASE bleibt als Messschiene: die Fahrt oben ist ohne Neubau wiederholbar.
 * Riegel: re15_port/tests/unit/probe_abtastphase_11f0.c (rot bei -0.5 UND bei 0). */
#ifndef RE15_ABTASTPHASE_H
#define RE15_ABTASTPHASE_H

/* Verschiebung der SDL-Scheitelposition in Pixeln; s. Kopf. */
#define RE15_UV_ABTASTPHASE 0.375f

/* Die Grundwahrheit, die der Riegel nachrechnet — Bildschirmzellen aus
 * analysis/grundwahrheit/psx_room11f0_cut10.png (PSX, ROOM11F0 Cut 10). */
#define RE15_GW_QUAD_X0      159   /* Scheitel-x der Kreuz-Quadspalte (RE15_TRILOG) */
#define RE15_GW_QUAD_X1      162
#define RE15_GW_QUAD_U0       55
#define RE15_GW_QUAD_U1       73
#define RE15_GW_QUAD_Y0      117   /* Scheitel-y der Kreuz-Quadzeile (RE15_TRILOG) */
#define RE15_GW_QUAD_Y1      120
#define RE15_GW_QUAD_V0       34
#define RE15_GW_QUAD_V1       46
#define RE15_GW_HELL_X       160   /* PSX: helle Spalte des senkrechten Kreuzarms */
#define RE15_GW_SCHATTEN_X   161   /* PSX: seine Schattenspalte */
#define RE15_GW_HELL_Y       118   /* PSX: helle Zeile des waagerechten Kreuzarms */
#define RE15_GW_SCHATTEN_Y   119   /* PSX: seine Schattenzeile */

/* CLUT-Indizes des Cursor-TIM, ROOM11F0.RDT @Datei 0x018DAC (selbst ausgelesen). */
#define RE15_CUR_IDX_HELL     4    /* 0x035B (216,208,0) */
#define RE15_CUR_IDX_HELLKERN 5    /* 0x47FE (240,248,136) */
#define RE15_CUR_IDX_SCHATTEN 2    /* 0x00E8 ( 64, 56,0) */
#define RE15_CUR_TIM_OFF      0x018DACu

#endif /* RE15_ABTASTPHASE_H */
