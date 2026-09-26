# Pruefung: raum11f0-raetsel-cursor.md

Status: ABGESCHLOSSEN — URTEIL: NICHT HALTBAR
Pruefer: Skeptiker-Agent, 2026-09-26

## Zu pruefende Behauptung
Der verzerrte Cursor-Schatten entsteht, weil der PC-Zeichner jedes Quad an der
Diagonale v0-v3 statt an der PSX-Diagonale v1-v2 teilt
(main.c:9514/9520 gegen psx-spx und FUN_800256b0 @0x800256c0 POLY_GT4).

## Pruefpunkte
1. [x] psx-spx: wie teilt die GPU ein POLY_GT4 wirklich?
2. [x] @0x800256c0 / FUN_800256b0 selbst disassemblieren
3. [x] main.c:9514 / :9520 + "vier weitere Stellen" — Kollateral
4. [x] Einfachere Erklaerung (gebackener Schatten -> UV/Skalierung)?
5. [x] Entscheidende Messung
6. [x] Ueberzogene Saetze woertlich

## Befunde
(folgen)

## A. psx-spx — Zitat stimmt WÖRTLICH (Pruefpunkt 1 ✅)
`info/.../psx-spx.github.io-master/docs/graphicsprocessingunitgpu.md:208-212`, selbst gelesen:
> "Quads are internally processed as two triangles, the
> first consisting of vertices 1,2,3, and the second of vertices 2,3,4. This is an
> important detail, as splitting the quad into triangles affects the way colours
> are interpolated."
Null-basiert (v0,v1,v2)+(v1,v2,v3) → Diagonale v1–v2. Das Zitat ist korrekt wiedergegeben.
ABER: der Satz sagt "affects the way **colours** are interpolated" — er spricht von der
GOURAUD-Farbe, nicht von der UV-Abbildung. Das Dossier verallgemeinert ihn auf die Textur.

## B. main.c:9514/9520 — Code sagt, was behauptet wird (Pruefpunkt 3, teilweise ✅)
Selbst gelesen: die beiden Aufrufe verwenden (ax0,ax1,ax3) und (ax0,ax3,ax2) → Diagonale v0–v3.
Kontext der 5 Stellen (selbst gelesen): 1916/1918 = Spieler-AUSWAHL-Modelle
(`pselect_render_model`, main.c:1755), 7996/8002 = SPIELERMODELL (Leon/Ada, lctx_player),
8205/8211 = WAFFE IN DER HAND (PLW, main.c:8118-8130), 9265/9271 = GEGNER/NPC (lctx_npc),
9514/9520 = Raum-Props. Also praktisch JEDES texturierte Viereck im Spiel.
Gegenbehauptung IM CODE, die das Dossier nicht erwaehnt: main.c:7995
`/* Quad -> 2 tris with (v0,v1,v3,v2) winding (matches PSX). */` — dort steht das Gegenteil.

## C. @0x800256c0 — stimmt, ist aber nur die halbe Kette (Pruefpunkt 2, teilweise)
Selbst disassembliert (PSX.EXE, `re15_disasm.py dis 0x800256b0 90`):

```
800256bc: sll a3,a3,1
800256c0: ori a3,a3,0x3c      ; 0x3C = POLY_GT4
800256ec: sb  a3,3(a2)
```

Belegt: RE1.5 gibt EIN Vierpunkt-Primitiv ab. NICHT belegt hat der Autor, in welcher
REIHENFOLGE die vier Ecken in xy0..xy3 landen (0x8002570c/0x80025710/0x80025720 laden die
Indizes +2/+6/+10 fuer das RTPT, die vierte Ecke separat @0x800257a0). Die Strip-Ordnung ist
stattdessen aus den DATEN belegt (Abschnitt D), nicht aus dieser Funktion.

## D. DER KERN: die Kausalkette ist fuer DIESEN Cursor nicht nur unbewiesen, sondern FALSCH
Der Autor schreibt woertlich (Dossier 3.4):

> "Für ein Quad, dessen Bildschirm-Viereck unter Perspektive KEIN Parallelogramm ist (und
> das ist es hier nicht, **die Kamera schaut schräg auf das Brett**) …"

Das ist falsch. Kamera-Cut 10, selbst aus `ROOM11F0.RDT` gelesen (Tabelle @Datei 0x0060,
Eintrag 10 @Datei **0x01A0**), Bytes:
`00 00 3c 68 | 54 b3 ff ff | de bb ff ff | 58 58 00 00 | 54 b3 ff ff | 38 3e 00 00 | 59 58 00 00 | 08 08 00 00`
→ pos = (-19628, -17442, 22616), tgt = (-19628, 15928, 22617), fov = 26684
→ Blickrichtung (dx,dy,dz) = (0, +33370, +1) = **NADIR, senkrecht nach unten**.
Bestaetigt durch den bereits gebauten
`re15_port/build/tests/unit/probe_gen_11f0_cursor_view.exe`, den ich nur AUSGEFUEHRT habe:
`fov_screen_dist=208`, Cursor-Bbox x[149..172] y[107..130].

Durch `re15_camera_build_view` (`re15_port/engine/src/camera_common.c:77-105`) wird daraus
mit dx=0, horiz=SquareRoot0(1)=1, sy=0, cy=4096 und cp = 1*4096/33370 = **0**:

```
V = [ 4096   0     0  ]     view_x =  world_x
    [   0    0    sp  ]     view_y = -world_z
    [   0   -sp    0  ]     view_z =  world_y     (sp = -4096)
```

**Die Sehtiefe haengt NUR von world_y ab.** Die Oberseite des Cursors liegt vollstaendig in
EINER Ebene y = const — MD1 @Datei 0x001928, selbst geparst: 49 Quads mit y = -900 (die
einzigen sichtbaren), dazu 49 vollstaendig transparente Quads in z = +900 und 16 Dreiecke.
Also ist fuer alle vier Ecken jedes Quads `_vz` identisch → `n = re15_gte_divide(208,_vz)`
identisch → `ax` haengt nur von world_x ab, `ay` nur von world_z ab.
⇒ **Jedes Quad projiziert auf ein exakt achsparalleles Rechteck in ganzzahligen
Bildschirmkoordinaten** — auch NACH der Integer-Truncation, weil die Rundung spaltenweise
bzw. zeilenweise dieselbe ist.

Selbst nachgerechnet aus der Datei (alle 49 Oberseiten-Quads):

* Welt-XZ-Twist `p0-p1-p2+p3` = (0,0) fuer **49 von 49**
* UV-Twist `t0-t1-t2+t3` = (0,0) fuer **49 von 49**
* Eckenreihenfolge = Strip (v0=TL, v1=TR, v2=BL, v3=BR), z.B. Quad 0:
  (386,-386)(643,-386)(386,-643)(643,-643) mit uv (90,58)(107,58)(90,69)(107,69)

Fuer ein Parallelogramm mit Parallelogramm-UVs sind **beide Zerlegungen mathematisch
dieselbe affine Abbildung** — Diagonale v0–v3 und Diagonale v1–v2 liefern Pixel fuer Pixel
dasselbe Ergebnis. **Die vorgeschlagene Aenderung kann am ROOM11F0-Cursor kein einziges
Texel bewegen.** Die Kausalkette ist damit nicht nur "belegt-nebeneinander", sie ist
widerlegt.

## E. Die einfachere Erklaerung, die der Autor uebersehen hat (Pruefpunkt 4)
Selbst aus dem TIM @Datei 0x018DAC gemessen (8bpp, 128x256, eine CLUT 256x1):

* senkrechter Kreuzarm HELL = Palette-Index 4 (216,208,0) auf **u 59..63 = 5 Texel breit**,
  v 21..60 = 40 Texel hoch; sein Schatten = Index 2 (64,56,0) auf **u 64..68**, also
  unmittelbar daneben.
* waagerechter Arm: u 32..91 (60 Texel) bei v 38..44 (**7 Texel hoch**).

Massstab (Oberseite u 1..127 = 126 Texel, v 0..82 = 82 Texel, auf 24 x 24 Pixel):
**5,25 Texel/Pixel in u, 3,4 Texel/Pixel in v.** Damit:

* waagerechter Arm = 7/3,4 = **2,1 Pixel hoch** → ueberlebt das Punktsampling. Genau der
  wird im Dossier als korrekt gemessen.
* senkrechter Arm = 5/5,25 = **0,95 Pixel breit**, und sein Schatten liegt **0,95 Pixel**
  daneben (+5u). Bei NEAREST-Sampling entscheidet allein die **Sub-Pixel-Phase**, ob diese
  eine Spalte das helle oder das dunkle Band trifft; eine halbe Pixelbreite Phasenfehler
  (= 2,6 Texel) kippt sie.

Genau das ist der Messbefund des Dossiers ("Schatten 1 px zu weit links", "8 von 11 Zeilen
in Schattenfarbe"): 1 px = 5 Texel = **exakt der gebackene Schattenversatz**.
Kandidat fuer die Phase, mit Zeile: `re15_port/platform/pc/src/render_pc.c:2345-2346` uebergibt
`tex_coord = u * (1/s_tim_w)` **ohne Texel-Mitten-Offset (+0,5)**, waehrend SDL_RenderGeometry
auf der GPU an der PIXELMITTE abtastet. Bei 5,25 Texel/Pixel ist das ein systematischer
Versatz von rund 2,6 Texeln — dieselbe Groessenordnung wie der Fehler und um Groessenordnungen
mehr, als die Diagonale hier bewirken kann (naemlich nichts).
Zweitkandidat: der Doppelrand ist ORIGINAL sichtbar. Der Autor hat KEINE PSX-Gegenmessung;
bei 5-facher Minifikation zeigt die PSX-GPU denselben 1-Pixel-Doppelrand moeglicherweise auch.

## F. Risiko des Umsetzungsplans
Dossier 5.1 will die Diagonale an ALLEN fuenf Stellen drehen. Betroffen waere damit jedes
texturierte Viereck von Spieler (`main.c:7996/8002`), Waffe in der Hand
(`main.c:8205/8211`), allen Gegnern/NPCs (`main.c:9265/9271`), allen Raum-Props
(`main.c:9514/9520`) und dem Spieler-Auswahlbild (`main.c:1916/1918`). Anders als beim
Cursor sind Figuren-Quads NICHT planar und nicht parallelogrammfoermig — dort aendert sich
das Bild wirklich.
Zusaetzlich aendert sich die Malreihenfolge: `re15_port/platform/pc/main.c:7993-7994`
bildet `avg_z1 = (wz0+wz1+wz3)/3` und `avg_z2 = (wz0+wz3+wz2)/3` als Sortierschluessel der
beiden Haelften; mit der anderen Diagonale werden daraus andere Werte, d.h. die
Painter-Reihenfolge innerhalb jeder Figur verschiebt sich (Memory: "Runde 19: vier
PRI-Marken"). Alles das fuer einen Cursor, an dem die Aenderung beweisbar nichts tut.

## G. Entscheidende Messung (Pruefpunkt 5)
1. PSX-Grundwahrheit (Skill `re15-room-capture`): DuckStation, Debug-Menue → Stage 1,
   Raum 11F0; am Bedienfeld ausloesen (AOT slot 1, sce=3, @Datei 0x00D64), damit sub16
   @0x15A2 laeuft und `Cut_chg 0x0A` @0x15C0 greift; Savestate; Framebuffer-Dump
   (Skill `re15-savestate-ghidra`), Cursor auf der Startzelle (-19554, 22684).
   **Zu lesen ist genau EINE Pixelspalte:** im Kasten x 149..172 / y 107..130 die Spalte,
   die den senkrechten Kreuzarm traegt (Port heute: x = 160 in Index-2-Braun (64,56,0),
   Schatten bei x 159..160). Zeigt die PSX dort Index 4 (216,208,0), ist es ein
   Port-Defekt — und zwar ein Sampling-Phasen-Defekt. Zeigt sie dasselbe Braun, gibt es
   ueberhaupt keinen Defekt.
2. Port-seitiges A/B, das die richtige Hypothese testet: NICHT die Diagonale, sondern
   +0,5 Texel in `render_pc.c:2345-2346`, danach derselbe `RE15_FRAMEDUMP`-Lauf und
   dieselbe Tabelle wie Dossier 3.3. (Braucht einen Bau — in dieser Runde verboten.)
3. Ein A/B der Diagonale ist am Cursor nutzlos: nach D kommt bitgleich dasselbe Bild.

## H. Ueberzogene Saetze, woertlich (Pruefpunkt 6)
1. "### 3.4 **DIE URSACHE**: der Port teilt jedes Quad an der FALSCHEN Diagonale" — die
   Ueberschrift behauptet die Ursache, 6.1 desselben Dossiers sagt "ist **nicht gemessen**".
2. Urteil, Zeile 5: "und er verzerrt, **weil** der PC-Zeichner jedes Quad an der falschen
   Diagonale teilt" — kausales "weil" ohne Messung, nach D falsch.
3. 5.1 "**Der Schatten — EINE Änderung, belegt**" und 5.4 "1. §5.1 (Quad-Diagonale) —
   eigenständig, **belegt**, sofort umsetzbar."
4. "(und das ist es hier nicht, die Kamera schaut **schräg** auf das Brett)" — falsch,
   Cut 10 ist eine Nadir-Kamera (RDT @Datei 0x01A0).
5. psx-spx sagt "affects the way **colours** are interpolated" — das Dossier uebertraegt
   den Satz stillschweigend auf die TEXTUR-Abbildung.

## Was das Dossier RICHTIG hat
* Das psx-spx-Zitat ist woertlich korrekt; die PSX teilt POLY_GT4 an v1–v2.
* Der PC-Zweig teilt tatsaechlich an v0–v3 (main.c:9514/9520 und vier weitere Stellen).
* Der PSX-Zweig `re15_port/platform/psx/src/mesh_psx.c:681-706` schickt wirklich ein
  einzelnes POLY_GT4 in Strip-Ordnung.
* Der Schlagschatten ist wirklich in die Textur gebacken (+5u/+3v, Index 1 hinter 3 und
  2 hinter 4/5) — selbst nachgemessen.
* Es gibt also einen echten, belegten Abweichungspunkt gegenueber der PSX-GPU. Er ist nur
  NICHT die Ursache des gemeldeten Cursor-Schattens und gehoert in einen eigenen Befund
  mit eigener Messung.

## URTEIL: NICHT HALTBAR
Die Diagonale ist ein echter Unterschied zur PSX, aber am ROOM11F0-Cursor beweisbar
wirkungslos (Nadir-Kamera → alle 49 sichtbaren Quads sind exakte Parallelogramme, 49/49
selbst nachgerechnet). Der Umsetzungsplan wuerde Spieler, Waffe, alle Gegner, alle Props
und das Auswahlbild anfassen, um ein Symptom zu behandeln, das er nicht erzeugt. Die
naheliegende Erklaerung — 5 Texel breiter Arm bei 5,25 Texel/Pixel und ein Schatten exakt
einen Pixel daneben, also reine Sampling-Phase — ist ungeprueft geblieben.
