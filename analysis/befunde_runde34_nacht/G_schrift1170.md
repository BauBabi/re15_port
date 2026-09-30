# Spur G — ROOM1170: blinkende Gebaeudeschrift im Hintergrund

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code). Zweig `r34n/schrift1170`.
Stand: Ermittlung abgeschlossen (2026-09-30, 02:30-03:45).

## 0 Kurzfassung

* **Lesart:** "die Schrift des Gebaeudes im Hintergrund" = das Leuchtschild **MAGAZINE CLUB** am Haus
  gegenueber (Hintergrund-Cuts 2, 3, 10). Es ist die einzige Schrift auf einem Gebaeude in allen 13
  Hintergruenden von ROOM1170 (Zensus Abschnitt 1).
* **Befund Original: die Schrift blinkt NICHT.** Gemessen am laufenden Original (MZD-Disk, identische
  RE1.5-EXE, DuckStation Software-Renderer 1x):
  * Cut 2, 300 s mit 60 Bildern/s, Ausschnitt um das Schild: **18000 Bilder, 0 Bilder mit irgendeiner
    Aenderung** gegen das vorige (`rec_analyse.py klein`).
  * gesamter Vorspann ab der Montage (150 s, 30 Bilder/s, alle Cuts 7/0/2/1/4/6/3) + 58 s Freilauf
    Cut 3: je Kamera-Abschnitt aendern sich nur 3D-Figuren, Hubschrauber, Untertitel und die
    Blenden; das Schild ist in Cut 2 in 740 Bildern pixelgleich (blau 353, L 134,0), in Cut 3 nach
    dem Ausblenden des Kinobalkens 1737 Bilder pixelgleich (L 139,2).
  * Cuts 5, 8, 9, 10, 11, 12 per RAM-Patch erzwungen (je 13-28 s): **0 Pixel mit Hub > 40**.
  * 18 Savestates in Cut 2/3 (Vcount 8049..15369): **kein GPU-Primitiv** in den drei OTs schneidet das
    Schild; in 19 Savestates ist die RAM-Hintergrundkopie 0x80198000 gleich der BSS-Dekodierung
    (max |Δ| 21/23 = Rundung Hardware- gegen Software-MDEC) und im Schildbereich gleich beiden
    Bildspeichern.
* **Mechanismus: gibt es nicht.** Der Hintergrund wird je Bild unveraendert aus der RAM-Kopie
  hochgeladen (`jal 0x80043870` @0x8002157c → `LoadImage` @0x800438b0, RECT {0, y, 0x140, 0xF0});
  die Kopie wird nur beim Cut-Wechsel geschrieben (MDEC `jal 0x80053a8c` @0x80021e34, `StoreImage`
  `jal 0x80068cec` @0x80021e44). ROOM1170 hat keinen Raum-ESP (RDT+0x4C = 0), keine SCD-Schleife auf
  Hintergrund/Objekte, und keines der 6 Objektmodelle ist ein Schild.
* **Port:** zeigt das Schild in Cut 2/3/10 pixelgleich zur BSS-Dekodierung (max |Δ| 0, 64 gemessene
  Bilder) — also genau wie das Original.
* **Ergebnis: kein Umbau.** Nach der Regel "Ist das im Original nicht so, ist der Port richtig"
  (memory reai-v2-original-oder-nicht) ist der Port hier bereits byte-true. Offen bleibt allein die
  Beobachtung des Nutzers — Abschnitt 8 formuliert die Messfrage (keine Wahlfrage).
* ⛔ Nebenwirkung: meine DuckStation-Laeufe haben `HASH-957757946319438E_resume.sav` (Stand 00:06
  Uhr) ueberschrieben (Abschnitt 7).

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
| Leuchtschild **"MAGAZINE CLUB"** (weisse Schrift auf blauem Grund, Haus gegenueber) | 2, 3, 10 | Cut 2 x285..312 y50..63; Cut 3 x292..319 y6..21; Cut 10 x262..287 y0..10 (Blau-Maske b>150, b>r+60, b>g+40 auf der BSS-Dekodierung) | `G02_schild_cut02_03_10.png` |
| Plakatwand + senkrechtes Band (".. Fashion / Men / Clothing / Furniture") | 6 (Himmel beim Abflug) | Plakat x97..150 y195..235; Band x248..272 y140..215 | `G01` |
| gruenes Notausgangs-Piktogramm ueber der Tuer | 8 | x230..275 y5..30 | `G09` |

Sonst keine Schrift: Cut 0/1/4 zeigen Fassaden nur mit Fenstern (vergroessert geprueft); das Schild am
Tor (Cut 12) hat der Nutzer selbst als "schon im Original unleserlich" eingeordnet (memory
reai-v2-tor-1170-tuermodell).

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

**Messung an der echten exe** (`re15_pc_g.exe` = Kopie von `re15_pc.exe` dieses Baums, Bau c274a66f;
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_DEBUG_JUMP=1170@240 RE15_WINDOW_SCALE=3 RE15_CUT_SYNC_LOG=cutsync.log RE15_FRAMEDUMP=<a>-<b>/<s>:…`):

| Lauf | Bilder | Ergebnis Schild-Ausschnitt gegen BSS-Dekodierung |
|---|---|---|
| Vorspann F400-1500/20 | 36 Bilder Cut 2 (F480-1380) | blau 356, L 130,4, **max \|Port − BSS\| = 0** in allen |
| Freilauf F1800-2600/25 | 33 Bilder Cut 3 | blau 427, L 135,5, **max \|Δ\| = 0** in allen |
| RE15_FORCE_CUT=10, F1800 | Cut 10 | Schild oben sichtbar, Bild = Original-Cut-10 (`G10`) |
| F1760-1800/1 | Ende Vorspann → Cut 3 | Kinobalken blendet in 13 Bildern aus (F1769 L 8,3 … F1782 L 135,5), danach konstant |

Tabelle: `G_belege/G14_port_schild_stats.txt`. Bilder: `G03`/`G04` (Cut 2 Original gegen Port, gleiche
Szene "Is there no one else left inside?"), `G08` (Port-Vorspann), `G10` (Cut 10 Original/Port, Cut 3
Port).

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
(FUN_800264e8), Statusbild/Karte (FUN_80049a5c, FUN_8004d96c), Waffen-Nachladen beim Raumaufbau
(@0x800466c8). **Kein Pfad veraendert die Hintergrundkopie oder den Upload waehrend des Spiels
periodisch.** Alles, was ueber dem Hintergrund liegt, muesste als GPU-Primitiv in einer der drei OTs
stehen (DrawOTag @0x800215bc/@0x800215d0/@0x800215e4 auf 0x800ac714/0x800ab6d4/0x800aa6b4 + buf*Stride).

### 3.2 Raumdaten ROOM1170 (Auslieferungsstand = MZD-Disk)

| Pruefung | Ergebnis |
|---|---|
| RDT-Header | nSprite 0, nCut 13, nOmodel 6; Effekt-Sektion **RDT+0x4C = 0** (kein Raum-ESP), ESP-TIM 0 |
| SCD (scd_dump_room.py) | main00 + 16 Subs: Tueren/AOTs, Vorspann sub02/sub11/sub14, Hubschrauber-Bewegung sub04-08/12/13 (Speed_set/Add_speed auf Objekte 2-4), Warteschleifen sub09/10 auf Flag (5,32)/(5,33). **Kein Opcode wirkt auf Hintergrund, Schild oder Licht periodisch.** ROOM1171 ebenso. |
| Objektmodelle (RDT+0x30, TIMs dekodiert) | obj0/1 = Kiste ("ASH tex"), obj2-4 = Hubschrauber (Rumpf/Rotor/Heckrotor), obj5 = Pilot — **kein Schild-Modell** |
| sprite.pri (Kamera +0x1C) | Masken nur in Cut 1/2/4/5; Atlanten zeigen rote Lampe, Kiste, Gelaender — **kein Schild**. Cut 2: 1 Index bei (135,10), 3 Quadrate 16x16 bei (128,128)/(128,144)/(144,128) → Bild x263..295 y138..170 = rote Lampe, weit weg vom Schild (x285..312 y50..63) |
| Kamera-Eintraege (RDT+0x24, 13 x 0x20) | Wort +0 ist `end_flg` (nur Cut 12 = 1, BioRdt global.h RCUT) — kein Bild-Modus |
| BSS-Chunks | Chunk endet jeweils bei L (Stage-Tabelle @STAGE1.BIN+0x1EAE4, sll 5 / sll 1 @0x80021d50/4c); keine zweite MDEC-Aufnahme im Chunk, keine versteckten Bilder |
| MZD-Disk gegen Auslieferungsstand | RDT im Savestate-RAM (DAT_800ac778 = 0x801219b0) byte-gleich (alle 22 Sektionszeiger, SCD 0x11F4..0x1920); EXE-Code gleich bis auf einen Datei-Lade-Haken `j 0x800c02c0` @0x80013b7c (Anzeige/Protokoll des Dateinamens, springt nach `j 0x80013b84` zurueck). Header-Byte 0 = 3 im RAM schreibt FUN_800392d4 zur Laufzeit (Maskenzahl des Cuts). |

### 3.3 Dynamische Messung am Original (DuckStation, MZD-Disk, Software-Renderer 1x, TrueColor)

Werkzeuge: `re15_port/tools/r34n_g/` (Abschnitt 6). DuckStation zeigte mit `[Debug] ShowVRAM=true` das
ganze VRAM; aufgenommen wurde Bildspeicher 0 per ffmpeg-gdigrab.

| Messung | Umfang | Ergebnis |
|---|---|---|
| rec1: Cut 2, Freilauf (Start lamp_near.sav) | 25 s, ~28 Bilder/s, Schild-Ausschnitt | alle 718 Bilder gleich; Aenderungskarte des ganzen Bildes zeigt nur Leon (`G06`) |
| **rec3: Cut 2, Freilauf** | **300 s, 60 Bilder/s, 96x48 um das Schild** | **18000 Bilder, 0 Aenderungen**, Blau-Pixel 1269..1269 |
| rec2: Pilot-Dialog → Abflug → Freilauf (Start orig_1170_gp.sav) | 70 s, ~15 Bilder/s | Schild Cut 2 konstant (109,8/900), Cut 3 konstant (619 Bilder), Plakat/Band Cut 6 konstant (26,4 / 27,2) |
| **rec4: ganzer Vorspann ab Montage ROOM1240** (orig_intro_late.sav) | 150 s, 30 Bilder/s | 17 Kamera-Abschnitte, Aenderungen nur an 3D/Untertitel/Blenden; Schild Cut 2: 740 Bilder blau 353, L 134,0 konstant; Cut 3: Kinobalken blendet F2751-2763 aus, danach 1737 Bilder L 139,2 konstant; Cut 6: Plakat 26,0, Band 26,6 konstant (`G07`) |
| erzwungene Cuts 5/8/9/10/11/12 (RAM-Patch 0x800aca3c:=0x100 = Zonen-Scan aus, 0x800afbb5:=Cut) | je 13-28 s, 30 Bilder/s | **0 Pixel mit Hub > 40**; Schild Cut 10: 450 Bilder blau 247, L 134,0 konstant; Notausgang Cut 8 konstant (`G09`) |
| 19 Savestates Cut 1/2/3 (stage_saves, Vcount 8049..15369), davon 18 mit OT-Walk | RAM + VRAM | RAM-Kopie 0x80198000 == BSS-Dekodierung (max \|Δ\| 21/23, gleich in allen); Schild in beiden Bildspeichern == RAM-Kopie; OT-Walk (`ss_ot_walk.py`): 0 Primitive schneiden das Schild (`G11`, `G12`) |

Auswertung: `G_belege/G13_original_aufnahmen_auswertung.txt`.

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
   im ganzen Vorspann und 5 Minuten im Freilauf gemessen, dazu 16 Savestates und die Zeichenlisten —
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
