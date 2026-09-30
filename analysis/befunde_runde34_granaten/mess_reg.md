# Runde 34 (Granaten) — MESSER "Regressionen, Item-Debug, Zielen"

Stand: 2026-09-30, Integrationsbaum `.claude/worktrees/r34g_int`, Zweig `r34g/integration` HEAD ef1c6f94.
Integrations-exe: `re15_port/build_r34_int/platform/pc/re15_pc.exe` -> Messkopie `re15_pc_reg.exe` (byte-gleich, selbes Verzeichnis).
master-exe (v0.8.19): Hauptbaum `re15_port/build/platform/pc/re15_pc.exe` -> Messkopie (anderer Name).
Ausgaben: `build/r34g_mess_reg/` (unversioniert). Belege: `analysis/befunde_runde34_granaten/mess_reg_belege/`.
NUR MESSEN — kein Code, kein Commit (der Orchestrator committet dieses Dossier).

## 0. Fortschritt

| Punkt | Stand |
|---|---|
| Vorbereitung (Messkopien, Werkzeug) | erledigt |
| (1) Bestandseffekte master vs. Integration | erledigt — keine unerklaerte Abweichung (C1/C2/C3-A7/W7/W8/A2/N1.4) |
| (2) Drehen im Zielen 24/48 + Auto-Nachfuehrung H-1 | erledigt — Raten/Vorzeichen/H-1 bestaetigt; 5 Abweichungen (M-Z1..M-Z5) |
| (3) Item-Debug-Weg (W5, A M-2) | erledigt — alles wie Soll, inkl. R2/L2/Kreis in der exe |
| (4) Hebetisch ROOM1150 Granate aufnehmen + werfen | erledigt — laeuft; Befund M-H1 (Anker haengt am Zeichnen) |
| (5) Speichern/Laden mit Granaten | erledigt — wie Soll |
| (6) Framerate/Ruckler Explosion + Bodenflammen | erledigt — kein Ruckler bei 30 Bildern/s |

## W. Werkzeug und Messweg

* Messkopien (md5 `build/r34g_mess_reg/exe_md5.txt`): Integration `re15_port/build_r34_int/platform/pc/re15_pc_reg.exe`
  = f99f6f2c7f5e258ab9794d30b59e96b2 (= re15_pc.exe ef1c6f94); master `re15_port/build_r34_int/platform/pc/mst_reg/re15_pc_mreg.exe`
  = 102290a6aeece3b58773ff4c572dbf94 (= Hauptbaum-exe, gebaut 2026-09-29 19:46 = v0.8.19). Die master-Kopie liegt NICHT im
  Hauptbaum (dort schreibt die exe `befund.log` neben sich = Nutzerdatei); beide exe loesen dieselbe Asset-Wurzel auf
  (`[asset] cd-root: .../r34g_int/re15_port/shared_assets/PSX`, debug.log beider Laeufe). Einziger Asset-Unterschied
  master..Integration: `shared_assets/RE2/CORE00.ESP` + `TEX.TIM` (neu; die master-exe liest sie nicht). Beide Messkopien sind
  nach Abschluss geloescht (`re15_pc_reg.exe`, `mst_reg/`); `re15_pc_geg.exe` im selben Ordner gehoert einem anderen Messer.
* Werkzeug (Kopie zum Einchecken): `analysis/befunde_runde34_granaten/mess_reg_werkzeug/` (Laufordner-Ausgaben bleiben
  unversioniert unter `build/r34g_mess_reg/laeufe/`, Framedumps nach der Auswertung geloescht).
* Lauf-Skript `lauf_reg.sh <int|mst> <marke>` (Vorlage integration_werkzeug/lauf.sh), `lauf_save.sh` (+ Karte), Auswertung: `ppmdiff.py` (Pixel je Bild), `sheet2.py` (Kontaktbogen master | Integration | Diff),
  `fxframes.py` (FX-Log in Bilder zerlegen; die master-exe schreibt kein `F=` -> Bildgrenze = Ankereffekt), `footprint.py`
  (Diff-Pixel ausserhalb der Effekt-Fussabdruecke).
* Disassembly selbst: `mess_reg_werkzeug/disasm/*.txt` (re15_disasm.py, info/Re1.5/PSX.EXE); Tastatur-Einspeisung
  `tasten_inject.py` (PostMessage an das eigene Fenster), Bildzeiten `bildzeit.py`, Drehung `dreh.py`.

## 1. Bestandseffekte master (v0.8.19) gegen Integration (ef1c6f94)

### 1.1 Pistole (Id 3) in ROOM1140 — Muendung, Zweitblitz, Rauch, Huelse, Blut am Zombie

Lauf `pistole1140` (beide exe identisch: `RE15_DEBUG_JUMP=1140@250`, `RE15_GIVE=3:15 RE15_EQUIP=3`, Skript
`W1,M1,MA0.2,M2.5,W4` ab Spielbild 300 -> Schuss F360, Framedump 356-400 x3, RE2-KI Standard). Spawn-Folge im Waffen-Log
beider exe gleich: F360 `SPAWN id=2 sub=0 / id=3 sub=0 / id=4 sub=0 / id=2 sub=4`, Blut id 0 an (-1800,-1815,-19600).

* Pixel (`ppmdiff.py`): 0 in F356-359 und F394-400; Summe 52273 Pixel in F360-F393.
* **Ausserhalb der Effekt-Fussabdruecke** (Vereinigung beider FX-Logs, Bild F-1..F+1, Box 3 x Spritebreite): 0 Pixel in
  jedem Bild AUSSER F361 (3859 Pixel auf Leon und dem liegenden Zombie) -> das ist das Ein-Bild-Licht des Licht-Latches
  (Routine 9 der Muendung setzt 0x800b5358 `sb v0,21336(at)` @0x80017694, Leser nach dem ESP-Takt @0x8001ce5c-0x8001d084,
  Loeschung @0x8001d1b4 = BAUPLAN E11 / C3 + A7). Jeder Schuss hellt jetzt EIN Bild lang Leon und die Figuren im Bild auf
  (`e1_pistole_latch_F361.png`). **Zuordnung: C3/A7 (neu, belegt)** — kein Mangel.
* **Geometrie je Bild gleich** (FX-Log, master-Bilder ueber die Huelse als Anker zerlegt):
  * Blut id 0 (25 Partikel, 4 Lagen): Lage und Groesse je Bild identisch, Zelle in der Integration +1 (F365 c0 6 gegen 5),
    Ende ein Bild frueher (Integration letztes Bild F370, master F371); F362 zeigt die Integration schon den S32-Satz, master
    noch S16. -> **C1** (ESP-Takt hinter dem Spielschritt, @0x8001ce0c < @0x8001ce2c: der Zustand steht ein Bild frueher).
  * Huelse id 4: Bildschirmlage F362-F392 identisch; F360/F361 Integration `w16=43` (Zeile 0 w/h 1 bei Flags 0x63 = Quad
    von hoechstens 1 Pixel, master `w16=176128` = volle Huelse schon im Spawnbild) -> **C2** (defW/defH aus der Zeile,
    @0x800535d0/@0x800535e0); Zelle +1 und Ende ein Bild frueher (F392 gegen F393) -> **C1**.
  * Muendung id 2 sub 0 / Rauch id 3 (Plaetze UNTER dem Anker, deshalb master-Bild = Ankerbild + 1): Rauch Integration F
    == master F+1 in JEDEM Bild F360-F374 (n/Zelle/w16 gleich); master zeichnet im Spawnbild F360 den UNGETAKTETEN Rauch
    (`S=8 n=1 c0=0 w16=131072`), die Integration schon den getakteten (`S16 n4 c0 8`) -> **C1**. Muendung: Zellen der Integration
    ein Bild voraus, Lage je Bild wie master (folgt dem Waffenknochen desselben Bilds). Zweitblitz id 2 sub 4: Integration ab
    F360 (Zelle 7/9/12 in F360/361/362), master ab F361 (7/9/12 in F361/362/363) -> **C1**; Palette 490 statt Muendungsblatt
    -> **C2** (Routine 10 CLUT += row[0x1e]<<6 @0x800176e0-fc).
* **Helligkeit**: Huelse (deckend, ohne ABE) F376-F387, Pixel mit Abweichung und master >= 20: Median Integration/master
  = **2.000** (Soll 255/128 = 1.992) -> **W8** (Primitivfarbe 0x80 = x1.0, FUN_800537e4 @0x800537ec). Muendung, Rauch, Blut
  sichtbar heller (`e1_pistole_muendung_rauch_huelse.png`, `e1_pistole_blut_zombie.png`).
* Textur: die Streifen im master-Rauch (jede 4. Texelspalte) fehlen in der Integration -> **C2** (alte Blaetter ohne Bit 15,
  bau_c.md C2 "1269/4162/1269 abweichende Texel ... alle an Halbworten mit Bit 15").
* Ergebnis 1.1: **keine unerklaerte Abweichung** (C1/C2/C3-A7/W8).

* Hinweis zur Zerlegung der master-Logs: die master-exe schreibt keine Bildnummer; Bildgrenze = Ankereffekt (Huelse). Plaetze
  mit KLEINEREM Index als der Anker (Muendung, Rauch) stehen im Log VOR dem Anker ihres Bilds -> wahres Bild = Ankerbild + 1
  (in allen Vergleichen oben so gerechnet; Werkzeug `fxframes.py`).

### 1.2 Schrot (Id 8 Remington M870) in ROOM1140

Lauf `schrot1140` (`RE15_GIVE=8:7 RE15_EQUIP=8`, sonst wie 1.1, Framedump 356-410). Spawn-Folge beider exe identisch (F360 `id=2
sub=3` x5 Stroeme, `id=3`, `id=4 sub=3`, `id=0` x3, `id=2 sub=5`; Blut-Nachschuebe F365/367/369/371/373/375).
* Ausserhalb der Fussabdruecke: nur F361 (2489 Pixel an Leon) = Licht-Latch -> **C3/A7**.
* Muendung sub 3: `w16` Integration 391680 / master 327680 = **1.1953 = 0x1320/0x1000** -> **C2** (Zeile +0x04/+0x06 = 0x1320,
  @0x800535d0/e0; bau_c.md "Muendung sub 3/6: 0x1320"). Lage je Bild gleich, Zellen ein Bild voraus -> C1.
* Zweitblitz sub 5 (das helle Quadrat, S24): Integration F361, master F362 (C1), Palette 490 (C2), doppelt hell (W8); die
  Streifen im master-Blatt fehlen (C2) — `e2_schrot_muendung_huelse.png`, F361/F362.
* Schrothuelse id 4 sub 3: master zeigt F360-F372 eine VOLLE Huelse (`w16=192512`) still an der Waffe; Integration dort
  `w16=47` = Quad <= 1 Pixel (Zeile 0 w/h 1 bei Flags 0x63) -> **C2** (Regel byte-true fuer JEDEN Platz, @0x800535d0/e0; bau_c.md
  nennt nur sub 0/7 — sub 3 hat dieselbe Halte-Zeile, gemessen). Flug F373-F409 lagegleich, Integration endet ein Bild
  frueher (F409 gegen F410) -> C1.
* Blut: Anzahl je Bild gleich bis auf die Todesbilder, die in der Integration je ein Bild frueher liegen (F374 68 gegen 94,
  master 71 erst F375; F376 40 / master 38 in F377; F378 8 / master 8 in F379; letztes Blut F385 gegen F386) -> **C1**.
* Rauch: Integration F == master F+1 (F360-F374, n/Zelle/w16 gleich) -> C1; Streifen weg -> C2; heller -> W8.
* Ergebnis 1.2: **keine unerklaerte Abweichung**.

### 1.3 MP (Id 12 Ingram M10, Dauerfeuer) in ROOM1140

Lauf `mp1140` (`RE15_GIVE=12:100 RE15_EQUIP=12`, Skript `W1,M1,MA0.6,M2.5,W4`, Framedump 356-420). Spawn-Folge beider exe
identisch (Schuesse F360/363/366/369/372/375, Huelsen F360/367/374, Blut sub 0/1).
* Ausserhalb der Fussabdruecke NUR die Bilder F361/364/367/370/373/376 (je 2675-3292 Pixel an Leon, x484..533) = je Schuss
  ein Bild Licht-Latch (Routine 9 der Muendung, @0x80017694) -> **C3/A7**. Sichtbar: im Dauerfeuer blitzen Leon und die
  Figuren jedes dritte Bild auf (`e3_mp_latch_takt.png`, F364/F367). Kein Mangel (E11 byte-true), aber fuer den Nutzer eine
  auffaellige neue Wirkung -> Release-Hinweis.
* Sonst nur Effekt-Fussabdruecke (C1/C2/W8 wie 1.1). Ergebnis 1.3: **keine unerklaerte Abweichung**.

### 1.4 Blut am Hund (RE2-KI) in ROOM11D0

Lauf `hund11d0` (`RE15_DEBUG_JUMP=11D0@250`, `RE15_SET_FLAG=3:152` wie W6, Pistole, Skript ab Spielbild 3 Dauer-R1 + 6 Schuesse,
Framedump 20-40). Blut-Spawns beider exe identisch (`F21 SPAWN id=0 sub=1 scale=0x1500 streams=4`, `F145` x9).
* Integration: `wpos` != Anker + xlat, z.B. F28 Anker (2810,-145,-17691), xlat (464,-48,56), `wpos` (2394,-187,-17679) — die
  Tropfen fliegen um die Spawn-Gier gedreht (Hund-FX-Spawner Gier = rot_y + off, `lh 118(s0)` @0x80105104; Weltlage
  RotMatrix(euler + (0,Gier,0))·xlat @0x8001a16c-1a0). master zeichnet Anker + xlat UNGEDREHT (fliegt Welt +x). -> **A2**
  (bau_a.md §6 Gegenpruefung H-2 "ihre gezeichnete Flugrichtung aendert sich").
* Sichtbarkeit: Ereignis F21 zeigt die Integration ab F27 drei Stroeme (Bild 7 der Zeile, Flags 0x03 -> 0x13), master nur
  EINEN Strom ab Bild 8 (andere Lage, xlat (-361,111,0)); Regions-Test jetzt auf `wpos` (C2, @0x80053314-30) -> andere Stroeme
  fallen in die Kamera-Region. Ereignis F145: master zeichnet im Spawnbild vier UNGETAKTETE Stroeme (auch den Halte-Strom 3 mit
  voller Groesse), die Integration ab F146 drei (Strom 3 in der Halte-Zeile Flags 0x61 = unsichtbar) -> **C1/C2**.
* Bild: `e4_hund_blut_richtung.png` (F27: Integration Blutklecks rechts unten, master leer; F33 master schwach an anderer
  Stelle).
* Ergebnis 1.4: Abweichungen = A2 (Richtung) + C1/C2 (Sichtbarkeit) + W8 (Helligkeit); **kein unerklaerter Rest**.
  Offen (nicht Integration): ob die RE2-Hunde-Blutrichtung im RE2-Original (FUN_8001d894 "RotY(a1)·lokal") dieselbe
  Drehrichtung hat wie RE1.5-RotMatrix, ist nicht Gegenstand dieser Messung.

### 1.5 ROOM1090 Feuer (Raumbank id 9/16 + CORE00 id 8, sub 3)

Lauf `r1090` (`RE15_DEBUG_JUMP=1090@250`, Framedump F1-F60, Cut 8).
* FX-Log: gleiche Zeilenfolge in beiden exe (1582 Zeilen je exe); master zeichnet in F6 zusaetzlich die sieben UNGETAKTETEN
  Spawn-Plaetze (`c0=0`), die Integration zeigt in F6 schon Bild 4 (`c0=4`); danach ist der Integrationszustand ein Bild voraus
  (master F7 == Integration F6) -> **C1**.
* Pixel: Abweichung NUR im Feuer-Ausschnitt (x58..122 y570..674 bei x3); Summe des Beitrags gegen das feuerlose Bild F5 in
  x40..140/y540..700: Integration/master = 1.157, dabei 15202 gesaettigte Kanaele (>= 254) in der Integration gegen 1 in
  master -> die Verdopplung (**W8**) laeuft im hellen Kern in die Saettigung; sichtbar weiss-gelber Kern statt dunkelrot
  (`e6_room1090_feuer.png`). master F7 zeigt einen dunklen Klecks (ungetakteter Spawn-Platz), die Integration nicht (C1).
* Ergebnis 1.5: C1 + W8, **kein unerklaerter Rest**.

### 1.6 ROOM11E0 Strom-Funke (Raumbank id 0x11, sub27-Schleife, Sleep 20)

Lauf `r11e0` (`RE15_DEBUG_JUMP=11E0@250`, `RE15_FORCE_CUT=5` — im Sprung-Cut 0 ist der Effekt gecullt, Cut 5 zeigt ihn; Suche
ueber Cut 1..8, nur Cut 5 mit `->`-Zeilen), Framedump F1-F60.
* Waffen-Log: master `F250 SPAWN id=17 sub=0 ... streams=-1` (Eintrittsbild, Bank noch leer), Integration `streams=1`; danach
  beide F24/F44 `streams=1`.
* Pixel: Integration zeigt den Funken schon in F2-F9 und F16/F18 (erste Schleifenperiode), master dort nichts; ab F25 zeigen
  beide denselben Funken (gleiche Lage je Bild), Integration heller (additiv, W8) — `e5_room11e0_funke_cut5.png`.
* Zuordnung: erste Periode = **N1.4 der Spur C** (Raum-Effektbank vor dem SCD-Eintrittstakt; `jal 0x80019354` @0x8003996c vor
  `jal 0x8003ef6c` @0x80039a00, bau_c.md NACHBESSERUNG N1.4), Helligkeit **W8**. Kein unerklaerter Rest.

### 1.7 ROOM20A0 Raumeffekt (id 6 sub 6, sub02-Schleife)

Lauf `r20a0` (`RE15_DEBUG_JUMP=20A0@250`, `RE15_FORCE_CUT=2` — im Sprung-Cut 0 hinter der Kamera (Near-Gate); Cuts 2..5
zeichnen), Framedump F1-F60.
* Waffen-Log: master die ersten drei Spawns `streams=-1` (N1.4), Integration alle `streams=1`.
* FX-Log Integration: je Bild bis zu 11 Plaetze mit `->` (sx 201..241, sy 146..152), `wpos` ab F2 gesetzt.
* **Pixel: 0 Abweichung in F1-F60** — der Effekt ist in Cut 2 in BEIDEN exe unsichtbar (dunkler Kanalboden, x3-Ausschnitt
  x560..780/y380..500 in beiden Bildern gleich, `r20a0_zoom.png` im Ausgabeordner). Keine Regression; ob er im Original an
  dieser Stelle sichtbar ist, ist nicht Gegenstand dieser Messung (Hinweis H-E1 unten).

### 1.9 Salve (Id 5 Beretta M93R, 3er-Salve) in ROOM1140 — W7-Pruefstelle "Salven-Huelse"

Lauf `salve1140` (`RE15_GIVE=5:15 RE15_EQUIP=5`, sonst wie 1.1). Spawns beider exe gleich (F360 `id=2/3/4 sub=2` + `sub=0`, F363/F366
Folgeschuesse).
* Ausserhalb der Fussabdruecke: F361/F364/F367 (je ~3700-3800 Pixel an Leon) = Licht-Latch je Schuss (C3/A7); F381/F382 master
  zeigt noch den gestreiften Rauch des 3. Schusses, die Integration nicht mehr (Rauch endet frueher, C1); F400 3 Pixel =
  master-Huelse ein Bild laenger (C1).
* Die Steuerplaetze `id 3 sub 2` / `id 4 sub 2` (Salven-Huelse, Routine 15 = Kind-Spawner) zeichnet master NUR im ungetakteten
  Spawnbild F360 als volles Sprite (`id=4 sub=2 ... frame=0 -> sx=154 sy=79 S=16 w16=172032`), die Integration nie (nach dem
  ersten Takt Flags ohne Bit 1; C1/C2, W7 "erster Satz 1 statt 0").
* Ergebnis 1.9: **kein unerklaerter Rest**.

### 1.8 Zusammenfassung (1)

| Effekt | Abweichung master -> Integration | Ursache (belegt) |
|---|---|---|
| alle RE1.5-ESP-Sprites | doppelt hell (Huelse Median 2.000; Feuer bis zur Saettigung) | W8 (@0x800537ec) |
| alle Zeilen-VM-Plaetze | Zustand ein Bild frueher (Start und Ende), master zeichnet das ungetaktete Spawnbild | C1 (@0x8001ce0c < @0x8001ce2c) |
| Muendung/Rauch/Huelse | Streifen (Bit-15-Texel) weg; Zweitblitz Palette 490; Schrot-Muendung x1.195; Huelsen-Halte-Zeile 1 Pixel | C2 (@0x800535d0/e0, @0x800176e0-fc, TEX.TIM) |
| jeder Schuss / HE | ein Bild Licht-Latch auf allen Figuren (MP: jedes 3. Bild) | C3/A7 (@0x80017694, @0x8001ce5c-0x8001d084) |
| Hund-Blut (RE2-KI) | Flugrichtung um die Spawn-Gier gedreht, andere sichtbare Stroeme | A2 (@0x8001a16c-1a0) + C2 (Regionstest auf wpos) |
| ROOM11E0 / ROOM20A0 | Eintrittseffekt schon in der ersten Periode | N1.4 (@0x8003996c < @0x80039a00) |
Unerklaerte Abweichung: **keine**.

## 2. Drehen im Zielen (A8) und Auto-Nachfuehrung (A H-1)

### 2.1 Messweg
ROOM1050 (Debug-Sprung, KEIN Gegner im Raum — Zustandslog ohne Akteure; ROOM1000/1060 ebenso), Leon (20600,12350) Gier 0,
Skript ab Spielbild 10: `W0.5,ML1.5,MLA0.1,ML1,MUL1,ML0.5,L0.5,W1,MR1.5,MRA0.1,MR1,MDR1,MR0.5,R0.5,W1` (R1+LINKS heben/halten,
Abzug, Halten+OBEN, R1 los mit LINKS, dann dasselbe RECHTS/UNTEN). Je Bild d = rot(F) - rot(F-1) aus `PL(...,rot=..)` des
Zustandslogs, gruppiert nach Tasten und Zielclip `ac` (Werkzeug `dreh.py`). Waffen 1, 3, 8, 9, 12, 13, 14, 15 je in beiden exe
(`laeufe/<exe>_dreh_w<id>`), Nachladen `reload_w3` (`RE15_GIVE=3:1,21:30`).

### 2.2 Ergebnis Integration (alle Waffen gleich, Pistole als Beispiel `int_dreh_w3`)
| Phase (ac) | Integration d/Bild | master d/Bild | Original (selbst disassembliert) |
|---|---|---|---|
| Heben (6), LINKS | -24 (F25-34) | -72 | Sub 0: `subu` Byte0 der Waffenzeile @0x80033040 (Tabelle `addiu at,at,16528` = 0x80074090 @0x80033028) = -24 |
| Halten (8), LINKS | -48 (F35-69) | -48 | Sub 1: `subu` Byte1 (`addiu at,at,16529` @0x800333c8) = -48 |
| Abzug/Rueckstoss (7) | -24 (F71-91) | -72 | Sub 2: Byte1 `srl v0,v0,1` @0x800335a4 = -24 |
| Halten + OBEN (10) + LINKS | -48 (F103-132) | **0** | Sub 1 dreht, sobald die Hoehe steht (Sprung auf 0x80033394 ueber @0x80033290-2dc) = -48 |
| R1 los, Senken (6) | -24 (F148-157), dann -96 Laufdrehung | -72 | Sub 3: `addiu v0,v0,-24` @0x80033cf0 |
| Nachladen (13) | -24 (F95-124 in `reload_w3`) | -72 | Sub 4: `addiu v0,v0,-24` @0x80033e00 |
| RECHTS | jeweils + (+24/+48/+24) | + | `addu` (@0x8003308c, @0x8003342c, @0x800335f4, @0x80033d14, @0x80033e24) |
* Werte je Waffe (Tabelle `re15_disasm.py read 0x80074090 ...`, 5 Byte je Waffe ab Id 1): Id 1 `18 30 00 01 01`, Id 2 `00 00 00 00
  00`, Id 3/4 `18 30 07 01 01`, Id 5-7 `18 30 0a 01 01`, Id 8 `18 30 0a 00 01`, Id 9/10/11 `18 30 0a 01 00`, Id 12/13 `18 30 0a 01
  01`, **Id 14..19 `00 00 00 00 00`** (@0x800740d1-0x800740e9). Dauerfeuer-Maschine (Waffe 12/14/19 laut Verteiler
  0x80074030[w] @0x80032e74) liest die Tabelle NICHT: Konstanten -24/+24 Heben @0x80034160/84, -48/+48 Halten @0x800344b4/d8,
  -24/+24 Feuerschleife @0x80034618/3c.
* Gemessen Integration: Id 1 (Messer), 3, 8, 9, 12, 13, 14, 15 je -24/-48/-24/+24/+48/+24 wie oben; kein `& 0xfff` mehr (rot
  laeuft negativ, z.B. -25, -73 in `track_w3_b`; master springt auf 4071/4023). -> A8 fuer Pistole/Schrot/Granate/SPAS/MP/
  Flammenwerfer **bestaetigt** (Rate, Vorzeichen, Wertebereich).

### 2.3 Abweichungen vom Original, die die Messung zeigt (Integration)
* **M-Z1 Messer-Hieb dreht** (Id 1): Integration dreht im Hieb (ac 7) -24 je Bild (F71-94) und danach -48 (F95-102, Clip 7 noch
  aktiv), master -72/-48. Original: Nahkampf-Sub 2 (Verteiler `addiu at,at,16740` = 0x80074164 @0x80034ec0, Eintrag [2] =
  0x80035314) = Hieb `jal 0x80011f50` @0x800353cc, `anim_set` @0x800353e8, Ende `sh v0(=1),aca5a` @0x80035400 — **ohne
  Schreibzugriff auf 0x800acabe** (Sub 2 0x80035314-0x8003541c, `disasm/melee_sub2.txt`) -> im Original dreht Leon waehrend
  des Hiebs NICHT. A8 ("wirkt fuer ALLE Waffen ... Abzug 24") trifft fuer das Messer nicht zu.
* **M-Z2 Uebergangsbilder im Halten** (alle Schusswaffen): Original-Sub 1 verlaesst die Funktion VOR dem Drehcode (0x80033394)
  im Bild, in dem (a) R1 losgelassen wird (`ori v0,zero,0x3` / `sh aca5a` / `j 0x80033450` @0x800331f8-0x80033204), (b) die Hoehe
  wechselt (OBEN @0x80033228-3c, UNTEN @0x80033270-88, zurueck MITTE @0x800332bc-d4, jeweils `j 0x80033450`), (c) der Abzug
  beginnt (`sh v0(=2),aca5a` / `j 0x80033450` @0x80033324-2c). Die Integration dreht in genau diesen Bildern: F148 (R1 los) -24,
  F103/F133 (OBEN an/aus) -48, F70 (Abzug) -48 (`int_dreh_w3`). Je Uebergang 24 bzw. 48 Einheiten zu viel. (Nachladen-Start
  springt dagegen in den Drehcode `j 0x80033394` — dort dreht auch das Original; Integration F94 -48 = richtig.)
* **M-Z3 Reihenfolge Nachfuehrung/Handdrehung beim Heben**: Original Sub 0: erst `jal 0x8001a8f8` @0x80032fec (Nachfuehrung),
  DANACH die Pad-Drehung @0x80033000-94. Integration (player_common.c): erst Pad-Drehung, dann Nachfuehrung. Gemessen
  `int_track_w3_b` (ROOM1140, Ziel-Peilung 215, R1+LINKS): rot 0, -24, 152, **215, 215, ...** (die Handdrehung verpufft im
  Einrastbereich), danach Halten 167, 119, ... Nach der Original-Reihenfolge rastet das Ziel ein und DANACH kommen -24 dazu
  (Soll 191 im Heben, dann 143, 95, ...) -> 24 Einheiten Unterschied nach jedem Heben mit gehaltener Richtungstaste und Ziel.
  (Rechnung aus @0x8001a958-9b0: a1 = (t - rot + 200) & 0xfff < 400 -> rot := t; dann rot -= Byte0.)
* **M-Z4 Nachfuehrschritt der Dauerfeuer-Waffen**: Original-Dauerfeuer-Sub 0 `ori a1,zero,0xc0` @0x80034128 -> `jal 0x8001a8f8`
  @0x80034134 = **192** je Bild (Radius `ori a0,zero,0x7530` @0x800340f4). Integration (`slew = s_aim_melee ? 0xc0 : 0xc8`)
  nimmt 200 fuer Id 12/14: gemessen `int_track_w12_c` Schritt -200 (2361, 2161, 1961, ...). Soll -192. Die Nachbesserung H-1 zitiert
  den Aufrufer @0x80034134, uebernimmt aber seinen Schritt nicht.
* **M-Z5 Granatwerfer (Id 15)**: Original liest Waffenzeile 15 @0x800740d6 = `00 00 00 00 00` -> Heben/Halten/Abzug drehen um 0
  (Senken/Nachladen fest 24). Integration dreht 24/48/24 (`int_dreh_w15`). Die Zeilen 14..19 sind im Auslieferungsstand alle 0
  (unfertige Waffen der Beta) — nach der Regel Beta -> Retail waere RE2 das Ziel; der RE2-Wert ist NICHT gemessen. Einordnung
  offen (Hinweis).

### 2.4 Auto-Nachfuehrung H-1 (FUN_8001a8f8) — bestaetigt
* Band-Fall `track_w3_c` (Leon Gier 2361, Ziel-Peilung 215: (t - rot) & 0xfff = 1950 liegt im Band [0x801-200, 0x7ff]):
  Integration 2361 -> 2161 -> 1961 -> ... -> 561 (-200 je Bild bis Hebe-Ende), master 2361 -> 2561 -> ... -> 3961 -> 65 (+200).
  Original laut @0x8001a960-9b0 (a1 = 2150 > 0x800 -> `subu v0,a2,s1` / `sh` @0x8001a988, kein `+2s`): -200. **Integration =
  Original, master falsch** -> H-1 bestaetigt.
* Schritt ueber 0 ohne Maske: `track_w3_b` Integration -25, -73, ... (s16), master 4071, 4023 -> H-1 (`sh` ohne `andi 0xfff`).
* Einrasten: `track_w3_a` (Gier 0, t = 215): 0 -> 200 -> 215 in beiden exe (a1 = 215 < 400 -> rot := t @0x8001a984).

## 3. Item-Debug-Weg im echten Spiel (A10, W5, A M-2)

Alle Laeufe ROOM1140 (Debug-Sprung, Leon am Tuer-Sprungpunkt, kein Fresser wach), Skript ab Spielbild 260, Fenster x2.
Belege: `e7_itemdebug_d1_master_vs_int.png`, `e8_itemdebug_d4_r2_l2_kreis.png`, `e9_itemdebug_d3_reset_M2.png`.

* **D1** (beide exe; `S0.1,W2,A0.1,W1,E0.1,W0.3` + 9x`M0.1,W0.2` + `Q0.1` + `M0.1` + 2x`T0.1` + `X0.1,W1,S0.1,W3` + Wurf):
  * Integration: SELECT (F356) -> Platz 0 = Id 0 / Menge 255 (Zustandslog `mg=255` ab F360, Bild F364 leeres Feld "255");
    9x R1 -> "Hand Grenade 255" (F444); L1 -> "Remington M870 255" (F456); R1 -> "Hand Grenade 255" (F468); DREIECK -> Menge **0**
    (255 + 1 im Byte, F480), DREIECK -> 1 (F492); Kreuz, Schliessen F530 -> `[equip] W-bank -> W09`, `mg=1`; Wurf A 641 -> Spawn
    F665 (= A + 24, TIEF), `mg=0`. **Kein** `[debug-menu] OPEN` nach dem Sprung (W5 bestaetigt).
  * master: dieselbe SELECT-Flanke oeffnet `[debug-menu] OPEN (frame 356)` ("DEBUG MENU / UTILITY MENU / JUMP 114 BRIEFING
    ROOM" ueber dem Inventar), R1/L1 blaettern dort die JUMP-Ziele (F480 "JUMP 300 F.A.C.T. TUNNEL"), das Item bleibt "Combat
    Knife", kein Ausruesten. -> Regression behoben, Item-Debug neu.
  * Original-Beleg (selbst): Zustand 3 prueft die rohe Flanke 0x800ac762 (`addu v1,a1,zero` @0x8004a188) in der Reihenfolge
    R1 `andi 0x8` @0x8004a238 (+1 @0x8004a258), L1 `andi 0x4` (-1 @0x8004a288), R2 `andi 0x2` (+10 @0x8004a2b8), L2 `andi 0x1`
    (+246 @0x8004a2e8), KREIS `andi 0x20` -> `sb zero,9832(at)` @0x8004a308, DREIECK `andi v0,a1,0x10` @0x8004a300 -> Menge + 1
    `sb v1,27157(v0)` @0x8004a33c (Adresse 0x800ac766 - 8398 + 27157 + 4k = 0x800b10ad + 4k = Mengen-Byte), Kappung `sltiu
    0x48` @0x8004a350. Die Menge 255 + 1 = 0 ist damit Original-Verhalten (Byte-Store), kein Port-Fehler.
* **D4** (Integration; R2/L2 hat weder RE15_INPUT_SCRIPT noch RE15_PRESS -> OHNE Skript: `RE15_PRESS=start@260,square@323,
  select@356,circle@445,cross@470,start@500` plus ECHTE Tasten per `PostMessage(WM_KEYDOWN/UP)` an das Fenster der eigenen
  exe-Kopie, Werkzeug `werkzeug/tasten_inject.py`, Protokoll `laeufe/int_item_d4/inject.log`: '3' = R2 @F370/F385, '1' = L2
  @F400, 'e' = R1 @F415, 'q' = L1 @F430, 'e' @F455):
  SELECT -> leer (F368); R2 -> "Acid Grenade" (Id 10, F376); R2 -> "Colt Python" (Id 20 = 0x14, F392); L2 -> "Acid Grenade"
  (F404); R1 -> "Incendiary Grenade" (F420); L1 -> "Acid Grenade" (F436); KREIS F445; R1 F455 -> **bleibt** "Acid Grenade"
  (F460, Zustand 0); Schliessen -> `[equip] W-bank -> W0A`. Alle sechs Tasten wie @0x8004a238-0x8004a33c.
* **D2** (Integration; KREIS per `RE15_PRESS=circle@452` nach 9x R1): R1 in F461 aendert nichts (F464/F468 "Hand Grenade 255"),
  Schliessen -> `W09`.
* **D3 M-2** (Integration; 3x R1 -> "Browning HP 255", Kreuz + Schliessen OHNE Kreis F428, wieder oeffnen F521, 2x R1 im
  ITEM-Raster F617/F629): Bild F620/F632 weiter "Browning HP" (Debug-Zustand beim Oeffnen 0: `sb zero,9832(at)` @0x8004648c,
  `sb zero,9833(at)` @0x80046494), Schliessen -> `[equip] W-bank -> W03`. **M-2 bestaetigt.**
* Ergebnis (3): **alles wie Soll** (A10/K9, W5, M-2). Nicht gemessen: Pad-2-Auffueller (@0x8004a0dc-130, im Port nicht
  portiert), Id-Kappung 0x47 per L2 von 0 (nur Unit-Sonde).

## 4. Hebetisch ROOM1150 — Granate aufnehmen (Runde-30-Weg) und werfen

Lauf `hebetisch1150` (beide exe; Vorlage `analysis/befunde_runde30/nachtrag-granate_werkzeug/lauf_fahrt.sh`: Debug-Sprung
1150@240, Leon (-21000,-18500) Gier 0, Skript ab Spielbild 200 `W1,A0.2,W8,A0.2,W3,A0.2,W4` (Hebetisch, Sicherung Ja, Granate
Ja), dann Statusschirm `S0.2,W2,A0.2,W1,D0.1,W0.3,D0.1,W0.3,A0.2,W1,A0.2,W1,X0.2,W1,S0.2,W7` (Platz 4 -> AUSRUESTEN), dann
`M1,MA0.2,M2.5,W6` (MITTE-Wurf)).
* Beide exe: `[sicherung] Yes: genommen, Flag (9,53) ... Platz 3`, `[granate] Yes: genommen, Flag (9,56) gesetzt, Item 0x09 x1 in
  Inventar-Platz 4`, `[equip] W-bank -> W09`, danach "Fahrt zu Ende ... Sperre geloest" — Aufnahme + Ausruesten unveraendert.
* Integration, Wurf (gr.log): Abzug A = F1178 (`mg` 1 -> 0), `F=1200 SPAWN granate art=2 slot=0 anker=(-21444,-2356,-18836) gier=0`
  = A + 22 (MITTE, BAUPLAN 1.1); 7 Abpraller `010a0601 .. 010a0101` + `010a0001` + Liegen `010a0001` (SE-Folge wie 1.1),
  Liegen T1073, Explosion T1109 = L + 36 (Latch, `resolver art=2 P=(-9910,-491,-17132) r=500 eingriffe=0` — P.y = Liegestelle 9 -
  500; Irons (NPC 0x45, HP -1) 8000 entfernt, kein Kandidat), Kinder `03195000` (X), `03195000` + `030b5400` (X+5), Platz frei
  + `030b5800` (X+7), SE `04080001`. Ablauf = BAUPLAN 1.1/1.2.
* Kontakte bei S + 28 / L = S + 71 / X = S + 107 statt der 1140-Tabelle 29/73/109: die Spawnhoehe h = -2356 (1140: -2474) ist
  eine Posengroesse (BAUPLAN O1), siehe 4.1.
* Kamera: nach der Hebetisch-Fahrt Cut 0 ab F1060 (Leon ausserhalb des Bilds), in master identisch — keine Regression; der
  Wurf ist in diesem Cut nicht zu sehen (`hebetisch_wurf_bogen.png` im Ausgabeordner).

### 4.1 Befund: Granaten-Anker haengt am Zeichnen des Spielers
Gemessen ROOM1140, identischer MITTE-Wurf (Skript `W1,M1,MA0.2,M2.5,W4` ab 300, `RE15_GIVE=9:5 RE15_EQUIP=9`), nur der Cut per
`RE15_FORCE_CUT` verschieden (`laeufe/int_anker_cut0..7`):
* Cut 0 und 7 (Leon im Bild): `anker=(-6851,-2474,-18279)` (= bau_a a_wurf1).
* Cut 1..6 (Leon ausserhalb der Cut-Region, nicht gezeichnet): `anker=(-7606,-1693,-17553)` — 781 tiefer, ~1000 naeher.
Ursache im Port: `re15_player_gunbone_world` (re15_damage.c:1148) liest `s_hand_world/s_hand_rot`, die NUR der Spieler-Zeichner
setzt (main.c:8871-8873, im Block `player_visible` = Regions-Test main.c:8351-8364); nie gezeichnet -> Ersatz-Zielpose
(`muzzle_bone_world`), frueher gezeichnet -> letzter gezeichneter Stand (ROOM1150 nach der Fahrt: Knochen aus Cut 4).
**Original selbst disassembliert (`disasm/fun_8001e8c8.txt`, `fun_8001ef54.txt`)**: der Spieler wird im Zeichenteil der
Hauptschleife ueber FUN_8001e8c8 gefuehrt (`addiu a0,s0,720` = 0x800aca54 Spieler, `jal 0x8001e8c8` @0x8001d09c). Dort
entscheidet der Regions-Test (`jal 0x80014368` @0x8001e974, Viereck 0x800ac790) NUR zwischen zwei Teil-Schleifen ueber
`[+0x188]` (Schritt 172 = 0xAC, Anzahl `lbu s0,131(s0)`): im Bild FUN_8001e9ec (@0x8001e990), ausserhalb FUN_8001ef54
(@0x8001e9b4). BEIDE rechnen die Teil-Weltmatrix gleich: `jal 0x80022da0` mit a0 = `[Teil+108]` (Eltern), a1 = Teil+24,
a2 = Teil+64 (@0x8001ea1c-28 bzw. @0x8001ef7c-84); FUN_8001ef54 zeichnet nichts. Teil 11 + 0x40 = 11*0xAC + 0x40 = 0x7a4 = genau
der Granaten-Anker (`0x800acbdc` = Spieler 0x800aca54 + 0x188: FUN_80037a78 nimmt `addiu v0,v0,-13348` und daraus
`v0 - 360` = Spieler+0x20 (Matrix) / `v0 - 288` = Spieler+0x68 (Winkel) @0x80037a8c-98, `lw s1,0(v0)` = Teile-Feld). **Das Original schreibt den Anker also in jedem Bild fort, auch wenn Leon ausserhalb der Cut-Region steht;
der Port nicht** -> Port-Defekt M-H1. Betrifft ebenso Muendung/Rauch/Huelse (derselbe Anker) — dort unsichtbar (Region-Cull der
Effekte), bei der fliegenden Granate aber sichtbar (andere Bahn, Liegestelle, Explosionsort).

## 5. Speichern / Laden mit Granaten im Inventar

Karte vom Kartenwerkzeug `probe_r33_speichern_karte re15_card.mcr karte` (ROOM1150 vor dem Telefon, Inventarplatz 0 = Memory
Card 0x21). Laeufe mit `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=<laden>,<speichern>` (Titel-Autostart AUS — mit
`RE15_TITLE_SHOT` startet die exe ein NEUES Spiel und laedt nie; erster Versuch `save_s1` lief so 300 s ohne CONTINUE),
Granaten per Item-Debug im Spiel (RE15_GIVE wirkt beim CONTINUE nicht: das Laden ueberschreibt das Inventar, main.c
`re15_inv_load_briefing` + Kommentar zu RE15_GIVE), Telefon per VIERECK wie `integration_r33_speichern`.
* `save_s1` (laden Platz 0, speichern Platz 1): Item-Debug Platz 1 -> 9x R1 = 0x09, Platz 3 -> 10x R1 = 0x0A, Platz 2 -> 11x R1 =
  0x0B, dann Telefon: `FRAGE` -> `Antwort: JA` -> `[save] saved (room 1150) slot n=2 -> next=3; card=kept (slot 0 qty 1)`.
  Kartenblock Platz 1 (Datei 0x4128): `21 01 | 09 ff | 0b ff | 0a ff` (Platz 0 der Karte unveraendert `21 01 | 00 00 ...`).
* `load_s1` (laden Platz 1): `CONTINUE: resumed in room 1150`, Statusschirm zeigt Memory Card, Hand Grenade 255, Incendiary
  255, Acid 255 (`load_s1_bogen.png` im Ausgabeordner); Acid ausgeruestet -> `[equip] W-bank -> W0A`, Wurf -> `SPAWN granate art=3`,
  `mg` 255 -> 254.
* `save_s2` (laden Platz 1, Incendiary ausgeruestet, speichern Platz 2) + `load_s2` (laden Platz 2): beim CONTINUE sofort
  `[equip] W-bank -> W0B` (Ausruestung + ARMS-Bank aus dem Spielstand), Wurf ohne Statusschirm -> `SPAWN granate art=4`, 7
  Abpraller + Liegen, `resolver art=4 ... r=500`, `aufschlag re2_art=1` (Brand), `frei art=4`.
* Ergebnis (5): **Speichern/Laden der drei Granaten (Id, Menge 255, Ausruestung) wie Soll.**
* Beobachtung (kein Port-Fehler): im `load_s2`-Wurf (Leon x -23558, Gier 1864, Wurf nach Westen) springt die Weltlage von
  x -30539 (1. Abprall) auf +32497 (2. Abprall) — s16-Ueberlauf der Weltlage. Das ist Original-Verhalten: Weltlage wird als
  Halbwort gespeichert (`sh v0,40(a0)` @0x8001a1fc, BAUPLAN 1.1 "Weltlage +0x28/+0x2a/+0x2c ist s16"), der Resolver liest sie
  vorzeichenbehaftet; die Granate explodiert dann am anderen Ende der Welt (P = (31372,-491,-24428)). Nur bei Wuerfen nahe
  x/z = +-32768.

## 6. Bildrate / Ruckler bei Explosion und Bodenflammen

Messweg (ohne Eingriff in die exe, ohne Framedump — der verlaengert die Bildzeit selbst): `werkzeug/bildzeit.py` startet die
Messkopie und stempelt die Ankunft jeder Zustandslog-Zeile `F<n>` (1-ms-Abfrage, `time.perf_counter`). Aufstellung = RE2-Lauf
des Integrationstests (ROOM1140, Leon (-1676,-18070) Blick 1076, `RE15_AI_FLAVOR=re2`, Skript `MD0.6,MDA0.2,MD2.5,W5` ab Bild 1,
Fenster x3, `RE15_NOAUDIO=1`); Explosion X = F119 (gr.log `T=365 F=119 ... zuender=6`), Brand: `resolver art=4 ... eingriffe=1` +
`aufschlag re2_art=1` (Feuerball + 3 Bodenflammen, brennen bis ~X+140). Gegenlauf "leer" = gleiche Aufstellung ohne Wurf.
Parallel liefen fremde Suiten (Maschine unter Last).

| Lauf (30 Bilder/s, Standard) | vor X (F20-110) | X..X+11 | Flammen X+12..X+141 | danach | Bilder 1..320 |
|---|---|---|---|---|---|
| Brand 0x0B | 33.6 / Median 31.4 / max 47.4 ms | 34.1 / 31.5 / 47.0 | 33.4 / 31.4 / 47.5 | 33.7 / 31.5 / 52.8 | 33.55 ms/Bild, keine Luecke |
| HE 0x09 | 33.6 / 31.8 / 46.7 | 34.2 / 31.3 / 47.4 | 33.3 / 31.0 / 49.8 | 34.2 / 31.4 / 51.6 | 33.57 ms/Bild, keine Luecke |
| ohne Wurf | 33.6 / 31.5 / 48.7 | 34.3 / 31.8 / 46.7 | 33.6 / 31.7 / 53.1 | 33.4 / 31.6 / 48.2 | 33.57 ms/Bild, keine Luecke |
* Explosionsbild F119: Brand 35.8 ms (danach 26.6), HE 31.6 ms, ohne Wurf 32.7 ms. Die Einzelbilder mit ~47 ms (15-23 je 100)
  stehen in allen drei Laeufen gleich oft = Takt der `SDL_Delay`-Bremse (main.c:11171-11178), nicht die Granate.
* Gegenprobe 60 Bilder/s (`RE15_FPS=60`, nur Entwickler-Schalter, Standard 30 laut main.c:3685-3690): Flammenfenster Mittel
  17.61 ms gegen 17.15 ms ohne Wurf (+0.46 ms), Bilder > 20 ms 48 gegen 29 — messbare, aber kleine Zeichenlast der RE2-Flammen;
  im Explosionsbild keine Spitze (F226 16 ms).
* Ergebnis (6): **kein Ruckler** bei 30 Bildern/s (Standard); keine ausgelassenen/doppelten Spielbilder.
* Nebenbefund 60 Bilder/s (Hinweis H-E2): Anker/Takt der Granate sind bei `RE15_FPS=60` anders (TIEF-Anker (-2080,-1352,-18221)
  statt (-2108,-772,-19007), X - S = 165 Bilder statt 76): die Wurf-FSM laeuft je Anzeigebild, der ESP-Takt je 2. Bild. Da 60
  Bilder/s kein Nutzer-Schalter ist, nur Hinweis.

## MAENGEL (mit Beleg; Schwere)

| Nr | Schwere | Ort | Befund | Beleg | Vorschlag |
|---|---|---|---|---|---|
| M-Z1 | mittel | player_common.c Zielblock (`rate = READY ? (s_aim_recoil ? 24 : 48) : 24`) | Messer-Hieb dreht 24 je Bild (danach 48 im Clip-7-Rest); Original dreht im Hieb NICHT | `int_dreh_w1` F71-94 -24, F95-102 -48 (master -72/-48); Nahkampf-Sub 2 0x80035314-0x8003541c ohne Schreibzugriff auf 0x800acabe (Verteiler 0x80074164[2] @0x80034ec0; Hieb `jal 0x80011f50` @0x800353cc, Ende `sh aca5a` @0x80035400) | im Nahkampf-Hieb keine Handdrehung (Kommentar mit @0x80035314-41c) |
| M-Z2 | hinweis | player_common.c Zielblock | In den Uebergangsbildern des Haltens dreht der Port, das Original nicht: R1 los (-24), Hoehe an/aus (-48), Abzugsbeginn (-48) | `int_dreh_w3` F148/F103/F133/F70; Original `j 0x80033450` vor dem Drehcode @0x80033204 / @0x8003323c / @0x80033288 / @0x800332d4 / @0x8003332c (Nachladen `j 0x80033394` dreht) | Drehung in diesen Bildern auslassen |
| M-Z3 | hinweis | player_common.c (Handdrehung VOR `re15_player_aim_target`-Nachfuehrung) | Reihenfolge im Heben vertauscht: Original erst Nachfuehrung (`jal 0x8001a8f8` @0x80032fec), dann Pad (@0x80033000-94) | `int_track_w3_b`: 0, -24, 152, 215, 215 ... (Soll nach Original 176, 191, 191 ...) -> 24 Einheiten Versatz nach jedem Heben mit Richtungstaste + Ziel | Nachfuehrung vor die Handdrehung ziehen (nur Heben/L1) |
| M-Z4 | hinweis | player_common.c `slew = s_aim_melee ? 0xc0 : 0xc8` | Dauerfeuer-Waffen (Id 12/14/19, Verteiler 0x80074030[w] = 0x80034014) fuehren mit 0xc0 = 192 nach, Port 200 | `ori a1,zero,0xc0` @0x80034128 / `jal 0x8001a8f8` @0x80034134; `int_track_w12_c` Schritt -200 | Schritt 0xc0 fuer die Dauerfeuer-Maschine |
| M-Z5 | hinweis | player_common.c (feste 24/48) | Granatwerfer Id 15 (Gun-FSM) liest Zeile @0x800740d6 = `00 00 00 00 00` -> Original dreht im Heben/Halten/Abzug 0; Port 24/48/24 (Zeilen 14..19 alle 0 = unfertige Beta-Waffen) | `int_dreh_w15`; `re15_disasm.py read 0x80074090` | Einordnung Beta->Retail: RE2-Werte messen, dann entscheiden |
| M-H1 | mittel | re15_damage.c:1148 `re15_player_gunbone_world` <- main.c:8871-8873 (nur bei `player_visible`, main.c:8351-8364) | Granaten-Anker (und Muendung/Rauch/Huelse) kommt aus dem Zeichner: ist Leon ausserhalb der Region des aktiven Cuts, spawnt die Granate an Ersatz-/Altknochen. Original rechnet die Teil-Matrix auch ungezeichnet | ROOM1140 MITTE: Cut 0/7 `anker=(-6851,-2474,-18279)`, Cut 1..6 `(-7606,-1693,-17553)` (781 tiefer); ROOM1150 nach der Hebetisch-Fahrt `(-21444,-2356,-18836)`. Original: FUN_8001e8c8 Regions-Test @0x8001e974 waehlt nur Zeichnen (FUN_8001e9ec) oder Nicht-Zeichnen (FUN_8001ef54), beide `jal 0x80022da0` -> Teil+0x40 (@0x8001ea24 / @0x8001ef80) | Knochen-11-Weltmatrix des Spielers je Bild unabhaengig von `player_visible` rechnen (wie FUN_8001ef54) und an `re15_player_set_hand_world/_rot` geben |
| H-E1 | hinweis | ROOM20A0 Effekt id 6 sub 6 | in Cut 2 gezeichnet (`->`-Zeilen), aber 0 Pixel sichtbar — master und Integration gleich | Lauf `r20a0`, 0 Pixel in F1-F60 | keine Regression; ggf. gegen Original pruefen |
| H-E2 | hinweis | Entwickler-Schalter `RE15_FPS=60` | Wurf-FSM je Anzeigebild, ESP-Takt je 2. Bild: anderer Anker/anderer Zeitplan | TIEF-Anker (-2080,-1352,-18221) statt (-2108,-772,-19007), X - S 165 statt 76 | nur falls 60 Bilder/s je Nutzeroption wird |

Keine Maengel in (1) Bestandseffekte, (3) Item-Debug, (5) Speichern/Laden, (6) Bildrate.
Original-Eigenheiten (belegt, kein Port-Fehler): DREIECK im Item-Debug macht aus Menge 255 eine 0 (Byte-Store `sb` @0x8004a33c);
s16-Ueberlauf der Granaten-Weltlage (`sh` @0x8001a1fc) bei Wuerfen nahe +-32768; Licht-Latch je Schuss (auch im Dauerfeuer, E11).

## FAZIT

Die Integration ef1c6f94 bringt gegenueber master v0.8.19 bei den Bestandseffekten NUR belegte Aenderungen (C1 Takt, C2 Zeichnen,
C3/A7 Licht-Latch, W7, W8 Helligkeit, A2 Weltlage, N1.4 Eintrittseffekte) — keine unerklaerte Regression in 9 Szenarien. Das
Item-Debug laeuft in der echten exe vollstaendig wie das Original (SELECT/R1/L1/R2/L2/DREIECK/KREIS, Schliessen ruestet aus,
kein Debug-Menue, Reset beim Oeffnen). Hebetisch-Aufnahme, Wurf, Speichern und Laden der drei Granaten funktionieren; kein
Ruckler. Offen: das Drehen im Zielen stimmt fuer Pistole/Schrot/Granate/MP in Rate und Richtung, weicht aber beim Messer-Hieb
(M-Z1), in Uebergangsbildern (M-Z2), in der Reihenfolge beim Heben (M-Z3), im Dauerfeuer-Nachfuehrschritt (M-Z4) und beim
Granatwerfer (M-Z5) ab; und der Granaten-Anker haengt am Zeichnen des Spielers (M-H1, Port-Defekt: das Original rechnet die
Teil-11-Matrix auch ungezeichnet, FUN_8001ef54 `jal 0x80022da0` @0x8001ef80; `0x800acbdc` = Spieler+0x188 laut `addiu v0,v0,-13348`
/ `addiu s0,v0,-360` = Spieler+0x20 @0x80037a8c-98).
