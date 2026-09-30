# Spur G — ROOM1170: blinkende Gebaeudeschrift im Hintergrund

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code), danach GEGENPRUEFUNG und BAU. Zweig `r34n/schrift1170`.
Stand: Ermittlung abgeschlossen (2026-09-30, 02:30-03:45); Gegenpruefung 04:38 ("haltbar mit
Auflagen"); Stufe BAU 05:15-: alle neun Auflagen eingearbeitet (Markierung **[BAU, Auflage n]** an
jeder geaenderten Stelle, Abhakliste in Abschnitt 9.1). Kein Port-Code.

## 0 Kurzfassung

* **Lesart:** "die Schrift des Gebaeudes im Hintergrund" = das Leuchtschild **MAGAZINE CLUB** am Haus
  gegenueber (Hintergrund-Cuts 2, 3, 10; in Cut 4 nur ein Rand von 18 Bildpunkten). Es ist die
  einzige Schrift auf einem Gebaeude in allen 13 Hintergruenden von ROOM1170 (Zensus Abschnitt 1).
* **Befund Original: die Schrift blinkt NICHT.** Gemessen am laufenden Original (MZD-Disk = der
  Auslieferungsstand `info/Re1.5`, DuckStation Software-Renderer 1x). **[BAU, Auflage 3]** Tragend
  sind nur Messungen MIT Lebendnachweis, dazu die Savestates und die statische Kette:
  * **tragend:** gesamter Vorspann ab der Montage (rec4, 150 s, 30 Bilder/s, verlustfrei), in Stufe
    BAU ohne Schwelle Bild fuer Bild nachgemessen (`rec_leben.py`, G17 A): das Schild-Rechteck
    aendert sich in **keinem** Bild um auch nur einen Farbwert — Cut 2 in drei Abschnitten 510 + 193
    + 37 Bilder, Cut 3 nach dem Ausblenden des Kinobalkens 1736 Bilder —, waehrend sich im SELBEN
    Abschnitt das Bild anderswo aendert (250/67/14 bzw. 759 Bildwechsel: Figuren, Leon).
  * **tragend:** alle 23 ROOM1170-Savestates mit sauberer EXE (G16; Cut 1: 3, Cut 2: 11, Cut 3: 9,
    Vcount 8049..15369): in den 20 Staenden mit Schild im Bild ist das Schild in BEIDEN Bildspeichern
    gleich der RAM-Hintergrundkopie 0x80198000 (max |Δ| 0), und **kein GPU-Primitiv** der drei OTs
    schneidet das Schild-Rechteck des jeweiligen Cuts; die RAM-Kopie ist in allen 23 gleich der
    BSS-Dekodierung (max |Δ| 10 = 15-Bit-Quantisierung + MDEC-Rundung).
  * **ohne Lebendnachweis (nur ergaenzend):** Cut 2, 300 s mit 60 Bildern/s, Ausschnitt 96x48 um das
    Schild (rec3): 18000 Bilder, 0 Aenderungen — der Ausschnitt enthaelt nichts, was sich bewegen
    koennte. Cuts 5, 8, 9, 10, 11, 12 per RAM-Patch erzwungen (je 13-28 s): schwellenfrei **0
    Bildwechsel im ganzen Bild** (kein Leon, kein Hubschrauber im Bild; Schild Cut 10 und Notausgang
    Cut 8 je Spanne 0) — ein stehender Emulator saehe gleich aus (G17 B/C). Die fruehere Formulierung
    "0 Pixel mit Hub > 40" war schwellenbehaftet und ist ersetzt.
* **Mechanismus: gibt es nicht.** Der Hintergrund wird je Bild unveraendert aus der RAM-Kopie
  hochgeladen (`jal 0x80043870` @0x8002157c → `LoadImage` @0x800438b0, RECT {0, y, 0x140, 0xF0});
  die Kopie wird nur beim Cut-Wechsel geschrieben (MDEC `jal 0x80053a8c` @0x80021e34, `StoreImage`
  `jal 0x80068cec` @0x80021e44). **[BAU, Auflage 6]** Vollzensus aller GPU-Bildtransfers (EXE:
  LoadImage 22, StoreImage 4, MoveImage 2, ClearImage 4; STAGE1..6.BIN: 0) ergibt keinen weiteren
  Schreiber in den Bildspeicher; einzige Auslass-Bedingung des Uploads ist DAT_800b536c != 0 (Abschnitt
  3.1). ROOM1170 hat keinen Raum-ESP (RDT+0x4C = 0), keine SCD-Schleife auf Hintergrund/Objekte, und
  keines der 6 Objektmodelle ist ein Schild.
* **Port:** zeigt das Schild in Cut 2/3/10 pixelgleich zur BSS-Dekodierung (max |Δ| 0) — per
  Debug-Sprung (71 Bilder) und **[BAU, Auflage 2]** im echten Durchlauf Titel → NEW GAME → ROOM1240 →
  ROOM1170 (Gegenpruefung G15 A, eigene Abnahme Abschnitt 9.5) — also genau wie das Original.
* **Ergebnis: kein Umbau.** Nach der Regel "Ist das im Original nicht so, ist der Port richtig"
  (memory reai-v2-original-oder-nicht) ist der Port hier bereits byte-true. Offen bleibt allein die
  Beobachtung des Nutzers — Abschnitt 8 formuliert die Messfrage (keine Wahlfrage).
* ⛔ Nebenwirkung: die DuckStation-Laeufe der Ermittlung haben `HASH-957757946319438E_resume.sav`
  (Stand 00:06 Uhr) ueberschrieben (Abschnitt 7). Stufe BAU hat DuckStation nicht gestartet.

## 1 Nutzerwortlaut + Lesart

Wortlaut (AUFTRAG.md, letzter Punkt): *"In ROOM 1170 blinkt im Hintergrund die Schrift des Gebäudes.
Bei uns nicht."* "blinkt" benutzt der Nutzer auch fuer den Helligkeitspuls des Titelmenues (Runde 30,
AUFTRAG D) — gemeint ist also jede periodische Helligkeitsaenderung, nicht nur an/aus. Beides wurde
gemessen.

**Zensus aller Schriften auf Gebaeuden in ROOM1170.** Alle 13 Hintergruende (ROOM117.BSS, Schnitt
n = Datei[n*0x10000], Laenge 0x10000, Regel aus bg_pc.c) mit den Engine-Funktionen des Ports
dekodiert (`probe_r34n_g_bss`, Bild `G_belege/G01_bss_alle_cuts.jpg`). Schrift auf einem Gebaeude
steht nur an drei Stellen:

| Kandidat | Cuts | Lage im Bild (320x240) | Beleg |
|---|---|---|---|
| Leuchtschild **"MAGAZINE CLUB"** (weisse Schrift auf blauem Grund, Haus gegenueber) | 2, 3, 10 (+ Rand in 4) | Cut 2 x285..312 y50..63 (356 px); Cut 3 x292..319 y6..21 (427 px); Cut 10 x262..287 y0..10 (248 px); **[BAU, Auflage 5]** Cut 4 nur der rechte Rand x317..319 y12..21 (18 px), im Vorspann unter dem oberen Kinobalken (y0..24) (Blau-Maske b>150, b>r+60, b>g+40 auf der BSS-Dekodierung, in Stufe BAU fuer alle 13 Cuts nachgezaehlt: sonst 0 px) | `G02_schild_cut02_03_10.png`, G15 F |
| Plakatwand + senkrechtes Band (".. Fashion / Men / Clothing / Furniture") | 6 (Himmel beim Abflug) | Plakat x97..150 y195..235; Band x248..272 y140..215 | `G01` |
| gruenes Notausgangs-Piktogramm ueber der Tuer | 8 | x230..275 y5..30 | `G09` |

Sonst keine Schrift: Cut 0/1 und der Rest von Cut 4 zeigen Fassaden nur mit Fenstern (vergroessert
geprueft); das Schild am Tor (Cut 12) hat der Nutzer selbst als "schon im Original unleserlich"
eingeordnet (memory reai-v2-tor-1170-tuermodell).

**Gewaehlte Lesart:** das Leuchtschild **MAGAZINE CLUB**. Begruendung: einzige lesbare Schrift auf
einem Gebaeude; steht wirklich im Hintergrund (Haus jenseits der Strasse); Leuchtschild = das einzige
Objekt, bei dem "blinken" Sinn ergibt. Die beiden anderen Kandidaten wurden trotzdem mitgemessen
(Abschnitt 3.3): sie blinken im Original ebenfalls nicht.

## 2 Ist-Zustand im Port (gemessen)

**Code-Pfad.** `bg_pc.c`: `re15_bg_load_room_cut()` dekodiert den BSS-Chunk EINMAL je Cut-Wechsel
(`re15_bg_load_from_bss`: VLC → Software-MDEC → `s_bg_cache`), `re15_bg_blit()` kopiert je Bild
`s_bg_cache` unveraendert in den Bildspeicher. Keine Animationsstufe dazwischen — das entspricht dem
Original (Abschnitt 3.1). Raum-ESP: ROOM1170 hat keinen (RDT+0x4C = 0), also laedt `pc_load_room_esp`
nichts.

**Messung an der echten exe** (`re15_pc_g.exe` = Kopie von `re15_pc.exe` dieses Baums, Bau aus cf0e68ba = Rundenstand, nur Sonde nachgebaut;
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_DEBUG_JUMP=1170@240 RE15_WINDOW_SCALE=3 RE15_CUT_SYNC_LOG=cutsync.log RE15_FRAMEDUMP=<a>-<b>/<s>:…`):

| Lauf | Bilder | Ergebnis Schild-Ausschnitt gegen BSS-Dekodierung |
|---|---|---|
| Vorspann F400-1500/20 | 36 Bilder Cut 2 (F480-1380) | blau 356, L 130,4, **max \|Port − BSS\| = 0** in allen |
| Freilauf F1800-2600/25 | 33 Bilder Cut 3 | blau 427, L 135,5, **max \|Δ\| = 0** in allen |
| RE15_FORCE_CUT=10, F1790/F1800 | 2 Bilder Cut 10 | blau 248, **max \|Δ\| = 0**; Bild = Original-Cut-10 (`G10`) |
| F1760-1800/1 | Ende Vorspann → Cut 3 | Kinobalken blendet in 13 Bildern aus (F1769 L 8,3 … F1782 L 135,5), danach konstant |

Tabelle: `G_belege/G14_port_schild_stats.txt`. Bilder: `G03`/`G04` (Cut 2 Original gegen Port, gleiche
Szene "Is there no one else left inside?"), `G08` (Port-Vorspann), `G10` (Cut 10 Original/Port, Cut 3
Port).

**[BAU, Auflage 2] Messung im ECHTEN Durchlauf** (memory reai-v2-playthrough-not-jumpin; der
Debug-Sprung oben bleibt als zweite Probe stehen). Weg: Titel → NEW GAME → Leon → ROOM1240 (Montage)
→ ROOM1170, kein Sprung. Umgebung (G15 A, wortgleich):
`RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_PSELECT_AUTO=1 RE15_WINDOW_SCALE=3 RE15_INPUT_SCRIPT="W2,S1,W900"
RE15_INPUT_SCRIPT_START=30 RE15_CUT_SYNC_LOG=cutsync.log RE15_FRAMEDUMP="0-5200/10:f"
RE15_EXIT_AT="5200#1170"`.
⛔ `frame_count` beginnt je Raum bei 0: die Dumps der ROOM1240-Montage (`f_F<n>`) werden von denen aus
ROOM1170 mit gleicher Bildnummer ueberschrieben — ausgewertet wird deshalb nur, was nach dem
Raumwechsel liegt (Kamera je Bild aus `cutsync.log`, `view=`).

| Durchlauf | Bilder | Ergebnis Schild-Ausschnitt gegen BSS-Dekodierung |
|---|---|---|
| Gegenpruefung (G15 A), Stand cf0e68ba, jedes 10. Bild bis F5200 | Cut 2: 73 Bilder (F480..F1390) | blau 356, L 130,4, **max \|Δ\| = 0** in allen |
| dieselbe | Cut 3 ab F1790 bis F5200 | **max \|Δ\| = 0**; F1770/F1780 = Ausblenden des Kinobalkens |
| dieselbe | Cut 3 F1400/F1410, Cut 4 F1550..F1760 (Vorspann) | Schild unter dem Kinobalken (L 0,1..1,3), fuer Cut 3 wie im Original (rec4: Schild erst ab dem Ausblenden sichtbar) |
| eigene Abnahme Stufe BAU (Abschnitt 9.5) | siehe dort | siehe dort |

**Ist = Soll:** der Port zeigt das Schild in allen drei Cuts dauerhaft und unveraendert, wie das Original.

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

### 3.1 Wie der Hintergrund im Original je Bild entsteht (PSX.EXE, disassembliert mit re15_disasm.py)

Present-Routine FUN_8002137c (Decompilat `RE_15_Quellcode_V2/FUN_8002137c.c`, Bytes nachgeprueft):

    80021558: jal 0x80021bbc          ; Cut-Wechsel angefordert (DAT_800b5457) -> Cut anwenden
    80021560: j   0x800215fc          ;   ... und dieses Bild NICHT zeichnen
    80021568: lui a0,0x800b
    8002156c: lbu a0,-13772(a0)       ; DAT_800aca34 = Doppelpuffer-Index
    80021574: sltu a0,zero,a0
    80021578: subu a0,zero,a0
    8002157c: jal 0x80043870          ; SONST: Hintergrund hochladen, y = -(buf!=0) & 0xF0
    80021580: andi a0,a0,0xf0

FUN_80043870 (der einzige Hintergrund-Zeichner im Spielbetrieb):

    80043874: lui a1,0x8019
    80043878: ori a1,a1,0x8000        ; Quelle = RAM-Kopie 0x80198000
    80043880: lw  v1,-31412(v1)       ; DAT_800b854c
    80043888: bne v1,v0(=1),0x80043898
    80043894: ori a1,a1,0x8014        ;   ==1 -> Quelle +0x14 (10 Pixel versetzt; nur Optionsbild)
    80043898: ori v0,zero,0x140       ; w = 320
    800438a0: ori v0,zero,0xf0        ; h = 240
    800438a4: sh  a0,18(sp)           ; y = Parameter
    800438ac: sh  zero,16(sp)         ; x = 0
    800438b0: jal 0x80068c88          ; LoadImage(&rect, quelle)

Die RAM-Kopie schreibt nur der Cut-Wechsel FUN_80021bbc:

    80021e34: jal 0x80053a8c          ; MDEC: VLC 0x80190000 -> 0x80199e00 -> Streifen 0x80198000 -> VRAM
    80021e38: sh  v0,0(v1)            ; RECT.y @0x80072f2e = -(buf!=0) & 0xF0  (RECT @0x80072f2c = {0,0,320,240})
    80021e44: jal 0x80068cec          ; StoreImage(RECT 0x80072f2c, 0x80198000)
    80021e48: ori a1,a1,0x8000

Uebrige Schreiber von 0x80198000 (ghidra1_V2.txt, 16 Verweise) sind andere Bildschirme als
Arbeitspuffer: YOUDIED.TIM (FUN_8001613c), Config.tim/C_back.tim (FUN_8002dfb0), MEMORY CARD BG
(FUN_800264e8), Statusbild/Karte (FUN_80049a5c, FUN_8004d96c), Waffendatei-Laden
(`jal 0x80043d8c` mit Waffe DAT_800aca5d, Ziel 0x80198000, @0x800466c4/c8). **Kein Pfad veraendert die Hintergrundkopie oder den Upload waehrend des Spiels
periodisch.** Alles, was ueber dem Hintergrund liegt, muesste als GPU-Primitiv in einer der drei OTs
stehen (DrawOTag @0x800215bc/@0x800215d0/@0x800215e4 auf 0x800ac714/0x800ab6d4/0x800aa6b4 + buf*Stride).

**[BAU, Auflage 6] Beweiskette erweitert.** Die 16 Verweise auf 0x80198000 schliessen allein nicht
aus, dass ein Blinken als EIGENER Transfer in den Bildspeicher kommt. Deshalb Vollzensus aller
Aufrufstellen der vier GPU-Bildtransfers, in Stufe BAU selbst nachgezaehlt (jal-Wort
`0x0C000000 | (ziel>>2)` in `info/Re1.5/PSX.EXE` [t_addr 0x80010000, t_size 0xaf000] und allen
`info/Re1.5/PSX/BIN/*.BIN`; Werkzeug `re15_port/tools/r34n_g/exe_transfer_zensus.py`, Ausgabe
`G_belege/G18_transfer_zensus.txt`; Aufrufstellen wortgleich zu G15 D):

| Transfer (Name aus dem Binary, G15 D) | jal-Wort | EXE | STAGE1..6.BIN | sonst |
|---|---|---|---|---|
| LoadImage 0x80068c88 | 0x0c01a322 | 22 (0x800141a4 0x80014210 0x800195d8 0x80019618 0x8001dbfc 0x8002a154 0x8002aaa0 0x80036c74 0x80036cb8 0x80036ed8 0x80036f1c **0x800438b0** 0x80049370 0x8004c1ac 0x8004c2e0 0x8004eee0 0x8004ef48 0x80053b60 0x80053c54 0x8006d404 0x8006d47c 0x8006d4e4) | 0 | TITLE.BIN 2, DEBUG.BIN 2 |
| StoreImage 0x80068cec | 0x0c01a33b | 4 (**0x80021e44** 0x8002a088 0x8004c174 0x8004d7fc) | 0 | — |
| MoveImage 0x80068d50 | 0x0c01a354 | 2 (0x8004945c 0x80049504) | 0 | — |
| ClearImage 0x80068bf4 | 0x0c01a2fd | 4 (0x8001dbd4 0x80020cd8 0x8002ad90 0x8004ee20) | 0 | — |
| Huelle FUN_80043870 (Hintergrund) | 0x0c010e1c | 1 (0x8002157c) | 0 | — |
| Huelle FUN_80021bbc (Cut-Wechsel) | 0x0c0086ef | 1 (0x80021558) | 0 | — |

Die Zuordnung der LoadImage-Stellen (ESP-TIM, Blende 1 Pixel @(0,0x1e9), Film, Waffen-TIM y=0x1e0,
Statusschirm x>=0x2c0, Menueseite, TIM-Lader beim Raumladen, MDEC beim Cut-Wechsel, libgpu) steht in
G15 D; keine davon laeuft periodisch im Spielbetrieb auf x0..319 y0..479.

**Einzige Bedingung, die den Hintergrund-Upload auslaesst** (in 3.1 zuvor nicht genannt), selbst
disassembliert:

    80021518: lui v0,0x800b
    8002151c: lbu v0,21356(v0)        ; DAT_800b536c (Bildschirm-Modus)
    80021524: bne v0,zero,0x80021584  ; != 0 -> Cut-Anforderung UND Upload (0x8002157c) ueberspringen,
                                      ;         weiter mit DrawOTag @0x800215bc
    80021634: lui at,0x800b           ; FUN_80021634 = einziger Schreiber:
    80021638: sb a0,21356(at)         ;   DAT_800b536c := a0

Einziger Store mit Versatz 0x536c in der EXE ist `sb` @0x80021638 (Basis `lui 0x800b`), in
STAGE1..6.BIN keiner. FUN_80021634 hat **12** EXE-Aufrufer (G15 D nennt 10; dazu kommen
@0x8001d24c mit a0=2 und @0x8001d5cc mit a0=0 samt Cut-Anforderung DAT_800b5457:=1 @0x8001d5c8),
TITLE.BIN 7, STAGE1..6.BIN 0 — kein Overlay und kein Raum-Skript kann den Upload also aus- und
einschalten. Im Spielbetrieb steht DAT_800b536c auf 0: gelesen in allen 23 ROOM1170-Savestates (G16),
d.h. der Upload laeuft dort in jedem gezeichneten Bild.

### 3.2 Raumdaten ROOM1170 (Auslieferungsstand = MZD-Disk)

| Pruefung | Ergebnis |
|---|---|
| RDT-Header | nSprite 0, nCut 13, nOmodel 6; Effekt-Sektion **RDT+0x4C = 0** (kein Raum-ESP), ESP-TIM 0 |
| SCD (scd_dump_room.py) | main00 + 16 Subs: Tueren/AOTs, Vorspann sub02/sub11/sub14, Hubschrauber-Bewegung sub04-08/12/13 (Speed_set/Add_speed auf Objekte 2-4), Warteschleifen sub09/10 auf Flag (5,32)/(5,33). **Kein Opcode wirkt auf Hintergrund, Schild oder Licht periodisch.** ROOM1171 ebenso. |
| Objektmodelle (RDT+0x30, TIMs dekodiert) | obj0/1 = Kiste ("ASH tex"), obj2-4 = Hubschrauber (Rumpf/Rotor/Heckrotor), obj5 = Pilot — **kein Schild-Modell** |
| sprite.pri (Kamera +0x1C) | Masken nur in Cut 1/2/4/5; Atlanten zeigen rote Lampe, Kiste, Gelaender — **kein Schild**. Cut 2: 1 Index bei (135,10), 3 Quadrate 16x16 bei (128,128)/(128,144)/(144,128) → Bild x263..295 y138..170 = rote Lampe, weit weg vom Schild (x285..312 y50..63) |
| Kamera-Eintraege (RDT+0x24, 13 x 0x20) | Wort +0 ist `end_flg` (nur Cut 12 = 1, BioRdt global.h RCUT) — kein Bild-Modus |
| BSS-Chunks | Chunk endet jeweils bei L (Stage-Tabelle @STAGE1.BIN+0x1EAE4, sll 5 / sll 1 @0x80021d50/4c); keine zweite MDEC-Aufnahme im Chunk, keine versteckten Bilder |
| MZD-Disk gegen Auslieferungsstand | RDT im Savestate-RAM (DAT_800ac778 = 0x801219b0) byte-gleich (alle 22 Sektionszeiger, SCD 0x11F4..0x1920). **[BAU, Auflage 1] berichtigt:** Die Disk-Dateien PSX.EXE, STAGE1.BIN, ROOM117.BSS, ROOM1170.RDT und ROOM1171.RDT sind bytegleich zu `info/Re1.5` (sha256 G15 C; PSX.EXE `e6b340fd12ec43be…`, ROOM1170.RDT `bcd0b0aac6e60d2c…` in Stufe BAU nachgerechnet, auch gleich `re15_port/shared_assets`). Disk-EXE = `info/Re1.5/PSX.EXE`; der Haken `j 0x800c02c0` @0x80013b7c steht nur im RAM (Datei-Wort `0x3c01800c` = `lui at,0x800c`, im RAM von lamp_near.sav `0x080300b0` = `j 0x800c02c0`) und wird zur Laufzeit gesetzt. Die fruehere Aussage "EXE-Code gleich bis auf einen Datei-Lade-Haken" war falsch formuliert. Header-Byte 0 = 3 im RAM schreibt FUN_800392d4 zur Laufzeit (Maskenzahl des Cuts). |

### 3.3 Dynamische Messung am Original (DuckStation, MZD-Disk, Software-Renderer 1x, TrueColor)

Werkzeuge: `re15_port/tools/r34n_g/` (Abschnitt 6). DuckStation zeigte mit `[Debug] ShowVRAM=true` das
ganze VRAM; aufgenommen wurde Bildspeicher 0 per ffmpeg-gdigrab.

| Messung | Umfang | Ergebnis |
|---|---|---|
| rec1: Cut 2, Freilauf (Start lamp_near.sav) | 25 s, ~28 Bilder/s, Schild-Ausschnitt | alle 718 Bilder gleich; Aenderungskarte des ganzen Bildes zeigt nur Leon (`G06`) |
| rec3: Cut 2, Freilauf — **[BAU, Auflage 3] ohne Lebendnachweis** | 300 s, 60 Bilder/s, 96x48 um das Schild | 18000 Bilder, 0 Aenderungen (schwellenfrei bestaetigt, G17 B); der Ausschnitt enthaelt nichts Bewegliches — ein stehender Emulator saehe gleich aus. Nur ergaenzend. |
| rec2: Pilot-Dialog → Abflug → Freilauf (Start orig_1170_gp.sav) | 70 s, ~15 Bilder/s | Schild Cut 2 konstant (109,8/900), Cut 3 konstant (619 Bilder), Plakat/Band Cut 6 konstant (26,4 / 27,2) |
| **rec4: ganzer Vorspann ab Montage ROOM1240** (orig_intro_late.sav) — **TRAGEND** | 150 s, 30 Bilder/s, verlustfrei | 17 Kamera-Abschnitte, Aenderungen nur an 3D/Untertitel/Blenden; Schild Cut 2: 740 Bilder blau 353, L 134,0 konstant; Cut 3: Kinobalken blendet F2751-2763 aus, danach L 139,2 konstant; Cut 6: Plakat 26,0, Band 26,6 konstant (`G07`). **[BAU, Auflage 3]** schwellenfrei in voller Aufloesung nachgemessen (G17 A): Schild-Rechteck Cut 2 in 510/193/37 Bildern, Cut 3 in 1736 Bildern (ab 2764) **0 Bilder mit Aenderung, Spanne je Pixel/Kanal 0**; Lebendnachweis im selben Abschnitt: 250/67/14 bzw. 759 Bildwechsel anderswo im Bild |
| erzwungene Cuts 5/8/9/10/11/12 (RAM-Patch 0x800aca3c:=0x100 = Zonen-Scan aus, 0x800afbb5:=Cut) — **[BAU, Auflage 3] ohne Lebendnachweis** | je 13-28 s, ~28-30 Bilder/s, verlustfrei | schwellenfrei **0 Bildwechsel im GANZEN Bild** in allen sechs (kein Leon/Hubschrauber im Bild, `G09`); Schild Cut 10 (450 Bilder) und Notausgang Cut 8 (415 Bilder, Rechteck x228..277 y3..32, L 68,49) je **Spanne 0** — ein stehender Emulator saehe gleich aus. Einzig fuer Cut 12 belegt der End-Stand (DuckStation-resume.sav 03:28:12: Vcount 15939 gegen Start 14322 = 1617 VBlanks = 27,0 s), dass emuliert wurde. Die fruehere Aussage "0 Pixel mit Hub > 40" / "Notausgang Cut 8 konstant" ist hierdurch ersetzt. Nur ergaenzend. |
| **[BAU, Auflage 4]** alle Savestates unter `stage_saves/` (Vollzensus `ss_zensus_1170.py`, G16) — **TRAGEND** | 104 Dateien: 6 PATCHED-EXE ausgeschlossen, 1 ohne EXE im RAM (boot_16), **23 ROOM1170-Staende mit sauberer EXE** (Wort @0x80026e4c = 0x03e00008): Cut 1: 3, Cut 2: 11, Cut 3: 9, Vcount 8049..15369 | RAM-Kopie 0x80198000 == BSS-Dekodierung in allen 23 (max \|Δ\| 10 = 15-Bit-Quantisierung + MDEC-Rundung; die frueher genannten "21/23" sind mit dieser Methode — Kanal << 3 gegen die 8-Bit-Dekodierung — nicht reproduziert, die damalige Methode ist nicht dokumentiert); in den 20 Staenden Cut 2/3 Schild in BEIDEN Bildspeichern == RAM-Kopie (max \|Δ\| 0); **OT-Walk aller drei OTs beider Puffer mit dem Schild-Rechteck DES JEWEILIGEN CUTS: 20 Walks, 0 Treffer**. Die frueheren Zahlen "19 Savestates / 18 OT-Walks" waren nicht eingecheckt (G12: 14, G11: 4) und G11 hatte zwei Cut-3-Staende mit dem Cut-2-Rechteck geprueft; `ss_ot_walk.py` waehlt das Rechteck jetzt aus dem Cut (`--box auto`). |

Auswertung: `G_belege/G13_original_aufnahmen_auswertung.txt` (Ermittlung), `G16_savestate_zensus_1170.txt`
und `G17_aufnahmen_lebendnachweis.txt` (Stufe BAU).

**ePSXe-Staende des Nutzers (Juni 2026).** `Downloads/ePSXe2018/sstates/SLU__096.08.001..003` (gzip,
Kopf "ePSXe", RAM ab Datei-Offset 0x1BA, VRAM ab 0x2733DF, Bildspeicher y=0/240) sind die MZD-Disk
(Lade-Haken `0x080300B0` @0x80013b7c vorhanden) in ROOM1170 Cut 10 (.001/.002) und Cut 8 (.003). Auch
dort: Schild Cut 10 in beiden Bildspeichern == RAM-Kopie (100 % gleich), Bildspeicher sonst 97-98 %
gleich (Rest = Leon). ePSXe ist auf `VideoPlugin = GPUCORE` eingestellt (HKCU\Software\epsxe\config).
Einen LAUFENDEN ePSXe-Mitschnitt habe ich abgebrochen: ePSXe laesst sich nur per Tastatur (F2/F3)
auf einen Stand setzen, und der Desktop ist geteilt — waehrend des Versuchs lag die Rueckfrage einer
anderen Sitzung offen; eine Tastatureingabe haette dort landen koennen.

### 3.4 RE2 (Retail) zum Vergleich

* RE2 hat keinen Raum mit diesem Schild: Blau-Schild-Suche ueber alle 1261 dekodierten RE2-Hintergruende
  (`info/re2leon/COMMON/BSS/ROOM*/*.bmp`) ohne Treffer; RE2-ROOM117 ist die 2F-Halle.
* RE2s Hintergrund-Upload FUN_8002b968 (`RE2_Quellcode_V2/FUN_8002b968.c`) kann zusaetzlich ein ZWEITES
  Bild (Raum+0x44, BioRdt `pScrl`) als waagerechtes Band einblenden (`DAT_8009dc10`/`DAT_8009dc0c`/
  `DAT_800d4494`) — ein Roll-Hintergrund, kein Blinken; RE1.5 hat davon nur den +0x14-Versatz
  (RE2 `p = p + 5` bei `DAT_800dfd57`, RE1.5 `ori a1,a1,0x8014` bei DAT_800b854c == 1).
* Einordnung nach memory reai-v2-beta-zu-retail: RE1.5 ist hier **nicht unfertig** — das Schild ist
  fertig gemalt und wird vollstaendig gezeigt. Es gibt also kein RE2-Ziel, das einzusetzen waere.

## 4 Soll-Verhalten (Zeitlinie; Bild fuer Bild, wo relevant)

Das Schild ist in jedem Bild, in dem Cut 2, 3 oder 10 steht, die unveraenderte BSS-Dekodierung. Einzige
zeitliche Aenderung im Schildbereich ist das Ausblenden des oberen Kinobalkens am Ende des Vorspanns
(Cut 3 liegt mit y6..21 unter dem oberen Balken: statische POLY_F4-Daten @0x80072ecc = Ecken
(0,0)(320,0)(0,24)(320,24), unterer Balken (0,216)..(320,240); SetPolyF4/SetSemiTrans-Schleife
@0x80021260-0x800212a8) — die zeigt der Port bereits (Abschnitt 2). Keine Blinkfolge, kein Takt, keine Konstante.

## 5 Bauplan

**Kein Port-Code.** Der Port ist fuer diesen Punkt bereits byte-true (Abschnitt 2 = Abschnitt 3).
Einen Blink-Effekt einzubauen hiesse, ein Verhalten zu erfinden, das weder RE1.5 noch RE2 hat — das
waere genau die Rate-Klasse, die das RE-Gate verbietet.

Was die Integration uebernimmt: dieses Dossier, `G_belege/`, die Werkzeuge `re15_port/tools/r34n_g/`
und die Sonde `probe_r34n_g_bss` (registriert in `probes/r34n_g_schrift.cmake`, **kein add_test**,
baut nur bei Configure mit — kein Einfluss auf die Suite).

### Konstanten-Tabelle

Keine Verhaltenskonstante. Die in den Messwerkzeugen benutzten Werte und ihre Herkunft:

| Konstante | Wert | Beleg |
|---|---|---|
| BSS-Schnitt | Cut n = Datei[n*0x10000], Laenge 0x10000 | bg_pc.c (gemessen ueber 1688 Schnitte) |
| RAM-Hintergrundkopie | 0x80198000 (Variante +0x14) | `ori a1,a1,0x8000` @0x80043878 / `ori a1,a1,0x8014` @0x80043894; StoreImage @0x80021e44 |
| Upload-Rechteck | {0, 0 bzw. 0xF0, 0x140, 0xF0} | @0x80043898/@0x800438a0/@0x800438ac, y @0x8002156c-80 |
| OT-Koepfe | 0x800ac714+buf*0x40, 0x800ab6d4+buf*0x1000, 0x800aa6b4+buf*0x20 | @0x800215b4-e8 (s0 = 0x800aca34) |
| Aktueller Cut / Raum | DAT_800b0fe4 / DAT_800b0fe2 | FUN_80021bbc `lh 4068(v1)` @0x80021d44, `lh 0(s0)` @0x80021d48 |
| VBlank-Zaehler | 0x800787dc | VSync(-1): `lw v0,-30756(v0)` @0x80062004 |
| Zonen-Scan aus (nur Messung) | DAT_800aca3c Bit 0x100 | Katalog "main @0x8001cce0" |
| Schild-Rechtecke | Cut 2 x285..312 y50..63, Cut 3 x292..319 y6..21, Cut 10 x262..287 y0..10 | gemessen an der BSS-Dekodierung (Blau-Maske) |

## 6 Abnahmeplan

Da nichts gebaut wird, ist die Abnahme die **Gegenprobe der Messung** (reproduzierbar, alle Pfade im
Baum):

1. Hintergruende: `probe_r34n_g_bss <ROOM117.BSS> <praefix>` → 13 PPM; Schild in Cut 2/3/10 sichtbar.
2. Original, Cut 2, lang: `ds_rec_small.py <lamp_near.sav> rec.mkv 300 520 130 96 48` (vorher
   `tasklist | grep -i duckstation`), dann `rec_analyse.py klein rec.mkv 96 48` → erwartet
   "Bilder mit Aenderung gegen das vorige: 0".
3. Original, ganzer Vorspann: `ds_rec_fb0.py <orig_intro_late.sav> rec.mkv 150`, dann
   `rec_analyse.py voll rec.mkv <praefix>` → Schild-Laeufe mit min == max.
4. Savestates: `ds_sign_eval.py <ordner-mit-cap_*.sav>` (RAM-Kopie == beide Bildspeicher) und
   `ss_ot_walk.py <sav> [--box …]` → keine Zeile "<== SCHILD".
5. Port: echte exe mit `RE15_FRAMEDUMP` (Abschnitt 2), Schildausschnitt gegen BSS → max |Δ| = 0.

Gegenprobe, dass die Messkette ein Blinken ueberhaupt saehe: dieselbe Auswertung erkennt den
Kinobalken, der am Ende des Vorspanns ueber dem Cut-3-Schild ausblendet (Original F2751-2763, Port
F1769-1782) — eine Helligkeitsaenderung genau in diesem Rechteck wird also gemessen, nicht verschluckt.

## 7 Risiken, Softlocks, Wechselwirkungen

* **Keine Code-Aenderung → keine Wechselwirkung** mit Spur A-F und den r34g-Baeumen, kein Softlock.
* ⛔ **DuckStation-Fortsetzungsstand ueberschrieben.** `SaveStateOnExit=true`: jeder meiner Laeufe
  (02:52-03:28 Uhr) hat beim Schliessen `C:\Users\mjoedicke\AppData\Local\DuckStation\savestates\
  HASH-957757946319438E_resume.sav` neu geschrieben. Der vorherige Stand (00:06:12 Uhr, 1606754 B —
  vom Nutzer oder von der Granaten-Sitzung) ist damit **verloren**; eine Kopie gab es nicht. Nummerierte
  Slots (_1.._9) sind unberuehrt. Kuenftig vor jedem DuckStation-Lauf die resume-Datei sichern.
* DuckStation-Einstellungen wurden **nicht** veraendert (ShowVRAM/Software-Renderer so vorgefunden).
* Die Sonde `probe_r34n_g_bss` hat kein add_test und keine Abhaengigkeit ausser re15_engine — sie kann
  keinen fremden Bau brechen.

## 8 Offene Punkte

1. **Beobachtung des Nutzers (Messfrage, keine Wahlfrage).** Ich habe das Original in allen 13 Cuts,
   im ganzen Vorspann und 5 Minuten im Freilauf gemessen, dazu 19 Savestates und 18 Zeichenlisten —
   nirgends blinkt die Schrift. Der naechste Weg ist die Stelle, an der der Nutzer es gesehen hat:
   *"In welchem Bild (Screenshot oder kurzes Video mit Zeitmarke) blinkt die Schrift, und mit welchem
   Emulator/Renderer und welchem Disk-Abbild?"* Mit einer Zeitmarke laesst sich genau dieser Moment
   nachstellen und messen.
2. Nicht gemessen: DuckStation **Hardware-Renderer** (bewusst nicht umgestellt, um die Einstellungen
   des Nutzers nachts nicht zu veraendern) und ein **laufendes ePSXe/GPUCORE** (nur Standbilder
   geprueft, siehe 3.3). Ein dort sichtbares Blinken waere ein Emulator-Effekt, kein
   Original-Verhalten — der Port-Befund aendert sich dadurch nicht. Messweg, falls der Nutzer ePSXe
   benutzt: ePSXe-Stand `SLU__096.08.001` (Cut 10) laden, 30 s aufnehmen, `rec_analyse.py`.
3. ROOM1171 (Elza) nur statisch geprueft: gleiche BSS (ROOM117.BSS), gleicher Upload-Pfad
   FUN_80043870, RDT+0x4C = 0, SCD ohne Hintergrund-Opcodes. Kein Savestate in ROOM1171 vorhanden.
4. Nebenbeobachtung, **nicht Teil dieses Auftrags und nicht belegt**: das Ausblenden des oberen
   Kinobalkens am Vorspann-Ende laeuft im Port in 13 gleichmaessigen Stufen (F1769-1782); die
   Original-Aufnahme zeigt ~6 Stufen ueber 12 Aufnahmebilder (gdigrab ist nicht bildsynchron). Das
   braeuchte eine bildgenaue Messung (FrameAdvance), bevor daraus ein Befund wird.

## 9 Umsetzung (Stufe BAU, 2026-09-30)

Status: IN ARBEIT (Geruest angelegt, Abschnitte folgen).

### 9.1 Auflagen der Gegenpruefung (abgehakt / begruendet abgelehnt)

(folgt)

### 9.2 Dateien

(folgt)

### 9.3 Commits

(folgt)

### 9.4 Suite

(folgt)

### 9.5 Eigene Abnahme an der echten exe (Bilder)

(folgt)

### 9.6 Abweichungen vom Plan (mit Grund)

(folgt)

### 9.7 Offene Punkte

(folgt)
