# Runde 41 — Spur C "ventilkette" — Abnahme 6 (unabhaengig)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, gepruefter Stand HEAD **1e17827f** (Basis = merge-base master
116e8b0f; master steht auf 645fa45f und enthaelt fremde Spuren — Gegenstand ist der Spur-Diff `git diff master...HEAD`).
Massstab: die sechs Punkte des Auftragstexts und AUFTRAG.md (massgeblich) mit den Spur-C-Nachtraegen 6-10, 12, 13, 15, 16, 18, 19
und **25 + C-E4** (im Spurbaum vorhanden), den Entscheiden C-a..e, C-E1..C-E4, C-E2b (C-E2c widerrufen); Vorlage: Abnahme 5
(`C_abnahme_5.md`, Maengel 1-3), Nachbesserung 6 (Dossier §0g, Commits b2841a4a..1e17827f).
Begonnen 2026-10-25 06:1x MEZ. Pruefer: Abnahme 6 (Opus). Der Bericht wird FORTLAUFEND geschrieben. Messordner scratchpad `C41abn6/`
(unten `<a6>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/C41abn6`).

**Ergebnis (abgeschlossen 2026-10-25 ~08:0x MEZ): BESTANDEN.** Alle sechs Nutzer-Punkte erfuellt (Leon UND Elza, ueber echte Tueren gemessen); die drei Maengel der Abnahme 5 sind behoben: **Mangel 1** - der RE2-Alarm SUB_25 (bytegleich) laeuft mit dem Warntext 46 an und endet mit der Flut bzw. dem Raumwechsel (zwei unabhaengige Zwillinge: alte exe der Abnahme 5 und eigener Stummel-Zwilling, bitgleich ausserhalb des Alarm-Fensters; Elza per Korrelation); **Mangel 2** - der Countdown hat die RE2-Form (selbst disassembliert, gegen die RE2-VRAM und mit eigenem Nachbau Pixel fuer Pixel belegt: Port 55/55, 9/9, 54/54, 147/147 Bilder exakt; RE2 4/4 Bildpuffer exakt); **Mangel 3** - Kopfkommentar angepasst. Nachtraege 16/18/19/25 erfuellt. Gates G1 (6 Stichproben selbst disassembliert/gelesen), G2 (formale Hinweise V1/V2 offen, neu V3 audio_pc.c-Schalter) und G3 halten. Tests: Unit 290/290 + PASS (557), Integration 7/7 (6 r41 + cut10f0), Raumgruppe 17/17, `_kette` von Hand rc=0.

## 0. Stand / Protokoll (neueste zuerst)
* 07:59 ABSCHLUSS: Urteile §4, Maengel/Hinweise §5. Kein Code geaendert; nur dieser Bericht committet.
* 06:2x-07:5x Messlaeufe (§1): kb, kf, kf_ohne, kb_elza, kf_elza, p1r(+_elza), kbr(+_elza), p4(+_elza), p5v(+_elza), kh(+_elza), p5n,
  p6(+_elza), ka3, kn_elza, flr; RE2-Staende selbst ausgelesen; Integration 7/7 und Raumgruppe (§3); `_kette` von Hand rc=0.
* 06:2x Unit-Gruppe gruen, Integrationsstroeme gestartet, erste Messfahrt `kb` (Leon, echter Weg mit Ton) laeuft.
* 06:19 Bau: `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)` (Zeile 180 `integration_r41_ventilkette_kette uebersprungen:
  Spur A fehlt`), `local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (`<a6>/configure.txt`, `<a6>/build.txt`).
  exe `re15_pc.exe` 2026-10-25 04:59:05 (7945443 B); Quellen ventilkette_r41.c 04:53 / audio_pc.c 04:54 / ventilkette_r41_pc.c 04:58 /
  Kopf 04:52 aelter als ihre .obj (04:58/04:59) -> exe = Code HEAD (alle Engine-/Plattform-Aenderungen der Nachbesserung 6 liegen in
  81dadaab 05:00; die spaeteren Commits aendern nur Tests/Doku/plakat.py-Kommentar); `git diff HEAD` leer.
* 06:1x Start: `git status` = nur `?? build/`; HEAD 1e17827f; seit Abnahme 5 (b8d56709..HEAD) 8 Commits: Code ventilkette_r41.c (+46/-18),
  re15_ventilkette_r41.h (+44/-12), ventilkette_r41_pc.c (+104/-5), audio_pc.c (+30/-1, Haken C-E4), neues Asset
  shared_assets/RE2/SOUND/SUB_25.BGM, Riegel (unit R1-R3, Teil A Bildvergleich, neuer integration_r41_ventilkette_alarm), Doku.

## 1. Messungen an der exe (eigene Laeufe)
Werkzeug: `<a6>/lauf.sh <name> <timeout> KEY=VAL...` (eigene exe-Kopie `re15_pc_abn6c_<name>.exe` neben der exe, erbt keine
RE15_-Variablen, Laufordner `<a6>/mess/<name>`, Kopie danach geloescht; nie taskkill /IM). Wegplaene = die der Abnahme 5
(`<a6>/plaene/*.txt`, unveraendert kopiert; Treiber RE15_WEGPLAN = echte Pad-Bits). Beschleunigter Renderer (kein
RE15_SOFTWARE_RENDER), RE15_WINDOW_SCALE=1, RE15_FRAMEDUMP (Bildzaehler je Raum). Leon = Titel-Haken, Elza = RE15_PSELECT_AUTO=1
RE15_PSELECT_AUTO_SWITCH=1. exe = `re15_pc.exe` 04:59:05 = Code HEAD. Eigene Auswerter: `<a6>/py/capvergleich.py` (Takt-Differenz zweier
Aufnahmen, 1470 Stereo-Bilder int16 je Takt), `<a6>/py/alarmform.py` (Huellkurven-Periode/Spektrum), `<a6>/py/cdcheck.py` +
`cdtext.py` (EIGENER Nachbau der RE2-Anzeige: TIM-Leser fuer `info/re2leon/COMMON/DATA/TEX.TIM`, Zaehler nach den selbst
disassemblierten Stellen, Vergleich aller deckenden Glyphen-Pixel), `<a6>/py/re2vram.py` (eigener DuckStation-Leser: zstd-Frame,
RAM-Basis ueber die Code-Signatur von FUN_8002fa28, VRAM ab "GPU-VRAM"). Kein Werkzeug der Spur (re2cd/capdiff/capkorr) benutzt.

### KB — Mangel 1 (Alarm) + Mangel 2 (Countdown) + Punkt 3 (Flucht -> 20B0), ECHTER Weg (Leon `kb`; Plan und Parameter = Abnahme 5 `kb`)
Start 2010 am Kasten (Bits 101/102, (3,108), (9,125)), MIT Ton (RE15_SE_DEBUG, RE15_AUDIO_CAP_SYNC), Framedump alle 15 Bilder:
* Weg wie Abnahme 5: `loaded room2010.rdt` -> Tuer slot 0 -> `loaded room2000.rdt` -> slot 3 -> `loaded room2070.rdt` -> slot 1 ->
  `loaded room2030.rdt` -> `[rbj] Runde 41 Spur C: ROOM2030 bindet das RBJ von RE2 ROOM1080` -> `id=55`/`53`/`51`/`50` ->
  `Folge 09 07 02 01 -> RICHTIG: Warnung, Zaehler, Flut`.
* **Alarm an mit dem Warntext:** `[bgm] loaded: 6 VAGs, SEQ 244B, PPQN=480 tempo=500000us` -> **`[bgm] Runde 41 Spur C: RE2-Alarm SUB_25 an
  (ROOM2030, Warntext 46) geladen capTick 7188 F5111`** -> `[msgtext] room=2030 id=46 "WARNING: The water will rise in one minute. Please
  leave the room immediately!"` -> `[voice] Start ROOM2030 Id 46 ... capTick 7189` (Stimme wie Abnahme 5: derselbe capTick 7189).
* **Countdown in der RE2-Form:** nach dem Schliessen des Textes `Zaehler laeuft: 1800 Takte, Ost-Tuer -> 20B0, Wasser geparkt` ->
  **`Countdown RE2-Form Bild 0 " 0^59=98" Farbe 2 (x 104 y 32) F5563`**, `Bild 30 " 0^58=98" F5593`, `Bild 60 " 0^57=98" F5623` ... je 30 Bilder
  eine Sekunde, `Bild 810 " 0^32=98" F6373` -> zu Fuss ueber Treppe und Ring zur Ost-Tuer -> `DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750)
  target_cut=11` -> **`loaded room20b0.rdt`** -> `Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100 (Flut gelaufen, Leiter frei)` ->
  `[bgm] stage=1 room=0B entry=4A51 -> MAIN11(flag 1) SUB(flags 1/0)` -> **`RE2-Alarm SUB_25 aus (Raumwechsel, SUB-Platz vom Raum-BGM-Lader)
  capTick 8634 F1`**.
* **Bild gegen meinen eigenen RE2-Nachbau** (`cdcheck.py`, Start F5563, alle 60 Framedumps F5500..F6385): **55 von 55 Bildern nach dem Start
  exakt** (jedes deckende Glyphen-Pixel gleich, 515..605 Pixel je Bild, z. B. F5575 `" 0^59=58"` 596/596, F5605 `" 0^58=58"` 605/605, F6385
  `" 0^32=58"` 576/576; Farbe aus BGR555 << 3), die 5 Bilder davor ohne Anzeige. Bild **`<a6>/bilder/kb_vorher_nachher.png`** (F5590 links
  Abnahme 5 = `00:59:06` oben rechts in der Nachrichtenschrift, rechts heute = ` 0'59"08` dunkelrot in der 14x14-Schrift oben mittig; die
  alte Anzeige ist weg), Bogen `<a6>/bilder/kb_countdown.png`.
* **Alarm im Ton gegen einen unabhaengigen Zwilling:** dieselbe Fahrt der Abnahme 5 (`C41abn5/mess/kb/cap.raw`, alte exe ohne Alarm, gleicher
  Plan) — `capvergleich.py`: beide 8934 Takte; **bitgleich bis Takt 7190**, erste Abweichung **Takt 7191,043** (3 Takte nach dem Laden = SEQ0
  erste Note bei Tick 100 = 0,104 s), **RMS(neu-alt) 748,5..749,3 je 60 Takte durchgehend 7220..8599** (max |d| 2740..3094), mit der
  Raumwechsel-Ausblendung 686,8 (8600..8659) -> 143,1 (8660..8719) -> **0,0 ab Takt 8720** (bis 8933 bitgleich). Die Aufnahme ist ausserhalb
  des Alarms unveraendert (Raummusik, Stimme 46, Ventil-Toene, 20B0-Musik). Form (`alarmform.py`, Takte 7200..8600): Huellkurven-Periode
  **120 Takte = 4,00 s** (Autokorrelation 0,91) = SUB_25 SEQ0 (eigene Zerlegung des Containers: ppqn 480, Tempo 500000 us, Kanal 0 wechselt
  die Notenpaare 46/20 <-> 58/32 alle 3840 Ticks = 4,0 s, Schleife CC99 20 @Tick 100 .. CC99 30 @Tick 30820; Kanal 1 = Programm 1).

### RE2 — die Original-Anzeige selbst nachgelesen (Gegenprobe der Spur-Behauptung "RE2 zeigt 0'56"98 rot / 4'55"45 gruen")
* Quelle: die DuckStation-Staende der Nachbesserung (`nb6/re2cap/`): `quelle_ss2.sav` ist **bytegleich** dem Nutzer-Stand
  `%LOCALAPPDATA%/DuckStation/savestates/SLUS-00748_2.sav` (cmp); `ss2_cd60.sav`/`ss2_cd300.sav` = gepatchte Kopien; `cd60/resume.sav`/
  `cd300/resume.sav` = der Zustand nach dem Lauf im echten RE2. Mit meinem eigenen Leser (`re2vram.py`, kein Werkzeug der Spur):
  - Nutzer-Stand: `Status_flg 0x800cfbd8 = 0x24800000 (Timer-Bit 0)`, `Restsekunden 0x800d4832 = 0`.
  - gepatcht: `Status_flg = 0x2c800000 (Timer-Bit 0x08000000: 1)`, Restsekunden **60** bzw. **300**.
  - nach dem Lauf: Restsekunden **56** bzw. **295** (RE2 zaehlt selbst), Timer-Bit weiter 1.
  - beide Bildpuffer aus der VRAM gegen MEINEN Nachbau (`cdtext.py`, alle 90 Zaehlerstaende x 2 Farben probiert): 60er-Stand
    **`" 0^56=95" Farbe 2: 583/583 deckende Pixel gleich -> EXAKT`** (Puffer y 0) und **`" 0^56=98" Farbe 2: 591/591 -> EXAKT`** (y 240); 300er-Stand
    **`" 4^55=48" Farbe 1: 557/557 -> EXAKT`** / **`" 4^55=45" Farbe 1: 549/549 -> EXAKT`** (naechstbester Kandidat jeweils nur 85-94 %). Lage x 104,
    y 32, 14x14-Schrift aus TEX.TIM, Rot unter 60 s, Gruen ab 60 s, die letzten Stellen 98/95 zweier aufeinanderfolgender Bilder.
  - Damit ist (transitiv, Pixel fuer Pixel) die Port-Anzeige = mein Nachbau = RE2. Vergleichsbild 4x **`<a6>/bilder/re2_port_countdown_x4.png`**
    (Zeilen 1/2 RE2 ` 0'56"98`/` 0'56"95` rot, 3/4 Port F5665/F5680 ` 0'56"58`/` 0'56"08`, 5 RE2 ` 4'55"48` gruen): gleiche Schrift, Farbe, Lage.
* Die Nutzer-Spielstaende sind unversehrt: alle 11 `SLUS-00748*` in `%LOCALAPPDATA%/DuckStation/savestates` md5-gleich der Sicherung der
  Nachbesserung (`nb6/re2cap/nutzer_backup`), Zeitstempel unveraendert (juengster 2026-10-18 12:42). (Die Fensterfotos `fenster_*.png` der
  Nachbesserung zeigen nicht RE2, sondern andere Fenster — der Beleg ist allein die VRAM, oben selbst gelesen.)

### KF — Punkt 3 "Zeit abgelaufen und noch im Raum" + Countdown-Ende + Alarm-Ende mit der Flut (Leon `kf`, Zwilling `kf_ohne`)
Plan/Start = Abnahme 5 `kf` (Sued-Landeplatz, Bits 101/102, (3,108), 125; richtige Folge zu Fuss -> Warnung -> zurueck zum Plakat ->
2x Aktion -> stehen bleiben), MIT Ton + RE15_AUDIO_CAP_SYNC + RE15_FADE_LOG, Framedump JEDES Bild F6200..F6659:
* `RE2-Alarm SUB_25 an (ROOM2030, Warntext 46) geladen capTick 4347 F4094` -> `id=46 "WARNING: ..."` -> `Countdown RE2-Form Bild 0 " 0^59=98"
  ... F4546` -> 2x **`Ereignis 23: Plakat gesperrt (Zaehler/Flut laufen, ihre Pad-Sperre bleibt)`** -> `Bild 1770 " 0^00=98" F6316` -> **`FLUT:
  Wasser steigt (Countdown-Takte 1801 von 1800)`** -> **`Countdown RE2-Form Bild 1800 " 0^00=00" Farbe 2 (x 104 y 32) F6346`** -> **`RE2-Alarm
  SUB_25 aus (Flut/Tod) capTick 6599 F6346`** -> `SPRITZER: Wasser y -1800 erreicht die Fuesse` -> `[fade-log] kick ch=0 value=0x5000` ->
  **`ERTRUNKEN: Wasser y -2700, Spieler y -1800 -> hp -1, blaue Blende`**.
* Countdown-Ende Bild fuer Bild gegen meinen Nachbau (`cdcheck.py`, Start F4546): F6338 `" 0^00=25"` ... F6343 `" 0^00=08"`, F6344 `" 0^00=05"`,
  F6345 `" 0^00=02"`, **F6346 `" 0^00=00"` (589/589, das Bild des Flut-Beginns = RE2s abgelaufenes Bild @0x80026284)**, ab **F6347 kein Countdown
  mehr** (0 rote Pixel im Feld x 104..216 y 32..46 in F6347/F6348/F6360; RE2 schaltet ab @0x800262f0..@0x80026320).
* Bildbogen **`<a6>/bilder/kf_flut.png`**: F6330 ` 0'00"52`, F6346 Draufsicht Cut 4 mit ` 0'00"00` und dem steigenden Wasser, F6347 ohne Anzeige,
  F6380 Wasser ueber dem Steg, F6420/F6450 blaue Blende unter Wasser, F6480 Leon sinkt im blauen Licht zusammen (Todesbild), F6659 dunkel.
* Alarm-Ende gegen den Zwilling `kf_ohne` (gleicher Lauf, `RE15_RE2_ASSET_ROOT=<a6>/re2_ohne_alarm` mit dem 4-Byte-Stummel
  `shared_assets/PSX/SOUND/SUB_25.BGM` -> `[bgm] SUB_25.BGM nicht lesbar`, `... NICHT geladen capTick 4347 F4094`, Flut und Tod in denselben
  Takten 6599/F6346): **bitgleich bis Takt 4349**, erste Abweichung **4350,043**, RMS(A-B) **733,7..764,4** je 60 Takte waehrend des Zaehlers,
  6580..6639 274,4 (Ende im Fenster), **letzte Abweichung Takt 6673** (= Alarm-Ende 6599 + 74 Takte Hall, 6640..6699 nur noch max |d| 2),
  danach bis Takt 8357 bitgleich.

### P1 — Punkt 1: Leiter 2000 vor der Flut, echte Tuer 2010 -> 2000 (Leon `p1r`, Elza `p1r_elza`; Plaene der Abnahme 5)
* Leon: `[aot] DOOR FIRE slot=0 rect=(-5300,-4925,hw=500,hh=875) target_cut=14 spawn=(-26450,-1800,-2700)` -> `loaded room2000.rdt` ->
  `ROOM2000: Leiter Slot 4 (@0x0169E) -> Sperrtext 40` -> **`[msgtext] room=2000 id=40 "I cannot climb up the ladder because of the water
  pressure."`** (2x), kein `loaded room2050`, Wegplan `FLAG (9,100) = 0` vor (F63) und nach (F611) der Leiter. Bild **`<a6>/bilder/p1r.png`**
  (F240/F360/F480): Wasserstrahl aus dem Gitterfenster neben der Leiter, Leon an der Leiter, Satz 40 tippt.
* Elza: `Spieler-Familie PL04` -> `loaded room2011.rdt` -> `room2001.rdt` -> `room=2001 id=40` (2x), Bit 100 = 0 vor/nach, kein Raumwechsel.

### KBR — Punkt 2 auf dem echten Weg 2070 slot 1 -> 2030 Ring (Leon `kbr`; Plan der Abnahme 5)
* `DOOR FIRE slot=1 rect=(5500,-6400,hw=500,hh=1500) target_cut=0 spawn=(6550,-5400,-100)` -> `loaded room2030.rdt` -> **`ROOM2030: Bruecke obj 0
  oben y -5400, Zelle 12 gesperrt`** -> `Ereignis 24 -> 2030_pult_ohne` -> **`id=40 "A control panel for the bridge. The pressure gauge reads
  zero.Intake valve 13 is closed."`** -> Nordventil vor der Bruecke `id=49 "It's locked."` -> `2030_v13_frage` -> **`id=41 "Intake valve 13. Will
  you open it?"`** -> `id=42 "You've opened valve 13. The pressure is rising."` (`FLAG (9,101) = 1` F1974) -> `2030_pult_senken` -> **`id=43 "The
  pressure gauge reads 80. Lower the bridge?"`** -> `[prop-member F2773] prop[0] obj=0x00 m=1 -> val=-1800` -> `FLAG (9,102) = 1` (F3047) ->
  `id=44 "The bridge is down."` -> Spieler auf dem Steg (-1042,-2009) F3853. Bildbogen **`<a6>/bilder/kbr_bruecke.png`**: Pult an der Nordwand
  (F370), Frage Yes/No (F2650), Cut 4 Bruecke faehrt (F2770), "The bridge is d.." (F3070), Leon auf dem Steg (F3850/F4000). Kurbel nicht beteiligt;
  kein Countdown in diesem Lauf (richtig: kein Zaehler).

### KB_ELZA — dasselbe mit Elza (`kb_elza`, Plan = Abnahme 5 `kb_elza`, MIT Ton)
* `Spieler-Familie PL04 (character=4, Elza-Bit=1)` -> `loaded room2011.rdt` -> `room2001.rdt` -> `room2071.rdt` -> `room2031.rdt` -> `ROOM2031 bindet
  das RBJ von RE2 ROOM1080` -> `Folge 09 07 02 01 -> RICHTIG` -> **`RE2-Alarm SUB_25 an (ROOM2031, Warntext 46) geladen capTick 7868 F5115`** ->
  `room=2031 id=46 "WARNING: ..."` -> `[voice] Start ROOM2031 Id 46 ... capTick 7869` -> **`Countdown RE2-Form Bild 0 " 0^59=98" ... F5567`** ...
  `Bild 810 " 0^32=98" F6377` -> `loaded room20b1.rdt` -> `Bits 103 + 100` -> **`RE2-Alarm SUB_25 aus (Raumwechsel, ...) capTick 9306 F1`**.
* Countdown gegen meinen Nachbau (Start F5567): **54 von 54 Bildern nach dem Start exakt**, 0 Abweichungen.
* Ton: die Elza-Aufnahme der Abnahme 5 ist KEIN bitgleicher Zwilling (Abweichung schon ab Takt 80 - der Elza-Start ueber RE15_TITLE_CONFIRM_MS
  ist wanduhrabhaengig); deshalb Korrelation (`alarmkorr.py`): Elza ab ihrem Alarm-Takt 7868 gegen das Alarm-Signal aus Leons Zwillings-Differenz
  ab 7188, 1300 Takte: **0,515 bei Versatz 0** (Gegenprobe +61 Takte 0,003, -900 Takte 0,003). Ende: Leons Alarm-Signal ist schleifenperiodisch
  (Phase 100..400 gegen 1060..1360: **1,000** = Schleife 960 Takte = 32 s); Elza vor dem Raumwechsel (Takte 9066..9246) **0,628**, in 20B1 ab
  Alarm-Ende + 120 Takte (9426..9606) **0,017**, (9486..9606) **0,024** -> der Alarm laeuft in 2031 vom Warntext bis zum Raumwechsel und ist in
  20B1 aus.

### KF_ELZA — Flut/Tod mit Elza (`kf_elza` = Plan kf mit 2031, MIT Ton, Framedump jedes Bild ab F6200)
* `RE2-Alarm SUB_25 an (ROOM2031, Warntext 46) geladen capTick 5017 F4094` -> `id=46` -> `Countdown ... Bild 0 ... F4546` -> 2x `ROOM2031 Ereignis 23:
  Plakat gesperrt` -> `FLUT: Wasser steigt (Countdown-Takte 1801 von 1800)` -> **`Countdown RE2-Form Bild 1800 " 0^00=00" ... F6346`** -> **`RE2-Alarm
  SUB_25 aus (Flut/Tod) capTick 7269 F6346`** -> **`ERTRUNKEN: Wasser y -2700, Spieler y -1800 -> hp -1, blaue Blende`**.
* Countdown F6200..F6346 (147 Bilder, JEDES) gegen meinen Nachbau **147/147 exakt**; ab F6347 kein Countdown (0 rote Pixel F6347/F6400).

### P4 — Punkt 4: nach der Flut 20B0 statt 2000, Leiter frei (Leon `p4`, Elza `p4_elza`; Plaene der Abnahme 5, Bits 103/100 + (3,235))
* Leon: `ROOM2010: Tuer Slot 0 -> ROOM20B0 Cut 10 (Ziel ROOM2000)` -> `DOOR FIRE slot=0 ... target_cut=10` -> **`loaded room20b0.rdt`** -> Leiter
  **`DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)`** (kein Sperrtext) -> `loaded room2050.rdt` ->
  `ROOM2050: Tuer Slot 0 -> ROOM20B0 Cut 9` -> `loaded room20b0.rdt`. Bild **`<a6>/bilder/p4_20b0.png`** (Cut 9): das Gitterfenster ohne
  Wasserstrahl (vgl. `p1r.png` mit Strahl), Leon an der Leiter. (Lauf per Zeitlimit beendet - Plan ohne EXIT_AT, wie Abnahme 5.)
* Elza: `loaded room2011.rdt` -> `ROOM2011: Tuer Slot 0 -> ROOM20B0 Cut 10` -> **`loaded room20b1.rdt`** -> Leiter slot 4 -> `loaded room2051.rdt` ->
  `ROOM2051: Tuer Slot 0 -> ROOM20B0 Cut 9` -> `loaded room20b1.rdt`.

### P5 — Punkt 5: Hebel 2050 erst nach dem Pult (80 V) (Leon `p5v`/`kh`/`p5n`, Elza `p5v_elza`/`kh_elza`)
* Vorher (`p5v`): `ROOM2050 Ereignis 4 -> 2050_hebel_sperre` -> **`id=40 "The pump has no power. The switchboard must be set first."`**, `FLAG (3,104) =
  0` vorher (F63) und nachher (F532). Elza `room=2051 id=40` gleich, (3,104) bleibt 0.
* Pult (`kh`, Eingabeskript der Abnahme 5): `2050_pult_ein` -> `id=43 "A high voltage switchboard. The pump needs 80 volts."` -> `id=41 "Will you
  push them?"` -> `Cut_chg(15)` -> `Hebel 3 -> EIN ... 40 V` -> `Hebel 1 -> EIN ... 70 V` -> `Hebel 4 -> EIN ... 80 V` -> `id=42 "Power supply OK."`
  -> **`Pult 2050 GELOEST: 80 V -> Bit 106`** -> `id=44 "The switchboard is set to 80 volts. The pump is powered."`. Elza `room=2051` Zeile fuer Zeile gleich.
* Nachher (`p5n` = Plan p5v mit Bit 106): Hebel = Original **`id=0 "A lever to fill and drain water. Will you move it?"`** -> `FLAG (3,104)` **0 -> 1**.

### P6 — Punkt 6 (ersetzt durch Nachtraege 7/8/12 (k)): mit dem Strom 2030 knapp unter der Absperrung, Rechner 2080 = Original (Leon `p6`, Elza `p6_elza`)
* Leon ueber die echte 20B0-Ost-Tuer: `DOOR FIRE slot=3 rect=(5950,-6400,...)` -> `loaded room2030.rdt` -> **`ROOM2030: Strom (3,126) -> Wasser knapp unter
  der Absperrung y -5350 (Schachtoeffnung), Treppe Slot 8 -> Sperrtext 48, Ring frei`**, `[prop-render] pi=4 oid=0x04 pos=(0,-5350,0)`, Treppe
  `id=48 "I cannot go down the stairs because of the water."`, Lift B `FLAG (3,220)` 0 -> 1 -> `loaded room2080.rdt` -> **`id=0 "From here maybe I can
  control the water level.Will you operate the computer?"`** -> `FLAG (3,245)` 0 -> 1 -> `loaded room2090.rdt`.
* Elza Zeile fuer Zeile gleich (`room20b1` -> `room2031` Pegel -5350 -> `id=48` -> (3,220) 0 -> 1 -> `room2081 id=0` -> (3,245) 0 -> 1 -> `room2091`).

### N16 — Nachtrag 16 (Regression): Ventil-Sicherungskasten 2010 (Leon `ka3`), nach der Flut Herausnehmen + Weg zu 2060 (Elza `kn_elza`)
* `ka3`: 2070 -> `loaded room2000.rdt` -> `loaded room2010.rdt` -> `Ventil-Sicherungskasten Slot 8 ..., Sicherung fehlt` -> `id=40 "A fuse box for the valve
  controls.One of the fuses is missing..."` -> `id=41 "Will you use the Fuse?"` -> **`Sicherung eingesetzt -> Bit (9,125) = 1`** -> **`Standbild der
  Ventile offen (ROOM2030 Cut 7) + Aufschliess-Ton`** -> `Standbild BSS/ROOM2030/BG07.BSS dekodiert` -> `zu (nach 82 Bildern)` -> `id=42 "You've used
  the Fuse."` - unveraendert gegen Abnahme 5.
* `kn_elza` (nach der Flut, Bit 125): `loaded room20b1.rdt` -> `room2011.rdt` (`Sicherung steckt (Bit 125)`) -> `id=43 "The fuse is in place.Will you
  take the Fuse?"` -> `id=44 "You've taken the Fuse."` -> **`Sicherung genommen -> Bit (9,125) = 0`** -> `room2051` -> `room2021` -> `room2031` ->
  `room2061` -> `id=3 "One of the fuses is missing..."` -> **`id=4 "Will you use the Fuse?"`**.
* Gegenprobe Anzeige: in 20B0 nach der Flucht (kb F250..F295) 0 rote Pixel im Countdown-Feld; im Lauf ohne Zaehler (kbr) kein Countdown.


### FLR — Punkt 3 "Wasser in 2030: Ueber die Ringebene" (Leon `flr`; Plan des Riegels integration_r41_ventilkette_alarm: richtige Folge, dann auf den Ring)
* `Folge 09 07 02 01 -> RICHTIG` -> `Countdown ... Bild 0 ... F4484` -> Spieler auf dem Ring (6602,-299) -> `FLUT: Wasser steigt (Countdown-Takte 1801
  von 1800)` -> `Countdown ... Bild 1800 " 0^00=00" ... F6284` -> **`SPRITZER: Wasser y -5400 erreicht die Fuesse (Spieler y -5400)`** -> **`ERTRUNKEN:
  Wasser y -6300, Spieler y -5400 -> hp -1, blaue Blende`** (der Ringboden liegt bei y -5400; das Wasser steht 900 darueber). Bild
  **`<a6>/bilder/flr_ring.png`** (Cut 4, F6300 -> F6420): das Wasser steigt ueber Steg und Bruecke bis an die Ringwaende, danach die blaue Blende.

### KE — `integration_r41_ventilkette_kette` von Hand (C-E1; im C-Baum nicht registriert)
* configure: `-- integration_r41_ventilkette_kette uebersprungen: Spur A fehlt (...)`. Von Hand (`cmake -DRE15_PC_EXE=... -DRE15_SRC=re15_port
  -DWORKDIR=<a6>/kette_hand -P test_r41_ventilkette_kette.cmake`, `<a6>/kette_hand.txt`, 07:46-07:49): **`r41_ventilkette_kette: 2030 richtige Folge
  09/07/02/01 zu Fuss -> Warnung -> Ost-Tuer zu Fuss -> ROOM20B0, Bits 103 + 100; Teil ab der Szene uebersprungen: Haken der Spur A fehlt`**, rc=0.

## 2. Gates

### G1 RE-Gate (eigene Disassembly / eigene Byte-Lesung; `re2_disasm.py` auf `info/re2leon/PSX.EXE`, Rohbytes der RE2-RDTs)
1. **Countdown-Zweig der RE2-Hauptschleife** (selbst disassembliert): @0x80025de4..@0x80025e00 `lw v1,-1064(v1)` (0x800cfbd8) / `lui a0,0x800` /
   `and v0,v1,a0` / `beq v0,zero,0x80026364` (nur mit Timer-Bit 0x08000000); @0x80025e0c..@0x80025e1c System_flg 0x800cfb74 `andi 0x8` /
   `beq v0,zero,0x8002608c` / Verzoegerung `addiu a2,zero,1` (Farbe 1); @0x80026094 `slti v0,v0,30`; @0x800260a4 `lw 0x800c3a8c` (Vorbild);
   @0x800260b8..@0x800260d8 `addu s6,zero,zero` / `addiu s7,zero,8` / `lhu`+`addiu -1`+`sh` auf `-31158(s3+0x10000)`; @0x800260ec `slti v0,v1,60` ->
   @0x800260f8 `addiu a2,zero,2`; @0x800260fc `bltz v1,0x80026284`; @0x80026140 `addiu a1,zero,32`, @0x80026144 `addiu a3,zero,2`,
   @0x80026170/@0x80026174 Format **0x800109a4**, @0x80026188 `addiu v0,zero,9`, @0x800261e8 **`addiu a0,zero,104`**, @0x80026274 `jal 0x8002fa28`;
   abgelaufen @0x80026284..@0x800262a0 Format **0x800109b4**, x 104, y 32, `addiu a2,zero,2`, Platz 0; @0x800262f0..@0x80026320 Status_flg &=
   0xf7ffffff, System_flg |= 4. Zeichenketten (EXE-Datei 0x800 + addr - 0x80010000): 0x800109a4 = `25 32 64 5e 25 64 25 64 3d 25 64 25 64 00` =
   **"%2d^%d%d=%d%d"**, 0x800109b4 = **" 0^00=00"** — **bestaetigt**.
2. **Druck/Schrift**: FUN_8002fa28 `sll a2,a2,4` / `ori a3,a3,0x200` / `or a2,a2,a3` -> attr = Farbe<<4 | Platz | 0x200 (kein 0x4000 ->
   feste Breite); FUN_800316a8 @0x800316e8 `andi v0,v0,0x200` -> FUN_800318a4: @0x800318fc/@0x80031900 SPRT **0x64808080**, @0x8003191c..@0x80031930
   CLUT y = **480** + 2*Farbe, x 256 (`ori 0x10`), @0x80031970/@0x80031974 Leerzeichen 32 nur Vorschub, @0x80031978 **`addiu t2,zero,14`**,
   @0x800319b0/@0x800319c0 **0x38e38e39** (c/18), @0x800319d8..@0x800319e0 **0x000e000e** (14 x 14), uv = ((c%18)*14, (c/18)*14) — **bestaetigt**.
   (Nebenbefund: mit attr-Bit 0x4000 gaebe es eine Breitentabelle @0x8009de34; der Countdown setzt das Bit nicht.)
3. **Ausloeser in den RE2-RDTs** (Rohbytes `info/re2leon/PL0/RDT`): ROOM6170 @0x038C6 `22 01 04 01` + @0x038CA `24 23 2c 01` (300), ROOM7010
   @0x02376 `22 01 04 01` + @0x0237A `24 23 2c 01`, ROOM7030 @0x022AC `22 01 04 01` + @0x022B0 `24 23 78 00` (120), ROOM7000 @0x0191C `22 01 04 00`;
   Save-Handler @0x8005451c: `lbu v1,1(v0)` / `lh a1,2(v0)` / `sh a1,0x47ec(at)` mit `sll v1,v1,1` -> 0x800d47ec + 2*0x23 = **0x800d4832** — **bestaetigt**.
4. **Alarm-Eintrag**: ROOM6040 @0x022E8 `57 00 04 06 11 25 00 80`, @0x022F0 `57 00 05 06 11 25 00 80`; Handler @0x80057e98 `lhu 2/4/6` -> a0 =
   0x0604_0000 | 0x2511 | 0x8000 -> FUN_8005b9f4 `sh a0` (= **0xA511**: MAIN 0x11, SUB-Byte **0xA5** = 0x25 | Bit 7); FUN_8005a444 @0x8005a650
   `srl v1,v0,7` -> DAT_800d75d1, @0x8005a654/@0x8005a658 `srl v0,v0,6` + `andi 1` -> DAT_800d75c9 — **bestaetigt** (Port-Gegenstueck
   re15_bgm_load_sub: `(sub_b>>6)&1` stummt SEQ0, Bit 7 sperrt SEQ#2).
5. **Alarm-Lautstaerke**: ROOM6040 @0x038BA `51 01 00 01 64 40`; Handler @0x80057e20 packt pc[1..5] -> FUN_8005b6f0: Teil (pc[2]) 0 -> Tabelle
   @0x80011608[0] = 0x8005b974: Lautstaerke != 0 und Programm (pc[3]) = 1 -> `sb s2,-15(v0)` = VabHdr + 16*1 + 17 = **ProgAtr[0].mvol = 0x63**,
   Pan -> ProgAtr[0].mpan = 0x3f — **bestaetigt**; der Port ruft `ss_seq_ctl_ex(1,0,1,0x64,0x40)` = dieselbe Semantik (prog_mvol[part-1] =
   vol-1, prog_mpan = pan-1, audio_pc.c ~Z. 3617, vorbestehend). SUB_25-Datei selbst zerlegt: Trailer SEQ#2 @244, VH @824 ("pBAV", 8 Programme,
   6 VAGs), **ProgAtr[0].mvol 0, [1].mvol 0** (in der Datei stumm), SEQ0 ppqn 480 / Tempo 500000 / Schleife Tick 100..30820. `shared_assets/RE2/
   SOUND/SUB_25.BGM` md5 **ef66256d0e847e428b11caedd5c655da** = `info/re2leon/COMMON/SOUND/SUB_25.BGM` (auch der Git-Blob).
6. **RE1.5-Gegenstueck im BGM-Lader** (auf das sich audio_pc.c beruft; `re15_disasm.py` auf `info/Re1.5/PSX.EXE`): @0x8004432c `jal 0x800603dc` mit
   a0 = DAT_800b52b6 (SUB-SEQ0) und @0x80044354 `jal 0x800603dc` mit a0 = DAT_800b52be (SUB-SEQ#2) = "beide SUB-Sequenzen stoppen";
   @0x8004439c..@0x800443ac `andi v0,v0,0x1` -> `sb DAT_800b52b5`, `sb v1 -> DAT_800b52bd` (Flags aus dem Sub-Byte); FUN_80044774 @0x80044774
   (Prolog, `addiu s0,s0,0x52b6`) = SUB-Lader — **bestaetigt**.
* Neue Konstanten im Diff seit Abnahme 5 (Kopf re15_ventilkette_r41.h "Countdown"): RE15_R41C_CD_X 104 / _Y 32 / _SCHRITT 14 / _GLYPHE 14 /
  _SPALTEN 18 / _TAKTE 30 / _ROT_UNTER 60 / _FARBE_GRUEN 1 / _FARBE_ROT 2 / _CLUT_Y0 480 / _ZEICHEN 8 — jede mit @0x... und alle oben selbst
  gefunden; audio_pc.c 0xA5 (@0x022EE) und `51 01 00 01 64 40` (@0x038BA) ebenso. Die PORT-WAHL-Konstanten der Nachbesserung 5
  (RE15_R41C_COUNTDOWN_X/_Y/_FARBE) sind entfernt (grep: nur noch als Geschichte im Kommentar).
* Rate-Woerter (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vorerst|ungefaehr|geschaetzt|approx`) in den HINZUGEFUEGTEN Zeilen
  des ganzen Spur-Diffs: **0 Treffer**. Neue `getenv` seit Abnahme 5 in engine/platform/include: **keine**.
* Erklaert der Fix den Befund? Mangel 1 (kein Abspielweg, SUB-Lader las nur RE1.5): jetzt Lader-Quelle RE2 nur waehrend des Alarm-Ladens +
  Schalter; gemessen (KB): Abweichung genau ab dem Warntext, Form = SUB_25 SEQ0. Mangel 2 (RE2-Anzeige nicht gefunden): gefunden und
  Pixel fuer Pixel nachgebaut, gegen RE2-VRAM belegt. Mangel 3: Kopf Z. 146-149/162 jetzt "Bit 102 ... UND Bit (9,125) ... Satz 49 = Original
  'It's locked.'" und "Fundort neben der Sicherung ... Zettel AUFRECHT an der Suedwand"; keine Reste ("look around"/"Blatt am Boden" nur als
  Geschichte).

### G2 Vertrag (VERTRAG.md §1/§2, Entscheide C-E3/C-E4)
* **Pfade** (`git diff master...HEAD --name-only`, ohne analysis/): keine `release/`, `platform/android/`, `shared_assets/PSX/`, `pri/`, keine Test-
  CMakeLists. `synchro/STAGE2/room2030|room2031/main46.wav` = Blob `2f750a04...` == master (Orchestrator-Commits 3da82407/f091aa04). Neu seit
  Abnahme 5 nur `shared_assets/RE2/SOUND/SUB_25.BGM` (bytegleich RE2, von C-E4 verlangt; Paket-Gate im Dossier §0g genannt).
* **Bits/Ids/Slots/Ereignisse:** seit Abnahme 5 keine neuen (Bits 100-103/106/107/125, Texte 2000/20B0 40-41, 2010 40-44, 2030 40-55, 2050 40-44,
  2070 40 unveraendert; `unit_r36_kartenbits` in der Unit-Gruppe gruen).
* **Gemeinsame Dateien:** `audio_pc.c` (C-E4 erlaubt den Haken): 4 markierte Einzeiler ("Runde 41 Spur C, C-E4": Zustand, Quelle im
  Track-Lader nur waehrend des Alarm-Ladens, Zuruecksetzen in re15_bgm_load_sub, `sub_same = 0` in re15_audio_start_room_bgm nur mit aktivem Alarm)
  + der Schalter `re15_audio_r41_alarm` am Dateiende (~20 Codezeilen, braucht die statischen BGM-Zustaende der Datei) -> **Hinweis V3 (formal)**:
  groesser als "1-5 Zeilen" (VERTRAG §1.3), vom Orchestrator wie V1 zu bestaetigen. main.c unveraendert seit Abnahme 5 (V1 offen), Bit 125 im
  master-VERTRAG (V2 offen).
* **Tests je Mangel:** Mangel 1 -> `integration_r41_ventilkette_alarm` (Zwilling ohne SUB_25: vor bitgleich, waehrend RMS >= 300, nach bitgleich;
  Elza-Korrelation) + Unit R3 (Alarm an/aus am Engine-Takt); Mangel 2 -> Teil A von `integration_r41_ventilkette` (Pixelvergleich gegen einen
  eigenstaendigen RE2-Zeichner, Text aus der Bildnummer) + Unit R1 (15 Zaehlerstaende inkl. 300/120/700 s) / R2 / R3 (Bild fuer Bild ohne Luecke,
  " 0^00=00" im Bild des Flut-Beginns, danach aus). Kein Riegel wurde entschaerft (im Unit-Diff seit Abnahme 5 keine entfernte Zeile; Teil A
  ersetzt den Rot-Mittelwert durch den strengeren Pixelvergleich). Linux-fest: `m` fuer test_r41_ventilkette ausser WIN32, `cmake_minimum_required`
  im neuen -P-Skript, `.exe` nur bedingt, eigener exe-Praefix `re15_pc_r41calarm_` + eigener Laufordner.
* **G2 haelt** (formale Hinweise V1/V2 aus Abnahme 5 weiter offen, neu V3).

### G3 Nebenwirkungen (beide Szenarien, Nachbarraeume)
* Ton ausserhalb des Alarms unveraendert: KB-Zwilling (alte exe) bitgleich bis zum Alarm (2010, 2000, 2070, 2030 vor dem Text) und ab Alarm-Ende
  + 86 Takte (20B0-Musik); KF-Zwilling bitgleich ausser im Alarm-Fenster. BGM-Lader-Riegel gruen (`integration_r35_cut10f0`, `unit_rotor_bgm_pin`,
  `unit_r35_cut10f0_*`, `unit_r39_johnruf1041_*`, Stimmen-/Ton-Units in der Unit-Gruppe).
* Countdown nur in 2030/2031 waehrend des Zaehlers (kein Countdown in kbr ohne Zaehler, in 20B0 nach der Flucht, nach dem Flut-Bild).
* Alle Nutzer-Punkte mit Leon UND Elza ueber echte Tueren nachgemessen (§1), Ergebnisse gleich Abnahme 5.
* **G3 haelt.**

## 3. Tests (gezielt, im Spurbaum; KEINE volle Suite)
* Unit-Gruppe (`<a6>/regex_unit.txt` = Regex der Abnahme 5 + BGM-/Stimmen-Pfade, die audio_pc.c linken oder den BGM-Lader pruefen:
  `unit_rotor_bgm_pin|unit_r35_cut10f0_.*|unit_r39_johnruf1041_.*|unit_r34_plattform_ton|unit_r38_werkstatt11b0_ton|unit_r39_stimmen_.*|
  unit_synchro_stimme|unit_voice_ausreden|unit_zombie_se_ab_10d0|unit_r38_elzaintro_stimme`; 290 Tests, -j 2):
  **`100% tests passed, 0 tests failed out of 290`** (`Total Test time (real) =  87.28 sec`, `<a6>/ctest_unit.txt`).
* Integration (zwei Stroeme parallel, `<a6>/integ.sh`, je `--timeout 3600 --output-on-failure`, `<a6>/ctest_integ1.txt` / `ctest_integ2.txt`):
  - `^integration_r41_ventilkette(_weg|_alarm)?$`: `integration_r41_ventilkette ... Passed 726.02 sec`, `integration_r41_ventilkette_alarm ...
    Passed 709.06 sec`, `integration_r41_ventilkette_weg ... Passed 1470.14 sec` -> **`100% tests passed, 0 tests failed out of 3`**.
  - `^(integration_r41_ventilkette_weg_elza|integration_r41_ventilkette_b|integration_r41_ventilkette_sperre|integration_r35_cut10f0)$`:
    `integration_r35_cut10f0 ... Passed 270.61 sec` (BGM-Lader, audio_pc.c beruehrt), `integration_r41_ventilkette_b ... Passed 534.57 sec`,
    `integration_r41_ventilkette_weg_elza ... Passed 1699.12 sec`, `integration_r41_ventilkette_sperre ... Passed 307.59 sec` ->
    **`100% tests passed, 0 tests failed out of 4`**.
  - Raumgruppe der Abnahme 5 (`<a6>/regex_integ_raum.txt`, 17 Tests: fahrstuhl_n1, spinnen_echtlauf, itemband(_lift), gegnerzahl,
    wasser(zombie), kiste(2feld/regel), dachkiste, dokumente_bild, tagebuch_lesen/karte/leon, irons_tisch_laden/bild; -j 2,
    `<a6>/ctest_integ_raum.txt`): **`100% tests passed, 0 tests failed out of 17`** (`Total Test time (real) = 2681.91 sec`; z. B. itemband_lift 1393.80 s, dachkiste 1552.57 s unter Last, kein Rot)
  - `unit_r41_ventilkette` direkt: **`PASS (557 Pruefungen)`**.
  - `_kette` von Hand rc=0 (KE).

## 4. Urteil je Nutzer-Punkt (gegen den Wortlaut, AUFTRAG.md; gebauter Stand exe 04:59:05 = Code HEAD 1e17827f)
AUFTRAG.md im Spurbaum enthaelt Nachtrag 25 + C-E4 (Massstab fuer Mangel 1/2 der Abnahme 5).

| Punkt | Urteil | Beleg (eigene Messung) |
|---|---|---|
| 1 Leiter 2000 -> 2050 mit herunterlaufendem Wasserstrom blockiert, "I cannot climb up the ladder because of the water pressure", bis die Flut gelaufen ist | **erfuellt** | P1 Leon + Elza ueber die echte Tuer 2010/2011 -> 2000/2001: Satz 40 woertlich (2x), kein Raumwechsel, Bit 100 = 0 vor/nach; Bild mit Strahl (`p1r.png`) |
| 2 2030: Bruecke zunaechst OBEN; Pult links (Kamera 2, Nordwand): zu wenig Druck / Ventil 13 zu -> Ventil 13 -> "Lower the bridge?" -> Bruecke faehrt herunter; nicht die Kurbel | **erfuellt** | KBR Leon + Elza ueber die echte Tuer 2070 -> 2030/2031: oben + Zelle 12 gesperrt, 40, 41/42 (Bit 101), 43 -> Ja -> Bruecke -1800 (Bit 102), 44, Spieler auf dem Steg (`kbr_bruecke.png`) |
| 3 Ventilraetsel -> Warnung (1 min, Raum verlassen) -> 60 s -> Wasser ueber die Ringebene; Ost-Tuer -> 20B0 (Auswasch-Szene); Zeit abgelaufen im Raum: Tod mit Ertrink-Effekt | **erfuellt** (die 20B0-Szene selbst nach C-E1 mit dem Haken der Spur A; `_kette` im C-Baum sauber uebersprungen, von Hand rc=0) | KB/KB_ELZA echter Weg: richtige Folge -> 46 -> Countdown -> zu Fuss zur Ost-Tuer -> 20B0/20B1, Bits 103 + 100; KF/KF_ELZA: Countdown bis " 0'00"00" im Bild des Flut-Beginns, Flut, Spritzer, ERTRUNKEN + blaue Blende (`kf_flut.png`); FLR: auf dem Ring Wasser y -6300 ueber Ringboden -5400 -> ertrunken (`flr_ring.png`) |
| 4 Danach 20B0 statt 2000 (spaeter wieder frei ohne Wasser), Leiter frei (kein Wasser laeuft mehr herunter) | **erfuellt** | P4 Leon + Elza: 2010/2011-Tuer -> 20B0/20B1, Leiter -> 2050/2051 ohne Sperrtext, 2050-Leiter -> 20B0; Bild Cut 9 ohne Strahl (`p4_20b0.png`) |
| 5 2050: Wasserlevel (Hebel) erst nach dem Pult-Raetsel (80 V wie 11F0) | **erfuellt** | P5 Leon + Elza: Hebel vorher Satz 40, (3,104) bleibt 0; Pult 43/41 -> Cut 15 -> 40/70/80 V -> "Power supply OK." -> Bit 106; mit Bit 106 Hebel = Original msg 0 -> (3,104) 0 -> 1 (`p5n`) |
| 6 2080-Rechner entleert danach auch 2030 - ersetzt durch Nachtraege 7/8/12 (k) (Strom: 2030 bis knapp unter die Absperrung, Rechner = Original) | **erfuellt** | P6 Leon + Elza ueber die echte 20B0-Ost-Tuer: Pegel y -5350, Treppe 48, Lift B (3,220) 0 -> 1, 2080/2081 Rechner = Original msg 0 -> (3,245) 0 -> 1 -> 2090/2091 |
| Nachtrag 18 Punkt 4 (typischer Timer + Alarm wie am Spielende + Voiceover beim Text) mit Nachtrag 25 (o) RE2-Alarm SUB_25, (p) Countdown in der RE2-Form, C-E4 | **erfuellt** | Countdown: RE2-Form selbst disassembliert (G1 Nr. 1/2) und gegen die RE2-VRAM belegt (4/4 Bildpuffer exakt); Port Leon 55/55 + 9/9 (Ende) + Elza 54/54 + 147/147 Bilder pixelgleich mit meinem Nachbau; erstes Bild " 0'59"98", letztes " 0'00"02", im Flut-Bild " 0'00"00", danach aus; alte MM:SS:HH-Anzeige weg (`kb_vorher_nachher.png`). Alarm: SUB_25 bytegleich, an mit Text 46, aus mit Flut (Zwilling: bitgleich bis 4349, RMS 734..764, letzte Abweichung Ende + 74) bzw. Raumwechsel (Zwilling alte exe: bitgleich bis 7190, RMS ~749, ab 8720 bitgleich), Form = SUB_25 SEQ0 (Periode 4,00 s); Elza Korrelation 0,515 (Gegenprobe 0,003), in 20B1 0,017. Voiceover: Aufnahme ausserhalb des Alarms bitgleich der Abnahme-5-Aufnahme, in der main46.wav mit 0,903 korrelierte; `[voice] Start ROOM2030/2031 Id 46` |
| Nachtrag 16 (Ventil-Sicherungskasten 2010) | **erfuellt** (Code seit Abnahme 5 unveraendert; Regression) | KA3 Leon: 40 -> 41 -> Ja -> Bit 125 -> Standbild Cut 7 + Aufschliess-Ton -> 42; KN_ELZA: nach der Flut 43 -> 44 (Bit 125 = 0) -> zu Fuss bis 2061 -> "Will you use the Fuse?"; Riegel `_weg`/`_weg_elza` (k1/k2/k3, v2a) gruen |
| Nachtrag 18 Punkte 1-3 (Plakat-Stil, 2050-Hebel gehen runter, RE2-Dreh-Animation) | **erfuellt** (Code seit Abnahme 5 unveraendert; Regression) | KB/KB_ELZA: RBJ von RE2 ROOM1080 gebunden, Ventil-Folge wie Abnahme 5; KH/KH_ELZA Hebel 3/1/4 -> 80 V; Riegel Unit F/Q + `_weg` v2p gruen; Urteile der Abnahme 5 bleiben |
| Nachtrag 19 (Text 46 woertlich) | **erfuellt** | `[msgtext] room=2030/2031 id=46 "WARNING: The water will rise in one minute. Please leave the room immediately!"` (KB, KB_ELZA, KF, KF_ELZA) |
| Abnahme 5 Mangel 1 (Alarm fehlt) | **behoben** | siehe Nachtrag 18 Punkt 4 (KB/KF/KB_ELZA/KF_ELZA, Zwillinge) |
| Abnahme 5 Mangel 2 (Countdown nicht in der RE2-Form) | **behoben** | siehe Nachtrag 18 Punkt 4 (eigener Nachbau, RE2-VRAM, G1 Nr. 1-3) |
| Abnahme 5 Mangel 3 (Kopf re15_ventilkette_r41.h) | **behoben** | Z. 146-149 "Bit 102 ... UND Bit (9,125) ... Satz 49 = Original 'It's locked.'", Z. 162 "Fundort neben der Sicherung", keine veralteten Reste (grep) |

## 5. Maengel (nummeriert, nachpruefbar)
**Keine.** Alle sechs Nutzer-Punkte und die Spur-C-Nachtraege sind am gebauten Stand erfuellt (Leon UND Elza), die drei Maengel der Abnahme 5
sind behoben, die Gates G1-G3 halten.

Hinweise (kein Mangel, an den Orchestrator):
* (V1, formal, aus Abnahme 5) `platform/pc/main.c`: zwei RBJ-Haken der Spur C (Boot- und Tuer-Ladeweg) ueber die in C-E3 angenommenen "4 Stellen"
  hinaus - bitte wie J-E1 annehmen oder ablehnen.
* (V2, formal, aus Abnahme 5) Bank-9-Bit **125** (Ventilkasten) im master-VERTRAG eintragen ("125-133 vergibt der Orchestrator").
* (V3, formal, neu) `platform/pc/src/audio_pc.c`: 4 markierte Einzeiler + der Schalter `re15_audio_r41_alarm` (~20 Codezeilen am Dateiende,
  braucht die statischen BGM-Zustaende) - groesser als "1-5 Zeilen" (VERTRAG §1.3); C-E4 erlaubt "einen kleinen, markierten Haken" -> bitte
  bestaetigen. Bei der Integration mit Spur D (audio_pc.c) beidseitig mergen.
* (a) Alarm-Lautstaerke: uebernommen ist der Betretens-Wert RE2 ROOM6040 sub23 @0x038BA (`51 01 00 01 64 40`, ProgAtr[0].mvol 99); RE2 stuft
  Programm 0 in 6040 je Kamera (sub25 @0x038E6..@0x039B8: 0x64 x6, 0x5a x5, 0x50 x2, 0x3c, 0x46, 0x7f - selbst gelesen). Eine Abbildung der
  RE2-Kameras auf die 2030-Kameras gibt es nicht; der haeufigste Wert ist der uebernommene. Der Nutzer kann eine Staffel verlangen.
* (b) Paket-Gate: `shared_assets/RE2/SOUND/SUB_25.BGM` (100688 B, md5 ef66256d...) in die Paket-Pruefliste aufnehmen (Dossier §7 nennt es);
  die Countdown-Schrift ist die schon gepackte `shared_assets/RE2/TEX.TIM`.
* (c) Countdown und Alarm nur auf dem PC (Bildebene/BGM-Lader der PC-Plattform); das PSX-Ziel ist weiter ungebaut (bekannt).
* (d) Laufzeit: die sechs registrierten r41-Riegel brauchen zusammen ~92 min RUN_SERIAL (hier in zwei Stroemen 48 + 47 min) - fuer die
  Release-Suite einplanen. Unter Last brauchte die Raumgruppe deutlich laenger als in Abnahme 5 (itemband_lift 1394 s statt ~860 s), ohne Rot.
* (e) Die Fensterfotos `nb6/re2cap/*/fenster_*.png` der Nachbesserung zeigen nicht RE2 (andere Fenster im Vordergrund); der Beleg der RE2-Anzeige
  ist die VRAM der Resume-Staende, hier selbst gelesen. Die Nutzer-Spielstaende `SLUS-00748*` sind unversehrt (md5 + Zeitstempel).
* (f) Die 20B0-Auswasch-Szene selbst braucht weiter den AUTO-Haken der Spur A (C-E1); `_kette` im C-Baum nicht registriert, von Hand rc=0.
