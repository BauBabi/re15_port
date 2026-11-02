# Spur GR — Handgranate zerfetzt Zombies (Runde 41, 2026-11-02)

Zweig `r41/granate`, Baum `.claude/worktrees/r41_granate`, Basis `r41/spinnengore` 2f886e91a.
Dossier wird FORTLAUFEND geschrieben (Sitzungslimit-Schutz).

## 0. Auftrag (Nutzer, woertlich, 2026-11-02)

> Und die Granate sollte laut den Videos der 80% Version die Zombies zerfetzen, also die normale, richtige
> Handgranate. Ausserdem ist sie noch zu schwach, die sollte auch die dicken Zombies zerfetzen

Gore-Regel (bindend): Gore = Maximum aus RE1.5-Disc, RE2-Retail und 80%-Videos. Gestaltung = PORT-WAHL,
jeder Einzelmechanismus mit @0x-Beleg.

## 1. Welches Item

* "normale, richtige Handgranate" = **Item 0x09 Hand Grenade** (Namensleser FUN_80028840, Runde 30;
  BAUPLAN Runde 34 E1). Die anderen beiden Granaten sind 0x0A Acid / 0x0B Incendiary. Der Granatwerfer ist
  die Waffe 0x0F (RE2-Explosivrunde, `SOUND/ARMS0F`).
* Explosion: ESP-Routine 31 @0x8001854c, Zuender 7 @0x8001856c, Resolver `FUN_80012d60(500, &P, 2)`
  (`ori a0,zero,0x1f4` @0x80018598, `addiu v0,v0,-500` @0x800185a8, a2 = 2 @0x800185b4).
  Art 2 -> Schaden DAT_8006f418[2] = 1000 (@0x8006f41c), Waffen-Id DAT_8006f430[2] = 9 (@0x8006f432).
* Port: Reichweite/Kandidaten RE2 Op 47 (`granate_r35.c re15_granate_r35_explosion`), Anwendung RE1.5-Gegnerzweig
  (`re15_resolver_gegnerzweig`, re15_damage.c), Stempel `re2_gl_explosion_stempel` (Zombie-Familie: Zeile 9,
  K0, Zone 1 = Spalte 1 -> DEATH[9][1] = 0x80108BEC Zerreissen, Runde 35 Spur A §3.5).

## 2. Video-Befund (80%-Material)

Material: B = `Version_80%/Resident Evil 1.5 (All Footage).mp4` (320x240, 30 B/s), A = `Version_80%/Biohazard 1.5
Final Build.mp4` (Diashow), CD1-4 = `documentation/Biohazard 1.5 Complete Disc Video N of 4 (360p).mp4`
(laut Spur S §1 ausser Vorspann/Abspann vollstaendig in B enthalten). Ausgangspunkt: die Gore-Sichtung der
Spur S (`S_video_gore.md` §2.1, Z08-Z11, Z14, Z21-Z24), hier jede Explosionsstelle neu am Einzelbild auf die
WAFFE geprueft (Werkzeug `scratchpad/GR/strip.py`, 5-30 B/s, Ausschnitte 2-2,5-fach).

### 2.1 Es gibt im 80%-Material keinen Handgranaten-Wurf — die "Granaten" sind Elzas Granatwerfer

| Stelle | Was man sieht | Waffe | Beleg |
|---|---|---|---|
| B 3:30.8-3:31.8 (3000 Cut 2) | Elza steht hinten, Waffe an der Huefte; **Muendungsblitz 3:30.9-3:31.0** an ihrem Rumpf; 0,3 s spaeter (3:31.3-3:31.4) platzt der Vordergrund-Zombie (weisses Hemd) zur roten Wolke mit Feuer, ab 3:31.6 ist er weg, am Boden Brocken/Blut | Granatwerfer (Schuss, kein Wurf, kein Bogen) | `GR_belege/video/v01_B0330_elza_muendungsblitz_dann_zombie_zerplatzt_300.jpg` |
| B 3:35.5-3:37.2 (3010 Draufsicht) | 3:35.5 Elza mit langer Waffe im Anschlag; 3:36.0 Blitz bei Elza, der RPD-Zombie vor ihr wird getroffen; 3:36.1-3:36.2 rote Saeule/Wolke ueber dem ganzen Koerper; ab 3:36.3 **ist der Zombie weg**, es bleiben eine grosse rote Lache und ein grauer Brocken | Granatwerfer | `v02_B0335_3010_elza_schuss_zombie_weg_lache.jpg` |
| B 8:05.2-8:08.0 (1070 Cut 3) | Muendungsblitz an Elzas Huefte 8:05.4 und 8:06.6, jeweils 0,2 s spaeter Explosion am Zombie vor der Kamera | Granatwerfer | `v03_B0805_1070_elza_muendungsblitz_explosion.jpg` |
| B 8:40.5-8:44 (1070 Cut 7 Draufsicht) | Elza zielt waagrecht mit langer Waffe, weisser Blitz an der Muendung 8:42.1; Explosion am Zombie in der Lache 8:42.1-8:42.3; danach **nur noch ein roter Fleischhaufen in einer grossen runden Lache** | Granatwerfer | `v04_B0840_1070_elza_schuss_haufen_in_lache.jpg` |
| B 5:18-5:23.2 (Tunnel, nicht auf der Disc) | Der Khaki-Zombie haengt ueber der Bruestung, 5:23.0 rote Wolke + weisse Wolke + helle Brocken, danach ist er weg | Schuetze nicht im Bild, Waffe offen | `v05_B0518_tunnel_zombie_zerfetzt_schuetze_unsichtbar.jpg`, Spur S g08 |
| B 7:50.8-7:51.6 (1030 blau) | orange Funken/Brocken am Zombie | offen (kein Schuetze im Bild) | Spur S g30 |
| A 0:00-2:07 | Diashow-Standbilder, keine Wurfpose, keine Granate in der Hand | – | `scratchpad/GR/a_*.jpg` (nicht im Repo) |

Kennzeichen eines Handgranaten-Wurfs (Port/RE1.5: Wurfclip W09 Clip 7/9/11, 35 Bilder, Flug mit Taumel-Sprite,
Abpraller, Explosion erst 36 Bilder nach dem Liegen = >= 1,2 s nach dem Aufschlag; BAUPLAN Runde 34 §1.1) kommt
an keiner Stelle vor. Alle Explosionen mit sichtbarem Schuetzen haben Muendungsblitz an der Huefte und
Einschlag 0,2-0,5 s spaeter ohne Bogen = Granatwerfer.

**Folgerung:** Das Video belegt, was eine **Explosivgranate** mit einem Zombie macht (Koerper verschwindet in
roter Saeule/Wolke, Lache + Brocken/Fleischhaufen bleiben). Dass die **Handgranate** dasselbe tut, ist die
Uebertragung nach der Gore-Regel des Nutzers (selbe Sprengladung, Gore = Maximum) — **PORT-WAHL**, nicht Video.
Ein dicker Zombie (Typ 0x11/EM11) wird im Video an keiner Explosionsstelle getroffen (3000/3010/1070: RPD- und
Hemd-Zombies). "Auch die dicken zerfetzen" ist damit Nutzer-Vorgabe, kein Videobefund.

### 2.2 Was das Video beim Zerfetzen zeigt (Soll-Bild)

1. Einschlag: rote Wolke/Saeule ueber dem GANZEN Koerper (3:31.4, 3:36.1-2, 8:42.3), mit Feuer/oranger Wolke.
2. Der Koerper ist danach weg (3:31.6, 3:36.3, 5:23.2) bzw. nur noch ein roter Haufen (8:42.5ff).
3. Es bleiben: grosse rote Lache (3:36.3ff, 8:42.5ff), ein grauer Brocken (3:36.4), Blut/Reste am Boden (3:31.8).
4. Einzelne fliegende Teile sind bei 320x240 als helle Brocken zu sehen (5:23.0-5:23.2), Gliedmassen als
   solche nicht unterscheidbar.

## 3. Ist-Stand VORHER (gemessen, echte exe, Messkopie `re15_pc_r41GR_vorher.exe` = Basis 2f886e91a)

Lauf `v1_leon_vorher` (Werkzeug `scratchpad/GR/cap.ps1`: eigene PID, gdigrab am Fenster-Handle, beschleunigter Renderer
direct3d, ohne AUTOSHOT/SOFTWARE_RENDER, 10 Bilder/s, `RE15_WINDOW_SCALE=2`). Echter Titel + echte Spielerwahl
(`RE15_TITLE_CONFIRM_MS=1500`, `RE15_PSELECT_AUTO`), dann `RE15_DEBUG_JUMP=1140@250`, Leon (-1676,-18070) Blick 1076,
Standard-KI (`RE15_AI_FLAVOR=re2`), Modelle wie in der Nutzer-Sitzung (Hybrid, `RE15_AI_MODELS=re15`, ZA-Dossier §6),
Handgranate (`RE15_GIVE=9:5`, `RE15_EQUIP=9`), TIEF-Wurf `MD0.6,MDA0.2,MD2.5,W5` (Aufstellung integration_r35_granate).

`state.log` (Bild nach dem Sprung), Explosion F119 (gr.log T=365, `resolver art=2 ... r=2000 eingriffe=4`):

| Slot | Typ | HP vor | HP nach | Zustand | Ausgang |
|---|---|---|---|---|---|
| 2 | 0x10 | 50 | -150 | 3 (Tod) | 0x80108BEC Zerreissen bzw. 0x80109610 Wegschleudern, Leiche F180 |
| 3 | 0x10 | 80 | -120 | 3 (Tod) | dito, Leiche bis F400 |
| 4 | **0x11 dick** | 250 | **50** | **2 (HURT)** | **ueberlebt**, F400 noch HP 50, greift weiter an |
| 5 | **0x11 dick** | 250 | **50** | **2 (HURT)** | **ueberlebt** (stirbt spaeter an anderem) |

Schaden = 200 = RE2-Granatwerfer-Zeile 9 Klammer 0 (`0x00a0c8c8` @0x800A41CC), seit Runde 34 E4 als Ersatz fuer die
RE1.5-Zahl. Brad/dicker Zombie 0x11 hat RE2-HP 250 (`addiu v1,zero,250` @0x801008C8) -> ueberlebt jeden Wurf mit 50.
Typ 0x16 bekommt sogar nur 80 (@0x800A4348) bei HP bis 143 -> ueberlebt meist ebenfalls.
Bild: `GR_belege/port/p01_leon_1140_vorher.jpg` — die zwei dicken Zombies (weisse Hemden) stehen nach der Explosion
weiter und gehen auf Leon los; die zwei normalen fallen (Sturz bzw. Zerreissen, Rumpf ganz).

## 4. Staerke — Herleitung (PORT-WAHL)

Quellen (alle selbst gelesen):
* RE1.5-Handgranate: `FUN_80012d60(500, &P, 2)` (`ori a0,zero,0x1f4` @0x80018598, `ori a2,zero,0x2` @0x800185b4),
  Art 2 -> `DAT_8006f418[2] = 1000` (`e8 03` @0x8006f41c). Gleich in `info/Re1.5/PSX.EXE` und `Version_40%/PSX.EXE`
  (eigener Vergleich: Tabelle `10,20,1000,1000,1000,50,100,200,300,1000,0`, Reaktionstabelle @0x8006f430 `3,3,9,10,...`).
* RE2: keine Handgranate (BAUPLAN Runde 34 E1, Musterscan). Die Zeile 9 ist die Granatwerfer-Explosivrunde
  (Zombie K0 200 / K1 50 / K2 10 @0x800A41CC; 0x16: 80 @0x800A4348). Staerkster RE2-Explosivwert am Zombie: Rakete
  Zeile 17 = 900 (`0x384E1384` @0x800A426C).
* HP: RE2-Zombie max 128 (@0x8010C670) + 15 (`sltiu ..,0x4` @0x801007FC / @0x80100810) = 143; **dicker Zombie 0x11 =
  Brad 250** (@0x801008C8). RE1.5-HP (`0x8011f034 + Typ*0x20`, STAGE1.BIN, Disc = Mod): 0x10 max 101, **0x11 max 107**,
  0x12 max 111, 0x13 max 88, 0x16 max 99, **0x18 einmal 1058** (Index 3 @0x8011f33a, sonst max 109).
* Video: ein Treffer = Zombie zerfetzt (Abschnitt 2).

**PORT-WAHL:** Die Handgranate (Art 2) traegt an der Zombie-Familie (re15_re2z_owns_type 0x10/0x11/0x12/0x13/0x16/0x18)
wieder ihre eigene Zahl **1000** (@0x8006f41c) statt der RE2-Granatwerfer-Zeile. Begruendung: Die Zeile 9 war eine
Zuordnung (RE2 kennt keine Handgranate), die 1000 ist der Wert des Gegenstands im Original; sie liegt ueber jeder
RE2-HP (max 250) und jeder RE1.5-HP ausser dem Einzelwert 1058 von 0x18 (nur ohne RE2-Modell, d. h.
`RE15_RE15_RE2Z_IMPORT=0` + RE1.5-KI; dort ueberlebt dieser eine Zombie mit 58, wie im Original). Die Regel
"HP und Schaden nur zusammen" (E4) wird damit fuer genau diesen Fall bewusst durchbrochen (Nutzer-Auftrag "zu schwach").
Nicht geaendert: Saeure/Brand (Art 3/4, E4 = 200), Werfer-Arten 6..9, alle Nicht-Zombies (Hund 300, Kraehe 60,
Spinne 60, G5 80 nach E16, Gorilla/Kakerlake/Alligator/Birkin 1000 RE1.5, Gator-Boss Modul).
Code: `re15_damage.c re15_enemy_take_damage_at` (Block "RUNDE 41 SPUR GR").

## 5. Zerfetzen — Umsetzung (GESTALTUNG / PORT-WAHL, Einzelmechanismen mit @0x)

Keine der drei Quellen nimmt dem Zombie alles auf einmal (RE2 Zeile 9 DEATH[9][1..2] = 0x80108BEC: Kopf + Arm(e),
Rumpf bleibt; RE1.5 0x80107ee0 ZERFETZEN: Kopf + Arme; RE1.5 0x80108abc BEINE WEG: Beine). Das Video zeigt aber den
ganzen Koerper weg und Reste in der Lache. Gebaut ist die Vereinigung, je Teil ein belegter Mechanismus (Code-Block
"RUNDE 41 SPUR GR" ueber `re2z_granate_zerfetzen`, enemy_ai_re2_zombie.c; RE2-Slots nach den Parser-Bindlaengen):

| Teil (RE2-Slot) | Was | Mechanismus / Beleg |
|---|---|---|
| Kopf 8 | fliegt weg, Halsfontaene | Drift 0x4A, Felder 0x80108BEC @0x80108DD4-E3C, Effekt 3000 @0x80108E18; Fontaene 0x00021F40 @0x80109428-38 |
| Oberarme 9/12 | fliegen (beide) | Wurf 0x1062, Felder @0x80108E54-E98 / @0x80108F10-F70 (RE2 Arm B nur 50 % @0x80108F04-08 -> hier immer) |
| Unterarme 10/13 | platzen in Scherben | RE1.5 FUN_80108d04 (@0x80108d04-6c: Blut 0x2000, +0x68 = 0x8f, Wort |= 0x4a), wie ZERFETZEN @0x8010818c/@0x80108224 |
| Haende 11/14 | weg | RE1.5 Teilwort 0 @0x801081d0 / @0x8010826c |
| Oberschenkel 2/5 | fliegen | Wurf 0x1062 mit den RE2-Beinwurf-Feldern @0x80105384-C0, Effekt 7000 @0x801053CC |
| Unterschenkel/Fuesse 3/4/6/7 | platzen | RE1.5 BEINE WEG @0x80108ba0 / @0x80108bb8 / @0x80108bd0 / @0x80108be8 (RE1.5-Teile 2/3/5/6 -> RE2 3/4/6/7, re2_hybrid_perm) |
| Rumpf 0 + Becken 1 | bleiben liegen | Spreng-Russ FUN_8010640C (Zeile 9, @0x80108CEC; Flammen weiter aus, Runde 36 D), Stumpf-Marker Part 0 @0x80108F98-B4, Rumpf-Stumpf Record 16 @0x80108FB8-FDC (nur RE2-MD1) |
| Blut/Brocken | Wolken, Fleischbrocken, Lache | FUN_80109610 P0 (8096/6096, n = (rand&3)+1 Brocken 0x0902xxxx @0x801096E8-0x801097B8), Brocken 0x0800xxxx + Wolke 8000 @0x80108FF0-0x80109054, grosse Lache (2 x 16 Spritzer, wie der fruehere Zeile-9-Zusatz) |
| danach | Sturz zur Leiche | 0x80108530 (`re2z_death_main`, wie re2z_80_zerfetzen) -> Zustand 7 |

* Ausloeser: DEATH-Wurzel mit Zeile 9 (Handgranate w9 und Granatwerfer-Explosivrunde w15, `re2z_row_from_weapon`), in
  jedem Zweig (stehend, liegend, Kriecher), genau einmal (Latch `zg_9f` Bit 0x08). Der Kriecher-Wiederbeleber
  0x80107438 (Spalte 0) wird damit fuer Zeile 9 nicht mehr erreicht — die Handgranate stempelt ohnehin Spalte 1.
* Farbwort der platzenden Teile: RE1.5 +0x68 = 0x8f; bei aktiver RE2-Bruecke nimmt der Zeichner das RE2-Farbwort
  +0x70 (main.c `gore_on`), deshalb steht dort derselbe Wert (Port-Mapping, beide multiplizieren das NCCT-Ergebnis).
* RE1.5-Gehirn (Flavor re15, `zombiegore_r15.c z_granate`): w9 -> ZERFETZEN (@0x8010800c-0x8010826c, ohne den
  Muenzwurf ZERFETZEN/OBERKOERPER @0x801080c8-d4) + BEINE WEG (@0x80108ba0-c50), Leiche statt Kriecher. Saeure/Brand
  (w10/w11) behalten die fruehere Gestaltung (Oberkoerper ab, kriecht weiter) — nicht Teil dieses Auftrags.
* Schalter wie alle 80%-Gestaltungen: `RE15_KEIN_80PROZENT_GORE=1` -> RE2 0x80108BEC wie vorher. Die Staerke (1000)
  haengt NICHT am Schalter.
* Nicht gebaut / nicht erfunden: das Verschwinden des Rumpfes (keine Quelle hat es; der Rumpf bleibt als Brocken in der
  Lache — Video 8:42.5 zeigt genau so einen Haufen). Die fliegenden RE2-Teile verschwinden nach dem zweiten
  Bodenkontakt (Kaskade @0x80028CA0), die RE1.5-Scherben nach 0x1d Bildern (@0x8001f208-214) — beides Original.
