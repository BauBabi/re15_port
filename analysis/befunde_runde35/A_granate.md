# Runde 35 — Spur A: Granate (Wand, Reichweite, Sound, Brutalitaet, Tests Birkin/Alligator)

Stand: 2026-10-03, Zweig `r35/granate`, Baum `.claude/worktrees/r35_granate`, Basis master 154a73c1.
Dossier wird FORTLAUFEND geschrieben (Sitzungslimit-Schutz). Auftrag: `AUFTRAG.md` (woertlich), Vertrag: `VERTRAG.md`.
Werkzeuge dieser Spur: `analysis/befunde_runde35/A_werkzeug/` (sca_pfad.py). Messlaeufe (unversioniert):
`re15_port/build/r35a_mess/<lauf>/` mit der byte-gleichen Messkopie `re15_pc_r35a.exe` (gegen fremde Kills).

## 0. Auftrag (Wortlaut des Nutzers)
1. Granaten sollen nicht durch die Wand fliegen
2. Die Range der Granaten-Explosion ist viel zu niedrig, es ist fast unmoeglich Gegner damit zu treffen
3. Die Granate hat den falschen Explosionssound
4. Eine normale Handgranate beim Zombie etc. darf mehr Brutalitaet haben — mehr Zerplatzen, abplatzende Beine, Arme
5. Die Tests bei Birkin und Alligator hinzufuegen (Explosion an Birkin und am Alligator)

## 1. Ausgangslage (aus Runde 34, `analysis/befunde_runde34_granaten/`, selbst nachgelesen)
* Routinen 30 (@0x8001843c Wurf-Init), 29 (@0x80018320 Flug/Abprall), 31 (@0x8001854c Zuender/Explosion)
  sind im Port byte-true (re15_esp.c `case 30`, `case 31`, `esp_fx_dispatch_b_29`).
* **Boden/Wand im Original:** R29 testet NUR `lh t1,42(t0)` / `blez t1` @0x80018330-38 (Welt-y > 0).
  Weder R29/R30/R31 noch der Tick FUN_80019e20 rufen die Raumkollision (FUN_8001c6e8) -> KEINE Wandkollision,
  Wurfweite HOCH ~18760 / MITTE ~11929 / TIEF ~1543 Einheiten (Wurf-GP §7). RE2 hat kein Gegenstueck zur
  Handgranate (Konstanten 0x17c/0x118/-110 in 0 von 22 RE2-Dateien, Wurf-GP §9) — aber ein vollstaendiges
  Geschoss-System fuer den Granatwerfer (FUN_8001ED9C + Op 47), s. §3.
* **Explosion:** FUN_80012d60(a0=500 `ori a0,zero,0x1f4` @0x80018598, P = (x, Welt-y-500 `addiu v0,v0,-500`
  @0x800185a8, z), a2 = 2 @0x800185b4). Treffertest FUN_8002b5d0: waagrecht dist < r_Ziel + 500, senkrecht
  -h < dy < h mit h = 500 + Kastenhoehe (@0x8002b768-78). Zombie-Kasten {0,-1440,0,400,1440,400} (STAGE1.BIN
  @0x1f778) -> Trefferzylinder um die Liegestelle: **Radius 900**, Hoehe ausreichend.
* **SE:** 0x04080001 (`lui a0,0x408` / `ori a0,a0,0x1` @0x800185e4-e8, `jal 0x80045024` @0x800185ec) = Bank 4 (CORE)
  Satz 8 -> CORE00.EDH @0x20 `00 00 93 00` -> VAG 6 (5904 B, ~0,47 s). Der Nutzer: falscher Explosionssound ->
  Sound ist RE2 (VERTRAG §2.2).
* Explosion an stehendem Zombie haengt im Original (80106c00 jalr v0=0) -> Port: RE2-Reaktion. Heute: RE2-Stempel
  Zeile 9, Spalten-Klammer K1 (`re2_gl_explosion_stempel`, re15_damage.c: `k_spalte = 1` fuer die Zombie-Familie)
  -> Spalte 3/4/5 -> DEATH[9][3..5] = `RE2ZD_MAIN` 0x80108530 = schlichter Sturz (Clip 1/2). KEINE Gore.

## 2. Messung vorher (gebauter Stand 154a73c1, exe `re15_pc_r35a.exe`, ROOM1140 per RE15_DEBUG_JUMP=1140@250)

### 2.1 W1 — MITTE-Wurf gegen die Wand (`r35a_mess/w1_mitte`)
Leon (-4311,-19289) Blick 0, RE2-KI, Skript `M0.6,MA0.2,M2.5,W5`, RE15_GRANATE_LOG=gr.log.
* Spawn F=41 Anker (-3438,-2474,-19797) gier=79; Flug bis x=8612 (12050 Einheiten in +x).
* `A_werkzeug/sca_pfad.py <baum> 1140 gr.log` (ROOM1140: 95 SCA-Zellen, 70 solide Rechtecke, Bbox x[-10600..16700]
  z[-25600..1850]):
  ```
  T=348 F=102 wpos=(8345,-11,-19758) frei
  T=349 F=103 wpos=(8385,-12,-19739) IN-WAND [x8350 z-20750 w4100 d1150 typ01 u0ff]
  ...  (7 Bilder F103..F109 in derselben soliden Zelle, Abprall-SE T=351/354 DARIN)
  T=356 F=110 wpos=(8568,-4,-19592) frei      (Zelle in z verlassen)
  T=360 F=114 wpos=(8612,9,-19500) liegen -> Explosion T=396 P=(8612,-491,-19500) eingriffe=0
  ```
  **Befund 1 bestaetigt:** die Granate fliegt durch eine solide Zelle (u0=0xff, Typ 1) hindurch und prallt
  darin am Boden ab; kein Code im Flug (R29) oder Tick prueft die SCA. Kein Gegner getroffen.

### 2.2 W2 — TIEF-Wurf an den Zombie (`r35a_mess/w2_tief`, Aufstellung aus integration_r34_granaten g9_re2)
Leon (-1676,-18070) Blick 1076, Skript `MD0.6,MDA0.2,MD2.5,W5`, RE15_STATE_LOG + RE15_WAFFEN_LOG.
* Liegen T=329 (-2199,20,-20548), Explosion T=365 = F119, P=(-2199,-480,-20548), r=500, `eingriffe=1`.
* Zombie Slot 3 (0x10, HP 80) bei (-1529,-20930): Abstand zu P waagrecht 771 (< 900) -> Treffer.
* **Sound (Befund 3, vorher):** wf.log Zeile 379 `SE  esp code=0x04080001 bank=4 satz=8 -> CORE` = RE1.5 CORE00
  Satz 8 (VAG 6, 5904 B).
* **Brutalitaet (Befund 4, vorher):** state.log Slot 3:
  ```
  F118 [3 t=10 st=1 ss1=1 ss2=1 mo=0 ...] hp=80
  F119 [3 t=10 st=3 ss1=9 ss2=0 mo=0 ...] hp=-120     (Schaden 200 = RE2-Record Zeile 9 K0, E4)
  F120 [3 t=10 st=3 ss1=9 ss2=1 mo=1 af=0 ...]         (Clip 1 = Sturz-Tod RE2ZD_MAIN 0x80108530)
  F180 [3 t=10 st=7 ...]                               (Leiche)
  ```
  = schlichter Sturz, keine Teile, kein Zerreissen.
* **Reichweite (Befund 2, vorher):** Treffervolumen = Zylinder r = 400 (Kasten) + 500 (a0) = **900** um die
  Liegestelle, nur fuer den TIEF-Wurf (1543 weit) ueberhaupt im Raum; MITTE/HOCH (11929/18760) landen durch
  Waende ausserhalb (W1). Ein Zombie, der 1,2 s nach dem Liegen (Zuender 42-7 = 36 Bilder @0x80018474/8001856c)
  nicht innerhalb 900 steht, wird nicht getroffen -> "fast unmoeglich" (Nutzer) erklaert sich aus Reichweite
  900 + fehlender Wand.

## 3. RE-Belege

### 3.1 RE1.5 ist beim Flug UNFERTIG (Wand) -> RE2 Retail ist das Ziel (VERTRAG §2.2)
RE1.5 R29 @0x80018320-434 (eigene Disassembly, Wurf-GP §1.2): einziger Test `lh t1,42(t0)` / `blez t1`
@0x80018330-38; kein `jal 0x8001c6e8` (Raumkollision), kein `jal 0x8003b0a4`. Das Geschoss-System von RE2
(FUN_8001ED9C, `re2_disasm.py dis 0x8001ed9c`) hat beides:

| Was | RE2-Adresse | Instruktionen |
|---|---|---|
| Boden/Wand-Test | @0x8001ee60-a0 | `addiu a0,sp,16` (Lage), `addiu a1,zero,2` (Radius), `addiu a2,zero,8192` (Klassenmaske 0x2000), `addu a3,zero,zero`, `jal 0x8004fba0`; FUN_8004fba0 loescht `sw zero,-13368(at)` 0x800dcbc8 @0x8004fc34 (Wand-Flag) und setzt es (KORRIGIERT in Nachbesserung 1, vollstaendige Tabelle N1.1): bei y > 0 (@0x8004fc48-58), an Objektkaesten im Hoehenfenster (@0x8004fcfc-0x8004fd18) und an Zellen NACH Klassenmaske (@0x8004fdc0-d0), Formtest (Tabelle 0x80011104 @0x8004fe34-54) und Hoehenfenster der Zelle (@0x8004ffd4-0x8005000c) — kein blosser "Zellkontakt" |
| Wand beruehrt | @0x8001ef80-8c | `lw v0,-13368(v0)` (0x800dcbc8) / `beq v0,zero,0x8001f10c` (frei -> weiterfliegen) |
| Rueckzug | @0x8001ef90-0x8001f0c8 | `lui t1,0x5555 / ori t1,t1,0x5556` (/3); vel.x -= acc.x (`lbu v1,8 / lhu v0,12 / subu / sh v0,12(t2)` @0x8001ef98-efd8), nochmals `subu v0,v0,a2 / sh v0,12` @0x8001f000-04; pos.x (+0x24) -= vel.x (`lhu v1,36 / subu v1,v1,a0 / sh v1,36` @0x8001f00c-24); pos.x -= vel.x/3 (`mult v0,t1 / mfhi t3 / sra a2,a2,31 / subu a2,t3,a2 / subu v0,v0,a2 / sh v0,36` @0x8001f018-0a8); y/z gleich (+0x26/+0x28, @0x8001f028-0c8) |
| Lage neu | @0x8001f0cc | `jal 0x8001d894` |
| Explosion SOFORT | @0x8001f0e0-104 | `lbu v0,3(v1)` (step[3] = 47) / `lb v1,27(v1)` (+0x1B Art) / `addu / sll 2 / lw v0,-10136(at)` (Optab 0x8009d868) / `jr v0` -> Op 47 im SELBEN Bild |

Kein Abprallen an der Wand: die RE2-Runde wird um (vel - acc) + (vel - 2acc)/3 aus der Wand zurueckgezogen
und explodiert sofort. Das ist der Retail-Mechanismus "Granate trifft Wand".

### 3.2 RE2 Gegner-Kontakt im Flug (Teil desselben Geschoss-Systems)
FUN_8001ED9C @0x8001ee90-ef14: Hitcode `lui s2,0x3 / ori s2,s2,0x9` (0x00030009) + Art (`lb a3,27(v1)` /
`addu a3,a3,s2` @0x8001eed0/dc); Box = sp+32 = 8 Byte aus @0x80010900 `88 fa 00 00 5e 01 fa 00` =
**{-1400, 0, 350, 250}** (`lui a2,0x8001 / addiu a2,a2,2304` @0x8001edb8-bc); Pruefhoehen y+1000 (`addiu v0,v0,1000`
@0x8001eec8, `jal 0x800470c0` @0x8001eed8) und y-1000 (`addiu v0,v0,-2000` @0x8001eef8, jal @0x8001ef08);
`bne s0,zero,0x8001ef38` @0x8001ef14 -> Status |= 0x80 @0x8001ef44-50 -> `lbu v0,2(v1)` @0x8001ef74 / `j 0x8001f0e4`
= Op aus step[**2**] + Art im selben Bild (KORRIGIERT in Nachbesserung 1: nur die WAND springt ueber step[3] @0x8001f0e0;
welche Op step[2] der Explosivrunde traegt, ist nicht gelesen — der Kontakt ist im Port nicht verdrahtet).
Port: der Kontakt ist nur AUSLOESER der Explosion (PORT-WAHL: die Handgranate ist EIN Geschoss, RE2 feuert
fuenf (5 x 0x020C0A00, Saeure/Brand-GP §2.2); der Flug-Stempel K0 wuerde den beruehrten Gegner nach DEATH[9][0]
= 0x80107438 als Kriecher HP 10 wiederbeleben (Runde-34-Fund K1) — die Explosion selbst (§3.3/3.5) trifft ihn).

### 3.3 RE2 Reichweite der Explosion — Op 47 @0x80020c3c (`re2_disasm.py dis 0x80020c3c 140`)
```
80020c58 lui a2,0x8001 / 80020c5c addiu a2,a2,2328     a2 = 0x80010918 (16 Byte Boxen -> sp+32)
80020d30 lbu v1,30(v1) / 80020d34 addiu v0,zero,12 / 80020d38 bne v1,v0,0x80020dc0   +0x1E == 12 (Explosiv-Runde)
80020d40 lui a0,0x111 / 80020d44 ori a0,a0,0x1 / 80020d48 jal 0x8005ba28           SE 0x01110001 an der Lage
80020d54 lui a3,0x1002 / 80020d58 ori a3,a3,0x9                                     Hitcode 0x10020009 (Zeile 9, K1)
80020d68 lbu a2,30(v0) / 80020d70 sll a2,a2,3 / 80020d74 addiu a2,a2,-96 / 80020d7c addu a2,s0,a2   Box = sp+32 + (12*8-96) = Box[0]
80020d78 jal 0x800470c0                                                             Applier an P
80020d98 addiu v0,v0,900 / 80020d9c sw v0,20(sp) / 80020db0 jal 0x800470c0           Applier an P + 900 (y)
```
Box[0] @0x80010918 = `30 f8 00 00 e8 03 f4 01` = **{-2000, 0, 1000, 500}**; FUN_80041EF8 (Port `re2gl_box_test`,
re15_damage.c): Ecke = P + RotY(Gier)·(-2000, 0, -2000), Kanten 4·1000 = 4000 und 2·500·4 = 4000 -> Quadrat
**±2000 um P**, gedreht um die Gier. Band je Kandidat @0x8004716c-a4: `e->y + b98 + 100 + h9e - P.y` in
[0, 2·(h9e+100)). RE1.5: 500 + Kastenradius = 900 (§2.2). RE2-Retail: 2000. Beta->Retail auf die
NUTZER-BEOBACHTUNG ("fast unmoeglich") — die 500 der Beta sind die unfertige Bemessung der Handgranate, deren
Retail-Nachfolger die RE2-Explosivrunde ist.
Schaden bleibt RE1.5/E4 (unveraendert): RE1.5-KI 1000 (DAT_8006f418[2] @0x8006f41c), RE2-Modell = RE2-Record
Zeile 9 K0 (Zombie 200 @0x800A41CC, G5 80 @0x800A5F7C).

### 3.4 Sound — RE2 0x01110001 liegt als RE1.5-Datei vor (kein neues Asset)
* RE2 Op 47: `lui a0,0x111 / ori a0,a0,0x1 / jal 0x8005ba28` @0x80020d40-48 (Sub-Id 12 = Explosiv). FUN_8005BA28:
  Bank = Code>>24 = 1 = ARMS-Datei der ausgeruesteten Waffe (Id 9 -> ARMS09, `read 0x800a8118`), Record 0x11.
* RE2 `COMMON/SOUND/ARMS09.EDH` @0x44 (Record 17): `00 00 33 20` -> Ton 3 -> VAG 3 (12800 B) = `ARMS09_00001.wav`.
* RE1.5 `SOUND/ARMS0F.VB` (30416 B) md5 `786ad6910be7a9ea8bf1df0b145ad55b` == RE2 `ARMS09.VB` md5 (eigener
  Vergleich, s. §2 Werkzeuglauf); RE1.5 `ARMS0F.EDH` @0x28 (Record 0x0A): `00 00 33 20` = derselbe Deskriptor.
  RE1.5-ARMS0F ist die Bank der RE1.5-Waffe 0x0F (Granatwerfer) = Capcoms Kopie der RE2-Explosivbank.
* Port-Weg: `re15_audio_arms_zusatz_se(0x0F, 10)` (audio_pc.c, laedt SOUND/ARMSxx.EDH/.VB), genau wie E9 fuer
  Saeure/Brand (ARMS10/11 = RE2 ARMS0B/0A, Satz 10). Der Zusatz-Cache hat 2 Plaetze -> 3.

### 3.5 Brutalitaet — RE2 DEATH-Tabelle Zeile 9 @0x8010CD68 (`table 0x8010cc24 170 --bin EMOVL10_S0.BIN`)
`{2,5,5, 1,1,1, 1,1,1}`: Spalte 0 = 0x80107438 (Knockdown mit ARM-ABRISS, Todeszweig P2 belebt als Kriecher
HP 10: `8010778c jal 0x80015fe8 / 80107794 andi 0x3 / 801077b0 sh v0(=10),342(s2)`), Spalte 1/2 =
**0x80108BEC ZERREISSEN** (Kopf ab + Bein(e) ab, Russ `jal 0x8010640C` @0x80108CEC nur Zeile 9/17, Ausstieg 2
nur Zeile 9: zwei Wuerfe -> Wegschleudern 0x80109610 mit Fleischbrocken), Spalte 3..8 = 0x80108530 Sturz.
Der Port stempelt heute K1 (Spalte 3+Zone) = Sturz. RE2 selbst reisst nur beim Flugkontakt (K0, Zone aus der
Pruefhoehe). **PORT-WAHL (NUTZER-VORGABE "mehr Brutalitaet, Zerplatzen, abplatzende Beine/Arme"):** die
Explosion stempelt die Zombie-Familie mit K0 und Zone 1 (Rumpf) = Spalte 1 -> 0x80108BEC (Zerreissen) bzw. ueber
dessen Zeile-9-Wuerfe 0x80109610 (Wegschleudern). Zone 1 statt der RE2-Zonenregel (@0x800472b8-cc), weil P =
Liegestelle - 500 bei JEDEM stehenden Zombie (b98 = -1500: `e->y - 750 < P.y`) Zone 0 ergibt und Spalte 0 den
Kriecher-Wiederbeleber traegt (Runde-34-Fund K1, gemessen 24/24). Hund/Kraehe/Spinne/G5 bleiben K0 mit
RE2-Zonenregel (unveraendert; Hund K0 = "zerplatzt", Teile-Wurf @0x80104440).

## 4. Umsetzung (Commits 731c3263 + Folgecommits; jede Konstante traegt ihre Adresse im Code)

### 4.1 Neues Modul `engine/src/granate_r35.c` + `include/re15_granate_r35.h`
| Funktion | Mechanismus | Beleg |
|---|---|---|
| `re15_granate_r35_wand(f)` | (Stand Nachbesserung 1, N1.3) STRECKE vorige -> neue Weltlage (Wurfbild: Werfer -> Hand) gegen die soliden SCA-Zellen des Werfer-Bandes (Band = -(granate_boden/0x708) wie FUN_8001c2dc), formgenau Typ 1-9, je Quadrantenliste, Maske u0&1. Erster Stand (46976419): Punkttest im Begrenzungsrechteck (`re15_collision_box_blocked`) — Maengel 1/2 der Abnahme 0 | RE2 `jal 0x8004fba0(&Lage,2,0x2000,0)` @0x8001ee60-a0 setzt DAT_800dcbc8 (Bedingungen N1.1); `lw/beq` @0x8001ef84-8c. Maske 1 = PORT-WAHL; Strecke = PORT-WAHL zur Nutzervorgabe; Hoehe = Abweichung (OFFEN N1) |
| `re15_granate_r35_flug(f)` | (Nachbesserung 1) der EINE Haken der Routine 29: Wand -> Rueckzug -> Weltlage -> Platz um 500 gesenkt (P = Rueckzugspunkt) -> Liegezustand + Zuender 7 -> Routine A im selben Tick | RE2 @0x8001ef84-0x8001f104; Op 47 liest die Lage ohne Versatz @0x80020cdc-fc; RE1.5 @0x80018368-84, @0x8001856c, Versatz 500 @0x800185a8 |
| `re15_granate_r35_rueckzug(f)` | je Achse: vel -= 2*acc; xlat -= (vel-acc) + (vel-2acc)/3 (C-Division = 0x55555556-Idiom) | RE2 @0x8001efd4-0x8001f0c8 (Zitate §3.1) auf den RE1.5-Feldern +0x10/+0x08/+0x34 |
| `re15_granate_r35_flugtest(f)` | nur Wand (Kontakt NICHT verdrahtet, §4.2) | — |
| `re15_granate_r35_explosion(p,gier,art)` | Box {-2000,0,1000,500} an P und P+900, "alle" Kandidaten; jeder Platz hoechstens einmal; Anwendung `re15_resolver_gegnerzweig` (RE1.5 Gate B, Richtung, Schaden E4, Stempel) in umgekehrter Sammelreihenfolge; KEIN Spielerzweig | RE2 Op 47 @0x80020c58-5c / @0x80020d54-db0, Box @0x80010918; FUN_80012d60 @0x80012f12-30 (do-while rueckwaerts); FUN_800470C0 nur Gegnerliste @0x800470c4-0x8004740c |
| `re15_granate_r35_explosion_se(p)` | `re2fx_se_hook(0x01110001, P)` | RE2 @0x80020d40-48 |
| `re15_granate_r35_kontakt(f)` | RE2-Flugkontakt als Geometrietest (Box @0x80010900, Pruefhoehen y+-1000, Gates 1-4, Band, Box) — nur Sonde/Dossier | @0x8001ee90-ef14 |

### 4.2 Gemessen und NICHT verdrahtet: der RE2-Flugkontakt
Erster Stand (Laeufe `r35a_mess/n1_mitte`, `n2_tief`) hatte Wand UND Kontakt im Flug:
```
n1: F=41 SPAWN (-3438,-2474,-19797) ... T=291 EV kontakt wpos=(-2321,-2614,-19837) -> explosion sofort
    (4 Bilder nach dem Wurf, P=(-2321,-3114,-19837) = 3114 ueber dem Boden, eingriffe=2 nur ueber P+900)
n2: F=43 SPAWN (-2108,-772,-19007)  ... T=290 EV kontakt wpos=(-2114,-772,-19087)  -> explosion sofort
    (3 Bilder nach dem Wurf, 80 Einheiten neben Leon)
```
Ursache: die RE2-Box @0x80010900 ist ein gefegtes Volumen HINTER dem Geschoss (Ecke -1400, Kante 4*350 = 1400
-> lokal x in [-1400, 0] (+r/4 vorwaerts), z +-1400), das Band des Appliers ist 3200 hoch (b98 -1500, h9e 1500,
@0x8004716c-a4) — passend zur schnellen, flachen RE2-Runde. Auf dem Bogen der Handgranate (280/Bild, Scheitel bis
~3000) zuendet sie ueber jedem ueberflogenen Zombie in der Luft — das Gegenteil des Nutzerziels ("Gegner treffen").
Entscheidung: Kontakt NICHT verdrahten; der Zeitzuender bleibt RE1.5 (42 @0x80018474, Zuendung bei 7 @0x8001856c
= Liegen + 36 Bilder). Belegt im Code (granate_r35.c) und in der Sonde (unit_r35_granate 110-112: Geometrie meldet
den Gegner, der Flug geht weiter, die Wandexplosion trifft ihn ueber die Reichweite).

### 4.3 Haken in gemeinsamen Dateien (klein, benannt "Runde 35 Spur A")
| Datei | Zeilen | Was |
|---|---|---|
| `engine/src/re15_esp.c` | +10/-7 gegen master (Stand Nachbesserung 1; erster Stand +37/-8 = Mangel 7) | include (1 Zeile); Haken am Kopf von `esp_fx_dispatch_b_29`: `if (re15_granate_r35_flug(f)) return;` (1 Zeile); `case 31` Zuender 7: `re15_granate_r35_explosion` statt `re15_resolve_attack(r=500)` (4 Zeilen -> 1) + Logtext `r=2000`; SE `re15_granate_r35_explosion_se` statt `0x04080001` (2 Zeilen geaendert); drei Dienste fuer das Modul hinter `esp_fx_weltlage` (`re15_esp_r35_weltlage`, `re15_esp_r35_routine_a`, `re15_esp_r35_log`, 4 Zeilen). Rueckzug, Sofort-Zuendung und die gr.log-Zeile `EV wand ...` stehen in granate_r35.c |
| `engine/src/re15_damage.c` | ~60 | `re2gl_treffer_t` mit Struct-Tag; `re15_resolver_gegnerzweig` oeffentlich; `re2_gl_explosion_stempel`: `k_spalte = 0`, Zonen-Bits 0 fuer die Zombie-Familie (Zone 1 fest) — PORT-WAHL §3.5; neu `re15_re2_gl_kandidat()` (Tore 1-4 + Band + Box des Appliers ohne Anwendung, angehaengt hinter `re15_re2_gl_apply`) |
| `include/re15_damage.h` | 8 | Deklarationen |
| `platform/pc/src/fx_plattform_pc.h/.c` | ~12 | `RE15_PC_RE2FX_SE_EXPLOSIV 0x01110001`, `RE15_PC_ARMS_EXPLOSIV 0x0F`; Weiche + Log-Text; `re15_pc_force_explosion` -> neue Zustellung, P.y = Spieler-Boden - 500 (Gator-Boss liegt in GB_WATER_Y -1200 mit Kasten +1200 auf Deckhoehe; mit ziel->y verfehlte der Haken, gemessen int1 `Treffer=0`) |
| `platform/pc/src/audio_pc.c` | 2 | `ARMS_ZUSATZ_N 2 -> 3` (+ Initialisierer) |
| `platform/pc/main.c` | 2 | Harness-Ausgabe P.y |

Keine neuen Assets (ARMS0F liegt in shared_assets/PSX/SOUND), kein Patch an shared_assets/PSX.

### 4.4 Beta -> Retail, ausgeschrieben
* **Wand** (RE1.5 unfertig: kein Kollisionsaufruf im Flug) -> RE2 FUN_8001ED9C Wandregel (Rueckzug + sofortige Explosion).
* **Reichweite** (RE1.5 500 vollstaendig, aber Beta-Bemessung; Nutzer: "fast unmoeglich") -> RE2 Op 47 Box +-2000 + Band;
  Schaden unveraendert RE1.5/E4 (RE1.5-KI 1000 @0x8006f41c, RE2-Modell Zeile 9 K0: Zombie 200 @0x800A41CC, G5 80).
  Der Spieler-Eigenschaden (Wurf-GP §2.5, < 950) entfaellt mit der RE2-Zustellung: FUN_800470C0 kennt keinen
  Spielerzweig — und mit der Wandzuendung neben Leon waere RE1.5s 1000 ein Selbstmord gewesen.
* **Sound** -> RE2 0x01110001 (VERTRAG §2.2 "Sound ist RE2").
* **Brutalitaet** -> NUTZER-VORGABE; Form = RE2 DEATH[9][1] 0x80108BEC (Zerreissen) / 0x80109610 (Wegschleudern), Spalte 1 als PORT-WAHL (§3.5).
* **Flugkontakt** -> RE2-Mechanismus gemessen und begruendet NICHT uebernommen (§4.2).

## 5. Messung nachher (exe `re15_pc_r35a.exe` ab Bau 731c3263, gleiche Aufstellungen wie §2)

### 5.1 N1b — MITTE gegen die Wand (`r35a_mess/n1b_mitte/gr.log`)
```
T=347 EV se code=010a0301 pos=(8304,25,-19777) abprall
T=349 EV wand wpos=(8385,-12,-19739) -> rueckzug (8332,-8,-19764) -> explosion sofort
T=349 EV latch
T=349 EV resolver art=2 P=(8332,-508,-19764) r=2000 eingriffe=0
T=349 EV kind code=03195000 ...  /  T=349 EV se code=01110001 pos=(8332,-508,-19764)
T=354 EV kind 03195000 + 030b5400 (Zuender 2)  /  T=356 EV frei (Zuender 0)
```
wf.log: `SE  re2fx code=0x01110001 -> ARMS0F Satz 10` / `SE  zusatz ARMS0F satz=10`; kein `0x04080001`.
Vorher (W1): 7 Bilder in der Zelle, Liegen bei x 8612 jenseits der Wand. Nachher: Explosion im Eintrittsbild, Lage
8332 < 8350 (vor der Zelle). exe bis EXIT_AT.

### 5.2 N2b — TIEF an den Zombies (`r35a_mess/n2b_tief`)
* Liegen T=329 (-2199,20,-20548), Explosion T=365 = F119 = L+36 (Zeitzuender unveraendert), `r=2000 eingriffe=3` (vorher 1).
* state.log Slots 2 und 3 (0x10): F119 `st=3 ss1=9 hp=-150 / -120`; F120 `mo=2` (re2z_death_rip, clip2[] @0x80108C24-30;
  vorher mo=1 Sturz), Lage F120 (-1441,-19915) -> F126 (-1803,-21213) = ~1400 weggeschleudert (vorher Stillstand);
  F180 `st=7` Leiche, keine Wiederbelebung.
* Ton: `SE  re2fx code=0x01110001 -> ARMS0F Satz 10` genau einmal.

### 5.3 Reichweite (Sonde [2]): vorn 1200/1900/2300 Treffer, 2600 nicht; hinten 1900 Treffer, 2100 nicht; seitlich +-2300
Treffer, 2600 nicht; eine Etage hoeher (y -1800) nicht, eine Etage tiefer Treffer (Band/2. Pruefhoehe); drei Gegner in einem
Quadrat; RE2-Puffer-Wachstum (r/4 je Treffer, Ruecknahme nur im Nicht-Treffer-Zweig @0x800473dc-408) gemessen: vierter
Gegner bei 3000 nach drei Treffern erreicht, bei 4000 nicht. Spieler 300 neben P: HP 100 (kein Spielerzweig).

### 5.4 Bosse (Sonde [5], beide Flavors, Granate 300 und 1500 vor dem Boss; exe-Pins int2)
| Boss | Treffer | HP | Reaktion |
|---|---|---|---|
| Birkin 0x30 (RE1.5-KI, ROOM3080) | ja | 300 -> 150 (Mutations-Schutz, enemy_ai_common.c) | Zustand 3 -> 1 im Folgebild (wie Runde-34-Zensus 't'), kein Haenger |
| Birkin 0x36@3080 | ja | 300 -> 150 | wie 0x30 |
| G5 0x36@5090 (RE2-Modul, Kampfstart grid 0x13) | ja | 600 -> 520 (E16: RE2 Zeile 9 K0 = 80 @0x800A5F7C) | Zustand 2, Modul-Akku/Routine reagiert |
| Alligator 0x23 (RE1.5-KI) | ja | 300 -> -700 (1000 @0x8006f41c) | Zustand 3 verlassen in Bild 20, Leiche 7 |
| Gator-Boss 0x23@2090 (Modul) | ja | 3000 -> 2000 | Zustand 2, Modul setzt zurueck, kein Haenger |
exe-Pins (echte exe, RE15_DEBUG_JUMP + RE15_FORCE_EXPLOSION=2@60:<slot>): `[gator] 23 Slot 15: HP 3000 -> 2000, Zustand 2,
exe bis EXIT_AT`; `[birkin] 36 Slot 2: HP 600 -> 520, Zustand 2, exe bis EXIT_AT`.

## 6. Tests
* `unit_r35_granate` (tests/unit/test_r35_granate.c, probes/r35_granate.cmake): [1] Wand (100-112), [2] Reichweite
  (120-136), [3] Sound (150-159), [4] Gore (170-175: 6 Laeufe, Spalte +0x1D2 = 1, Handler 5 = 0x80108BEC, Leiche 7,
  0 Wiederbelebungen, 3 fliegende Teile je Lauf), [5] Bosse (180-189). `test_r35_granate: ALLE PRUEFUNGEN GRUEN`.
* `integration_r35_granate` (tests/integration/test_r35_granate.cmake): Laeufe wand, zombie, gator (ROOM2090 Slot 15,
  RE15_FORCE_EXPLOSION=2@60:15, Schaden 1000), birkin (ROOM5090 Slot 2, Schaden 80). Alle vier gruen (int1/int2):
  `Wand x=8385 -> 8332, Explosion Tick 349`; `X=119, 2 Zombies zerrissen, 3 Eingriffe`; Bosse s.o.
* Angepasst (verhaltensbedingt): `probe_r34_wurf.c` 21/25/78 (Explosions-SE 0x01110001 ueber re2fx_se_hook; kein
  Eigenschaden), `test_r34_granaten.cmake` (SE-Erwartung 0x01110001 in gr.log/wf.log), `probe_r34_schaden.c` 31/34
  (HE-Spalte 1 statt 3; Brad HURT[9][1] = 0x80105BC0), `probe_r34_reaktion.c` 220/222 (Spalte 1, DEATH-Zelle
  0x80108BEC/0x80109610 = Port 5/6 statt 0x80108530 = 1; Brad Handler RE2ZH_STAGGER = 2). Die Negativ-Kontrolle 221
  (Spalte 0 von Hand -> Kriecher HP 10) bleibt unveraendert gueltig.

### 6.1 Suite (voller Lauf `local_build.sh all` im Baum, Endstand)
```
=== LOCAL-BUILD-OK (all) — Tests 480/480
```
(Lauf 1 davor: 478/480 — nur `unit_r34_schaden` / `unit_r34_reaktion` mit den Binaries VOR der Spalten-Anpassung
§6; nach dem Neubau einzeln und im Lauf 2 gruen. Keine Fenster-Haken geflattert.)

### 6.2 Commits (Zweig r35/granate auf 154a73c1)
41c906b8 Dossier Messung vorher + RE2-Belege · 731c3263 Umsetzung Modul + Haken + Sonde · 59117ee5 exe-Pins +
FORCE_EXPLOSION-Boden · c1a36aa8 Dossier §4-6 · 24642c7a r34-Sonden Spalte 1 · (final) fix(r35-granate).

## OFFEN
(Stand nach Nachbesserung 1 — die vollstaendige Liste steht in N1.5; 1 und 2 sind dort fortgeschrieben.)
1. **Wand-Maske 1 (Spielerklasse)** bleibt PORT-WAHL. FUN_8004fba0 ist jetzt vollstaendig gelesen (N1.1): RE2 testet eine
   eigene Geschoss-Klasse (Bit 0x2000 in Zelle+8); RE1.5-u0 kennt nur 01/02/04/fb/fd/ff — s. N1.5 Punkt 2.
2. **Explosionspunkt der Wandzuendung** — ERLEDIGT in Nachbesserung 1: P = Rueckzugspunkt (RE2 Op 47 liest die Lage ohne
   Versatz @0x80020cdc-fc), N1.3.
3. **Spieler-Eigenschaden entfaellt** (RE2-Zustellung) — Nutzer informieren (§Fuer den Nutzer); RE1.5 hatte ihn ueber den
   generischen Resolver (Wurf-GP §2.5), nicht als Granaten-Design.
4. Birkin 0x30/0x36 ausserhalb 5090: die RE1.5-KI-Reaktion bleibt der Mutations-Schutz (300 -> 150, Zustand 3 -> 1) aus
   Runde 34 (Bosse-GP OFFEN 1 "Zielmodell"); nicht Gegenstand dieser Spur, die Explosion trifft ihn und haengt nicht.

## Fuer den Nutzer
* Keine neuen Sprachdateien, keine neuen Assets fuer das Paket-/Android-Gate (ARMS0F.EDH/.VB liegen schon unter
  shared_assets/PSX/SOUND).
* Verhalten: die Handgranate (auch Saeure/Brand) explodiert jetzt an der ersten Wand (ohne Abprall, wie die RE2-Granate),
  sonst nach wie vor 36 Bilder nach dem Liegen; Reichweite der Explosion ein Quadrat von +-2000 um die Liegestelle
  (vorher 900 Radius); Zombies im Umkreis werden zerrissen/weggeschleudert (RE2-Explosivrunde); Explosionston = RE2
  Granatwerfer (ARMS0F). Leon selbst wird von der Explosion nicht mehr verletzt (RE2-Regel).
* Bedienung unveraendert: Granate ueber Item-Debug (SELECT + R1 im Statusschirm) oder Fund; Zielhoehe hoch/mitte/tief
  bestimmt die Wurfweite (tief ~1500, mitte ~12000 bis zur naechsten Wand).

---

# Nachbesserung 1 (nach `A_abnahme_0.md`, 2026-10-03)

Ausgang: HEAD 6ffc1d41, Abnahme 0 = NICHT BESTANDEN (Punkt 1 teilweise). Die Messungen "vorher" je Mangel stehen in
`A_abnahme_0.md` (Laeufe `abn0_mess/t1220h1`, `t1220h2`, `t11c0c`, `tisch`, `boss_5090d`); sie werden hier als Pins
nachgestellt (N1.4). Werkzeuge neu: `A_werkzeug/sca_zensus.py` (Zellfelder aller Raeume), `A_werkzeug/jal_scan.py`
(alle `jal <ziel>` einer EXE mit den Argument-Ladungen davor).

## N1.1 RE-Belege (neu, selbst disassembliert)

### RE2 FUN_8004fba0 vollstaendig (`re2_disasm.py dis 0x8004fba0 330`, Ende `jr ra` @0x80050108) — korrigiert §3.1/§4.1
Signatur am Aufruf der Geschoss-Routine @0x8001ee60-a0: a0 = `sp+16` (Lage x/y/z s32), **a1 = 2 = Radius**
(`addiu a1,zero,2` @0x8001ee68), **a2 = 0x2000 = Klassenmaske** (`addiu a2,zero,8192` @0x8001ee7c), a3 = 0
(`addu a3,zero,zero` @0x8001ee84 = Objektkaesten mitpruefen).

| Schritt | Adresse | Instruktionen |
|---|---|---|
| Flag loeschen | @0x8004fc30-44 | `sw zero,-13368(at)` 0x800dcbc8, `sh zero,15228(at)` 0x800c3b7c (Bodenhoehe), `sw zero,-13268(at)` 0x800dcc2c |
| **y > 0 -> Flag** | @0x8004fc2c / @0x8004fc48-58 | `lw v1,4(s3)` / `blez v1,0x8004fc5c` / `addiu v0,zero,1` / `sw v0,-13368(at)` — unter der Ebene 0 gilt als Kontakt |
| **Objektkaesten** (a3 == 0) | @0x8004fc5c-0x8004fd6c | Liste 0x800d0324, Schritt 504, bis `*0x800d4224`; `jal 0x80038950(Lage, Obj, r, 0)` @0x8004fcac; Hoehenfenster des Kastens `lw a0,0(s1)` / `lhu a2,22(s1)` @0x8004fcbc-e4; im Fenster `ori v0,v0,0x1` / `sw v0,-13368(at)` @0x8004fd0c-18 |
| Zellschleife | @0x8004fd74-0x800500cc | 16 B je Zelle, Ende s7 = Kopf + Zahl*16 (`lw v0,4(s2)` / `sll v0,v0,4` @0x8004fc00-10) |
| Quadrant | @0x8004fd74-80 | `lhu v0,10(s2)` / `and v0,v0,fp` (fp = Rueckgabe `jal 0x8004c198` @0x8004fc0c) |
| Rechteck-Vortest | @0x8004fd88-b8 | `(x + r - Zelle.x) <u (w + 2r)`, dasselbe in z |
| **Klassenmaske** | @0x8004fdc0-d0 | `lhu v1,8(s2)` / `lw t0,16(sp)` / `and v0,v1,t0` / `beq v0,zero` (naechste Zelle) |
| Form 10 uebersprungen | @0x8004fdd4-dc | `andi v1,v1,0xf` / `addiu v0,zero,10` / `beq v1,v0` |
| Unterkante s1 | @0x8004fde4-0x8004fe04 | `lw a0,12(s2)`; Schleife `sra a0,a0,1` / `addiu s1,s1,-1800` bis Bit 0 = -1800 * (niedrigstes gesetztes Bit) |
| Etagenhoehe s0 | @0x8004fe08-30 | `lhu v1,10(s2)` / `srl v1,v1,6` / `andi v1,v1,0x1f` ... `subu s0,zero,v0` = -1800 * ((Zelle+10 >> 6) & 31) |
| **Formtest** | @0x8004fe34-54 | `andi v1,v1,0xf` / `sltiu v0,v1,0xe` / `lw v0,4356(at)` (Tabelle **0x80011104**) / `jr v0` |
| Tabelle 0x80011104 | 14 Worte | [0] 0x8004ffb0 (ohne Formtest = Rechteck), [1] 0x8004fe5c `jal 0x8004cfc8`, [2] 0x8004fe7c `jal 0x8004d484`, [3] 0x8004fe9c `jal 0x8004d940`, [4] 0x8004febc `jal 0x8004dde8`, [5] 0x8004fedc `jal 0x8004ea14`, [6] 0x8004fefc `jal 0x8004ed84`, [7] 0x8004ff1c `jal 0x8004ef0c`, [8] 0x8004ff3c `jal 0x8004f17c`, [9] 0x8004ffb0, [10] 0x800500c8 (naechste Zelle), [11] 0x8004ff5c `jal 0x8004f8b8`, [12] 0x8004ff78 `jal 0x8004fa28`, [13] 0x8004ff94 `jal 0x8004fb38`; je `bne v0,zero,0x8004ffb0` (Treffer), sonst `j 0x800500cc` |
| Oberkante | @0x8004ffb0-d0 | `lhu v1,10(s2)` / `srl v1,v1,11` / v1*100 (`sll 1`, `addu`, `sll 3`, `addu`, `sll 2`) / `subu s0,s0,v0` — s0 = Etagenhoehe - 100 * (Zelle+10 >> 11) |
| **Hoehenfenster** | @0x8004ffd4-0x8005000c | `lw v1,4(s3)` / `slt v0,s1,v1` / `bne v0,zero,0x8005005c` (unter der Unterkante) / `slt v0,s0,v1` / `beq v0,zero,0x80050028` (auf/ueber der Oberkante) / `ori v0,v0,0x1` / `sw v0,-13368(at)` — Flag Bit 0 nur fuer **s0 < y <= s1** |
| ueber der Oberkante | @0x80050028-58 | Oberkante wird Bodenkandidat (`sh s0,15228(at)`), `slt v0,v0,s0` / `bne` -> naechste Zelle OHNE Flag |
| Bit 1 | @0x8005005c-8c | y < s1: `ori v0,v0,0x2` / `sw v0,-13368(at)` |

RE2-Aufrufer von FUN_8004fba0 (`jal_scan.py info/re2leon/PSX.EXE 0x80010000 0x800 0x8004fba0`): alle Geschoss-Routinen
(@0x8001e060, @0x8001eea0, @0x8001f15c, @0x8001f564, @0x8001f810, ...) mit (r 2, Maske 0x2000); @0x8003692c mit (r 450,
Maske 0x4000: `addiu a1,zero,450` / `addiu a2,zero,16384`); @0x800376d8 mit (r aus 0x800cfc92, Maske 0x8000:
`ori a2,zero,0x8000`). RE2 fuehrt also eine EIGENE Zellklasse fuer Geschosse (Bit 0x2000 im Wort Zelle+8) neben
0x4000/0x8000.

### RE2 FUN_8001ED9C: zwei Ausgaenge in die Op-Tabelle — korrigiert §3.1/§3.2
* Gegner-Kontakt (`bne s0,zero,0x8001ef38` @0x8001ef14) und Lebensdauer-Ende (`lbu v0,11(t2)` / `bne v0,zero,0x8001ef80`
  @0x8001ef28-30, sonst Durchfall nach 0x8001ef38): Status |= 0x80 (`ori v0,v0,0x80` / `sh v0,24(v1)` @0x8001ef4c-50),
  dann **`lbu v0,2(v1)` @0x8001ef74 / `j 0x8001f0e4`** = Op aus step[**2**] + Art.
* Wand (`lw v0,-13368(v0)` / `beq v0,zero,0x8001f10c` @0x8001ef84-8c): Rueckzug, `jal 0x8001d894` @0x8001f0cc, dann
  **`lbu v0,3(v1)` @0x8001f0e0** = Op aus step[**3**] + Art (`lb v1,27(v1)` @0x8001f0e4, Tabelle 0x8009d868, `jalr v0`
  @0x8001f104). §3.2 schrieb fuer den Kontakt "Op 47"; belegt ist nur: Wand -> step[3] (= 47 bei der Explosivrunde).
  Ohne Wirkung auf den Port (Kontakt nicht verdrahtet).

### RE1.5-Zelle: Formen ja, Hoehe nein
* Form-Verteiler RE1.5: FUN_8003b0a4 ruft `(*(code**)(0x800b2858 + typ*4))(Zelle, ...)` (`lw v0,0x0(at)` @0x8003b4a0);
  FUN_8003aea0 fuellt die Tabelle: [1] FUN_8003bca8 Rechteck (`sw` @0x8003af04), [2] LAB_8003d00c Raute (@0x8003af14),
  [3] FUN_8003d6a8 Kreis (@0x8003af24), [4] LAB_8003beb0 (@0x8003af34), [5] LAB_8003c734 (@0x8003af44), [6] LAB_8003cb9c
  (@0x8003af54), [7] LAB_8003c2cc (@0x8003af64), [8] LAB_8003d7e8 (@0x8003af74), [9] LAB_8003d930 (@0x8003af84).
  Die Port-Zwillinge (re15_collision.c push_rect / push_diag2 / push_circle / push_diag4..7 / push_caps8/9) tragen die
  Flaechen; fuer r = 0 ergeben ihre Eintrittsbedingungen:

  | Typ | solide Flaeche (Zelle x, z, w, d) | Bedingung im Handler |
  |---|---|---|
  | 1 | Rechteck | Vortest |
  | 2 | Raute, Mitte (x + w/2, z + d/2), Halbachsen w/2 und d/2 | je Quadrant `pcmp < edge` (LAB_8003d00c) = abs(dx)/hw + abs(dz)/hd < 1 |
  | 3 | Kreis, Mitte (x + w/2, z + w/2), Radius w/2 | `pen = cr - dist >= 1` (FUN_8003d6a8) |
  | 4 | Dreieck, rechter Winkel bei (x+w, z+d) | `Q < s8`: pz - (z+d) > -d*(px-x)/w (LAB_8003beb0) |
  | 5 | Dreieck, rechter Winkel bei (x, z+d) | `LINE < ZTERM`: pz - z > d*(px-x)/w (LAB_8003c734) |
  | 6 | Dreieck, rechter Winkel bei (x+w, z) | `s3 < s7q`: pz - z < d*(px-x)/w (LAB_8003cb9c) |
  | 7 | Dreieck, rechter Winkel bei (x, z) | `s6 < s3`: pz - (z+d) < -d*(px-x)/w (LAB_8003c2cc) |
  | 8 | Kapsel in x: Rechteck x+d/2 .. x+w-d/2, Kreise (Durchmesser d) an beiden Enden | LAB_8003d7e8 |
  | 9 | Kapsel in z: Rechteck z+w/2 .. z+d-w/2, Kreise (Durchmesser w) an beiden Enden | LAB_8003d930 |

* Quadranten: die Zellen stehen in 5 Listen (Kopf +4..+20); der Aufloeser nimmt die Liste des Quadranten der Lage
  (`FUN_8003b068(pos, ..., hdr[0], hdr[1])` in FUN_8003b0a4). In ROOM1140 sind alle Listen gleich, in ROOM11C0 nicht —
  eine Zelle gilt nur in IHREM Quadranten.
* **Hoehe:** die RE1.5-Zelle hat 12 B (w, d, x, z, typ u8, u0 u8, u1 u8, floor u8). Wort +10 = u1 | floor<<8:
  Bits 12-15 Band (`(u8)(ent+0x82) == (w5<<16)>>28` in FUN_8003b0a4), Bits 8-11 ein Klassen-Nibble, Bits 0-7 Merker.
  Zensus (`sca_zensus.py`: 4706 eindeutige Zellen, 4666 mit u0&1): Nibble 3 = 3996, 0/1/2 = 84/130/144, 4..d = 312; alle
  Formtypen kommen in Nibble 3 vor. Leser des Nibbles in der EXE (Suche ueber RE_15_Quellcode_V2, 1258 Funktionen): nur
  der Sichtstrahl FUN_8003dcc4(vec, Region, Maske, Wert) mit `(Maske & w5) == Wert` — FUN_8001b9b4 ruft ihn mit
  (0xf00, 0x300) (Schuetze -> Ziel; Aufrufer FUN_80011f50 = Zielwahl der Waffe), FUN_8001bafc mit (0x400, 0x400).
  KEIN Leser nimmt das Nibble als Zahl (keine Multiplikation wie RE2s `* 100` @0x8004ffb0-d0); in Treppenraeumen ist es
  = Band oder Band+1 (ROOM4070: 0x45/0x67/0x89/0xab/0xcd; ROOM2080: 0x45/0x55/0x78/0x88), also keine Hoehe ueber Grund.
  **ROOM1140: alle 19 Zellen je Liste tragen u0=ff u1=00 floor=03 — der Konferenztisch (#6 x[-4650..7450]
  z[-17450..-11350]) ist in den RDT-Daten von einer Wand nicht zu unterscheiden** (und sperrt im Original auch die
  Schusslinie der Zielwahl: Nibble 3). Eine Hoehenregel wie RE2 @0x8004ffd4-0x8005000c laesst sich aus den RE1.5-Daten
  nicht belegen -> OFFEN (N1.5), Nutzerhinweis.

## N1.2 Die Maengel der Abnahme 0, je Mangel: Ursache / Messung vorher / Beleg / Aenderung / Messung nachher

Messlaeufe nachher (unversioniert): `re15_port/build/r35a_mess/nb1/<lauf>/` (gr.log, state.log, wf.log, debug.log),
Startskript `nb1/lauf.sh <name> <timeout> VAR=...` (Kopie der exe als `re15_pc_nb1_<name>.exe`, Umgebung wie
`tests/integration/test_r35_granate.cmake`).

### Mangel 1 — Durchflug durch duenne Wandzellen: BEHOBEN
* **Ursache:** EIN Punkttest je Bild (alter `re15_granate_r35_wand`) bei Schritt 380 (HOCH, `ori 0x17c` @0x80018494) bzw.
  280 (MITTE, `ori 0x118` @0x800184bc). Zellen duenner als der Schritt fallen je nach Wurfphase zwischen zwei Bildpunkte.
* **Vorher (Abnahme 0, Lauf t1220h2):** ROOM1220, Leon (-19433,-9700), HOCH: `wpos.x -21503 -> -21873` ueberspringt
  `typ1 x[-21825..-21550] z[-19875..-4300]`, Explosion erst bei x -25811 in der Gefaengniszelle.
* **Beleg:** RE2 tastet ebenfalls je Bild (`jal 0x8004fba0` @0x8001eea0, Radius 2) — der Mechanismus selbst kennt den
  Durchflug. Die Strecke ist deshalb **PORT-WAHL zur NUTZER-VORGABE** ("Granaten sollen nicht durch die Wand fliegen");
  belegt ist ihre Form: Anfang = vorige Weltlage aus der Physik des Platzes `xlat += vel; vel += acc` (@0x8001a324-388)
  -> `xlat_alt = xlat - (vel - acc)`, Weltlage wie im Tick (@0x8001a118-2a4); Rueckzug weiter nach RE2
  (@0x8001ef90-0x8001f0c8 = 1 + 1/3 Schritt zurueck -> liegt auf der freien Vorgaenger-Strecke).
* **Aenderung:** `re15_granate_r35_wand` testet die STRECKE vorige -> neue Weltlage (`re15_granate_r35_strecke`, ganzzahlig,
  s64-Kreuzprodukte; an den Quadrantengrenzen geteilt). Im WURFBILD (xlat == 0, es gibt noch keine vorige Lage) die Strecke
  Werfer -> Hand; fuehrt der RE2-Rueckzug dort nicht auf die freie Seite, zuendet die Granate ueber dem Standpunkt des
  Werfers (PORT-WAHL, ohne Konstante). Steht der Werfer im getesteten Band selbst in einer Zelle (das Band der Granate ist
  -(Standhoehe/0x708), das Band des Spieler-Aufloesers der gefuehrte Zustand +0x82), gibt es keine freie Strecke — dann
  gilt im Wurfbild nur die Hand als Punkt (Sonde 235), damit eine Band-Abweichung nicht jeden Wurf sofort zuendet.
* **Nachher (exe, gleiche Aufstellungen):**
  ```
  d_a (Leon x -19533): T=289 EV wand wpos=(-21603,-3621,-10677) -> rueckzug (-21107,-3525,-10649) -> explosion sofort von=(-21231,-10656)
  d_b (Leon x -19433): T=290 EV wand wpos=(-21873,-3681,-10698) -> rueckzug (-21379,-3598,-10670) -> explosion sofort von=(-21503,-10677)
                       T=290 EV resolver art=2 P=(-21379,-3598,-10670) r=2000        (vorher: kein EV wand, Explosion x -25811)
  ```
  Sonde 230-233: Punkt vor/hinter der Zelle frei, Strecke blockiert; **38 von 38 Wurfphasen** (Wurfstelle je 10 versetzt)
  halten vor der Zellenfront (Explosion x in [-21424..-21059]); in **11 von 38** Phasen liegt kein Bildpunkt in der Zelle
  (= der Punkttest haette sie verfehlt; die Abnahme schaetzte 28 %); Wurfbild: Hand hinter der duennen Zelle -> Explosion
  im Wurfbild ueber dem Werfer (x -21082 = Werfer). exe-Pins `duenn_a` / `duenn_b`.

### Mangel 2 — Sofort-Explosion in freiem Gelaende (Rechteck statt Zellform): BEHOBEN
* **Ursache:** `re15_collision_box_blocked` prueft fuer jeden Zelltyp nur das Begrenzungsrechteck; 642 von 4666 Zellen
  (13,8 %) sind Raute/Kreis/Dreieck/Kapsel.
* **Vorher (Abnahme 0, Lauf t11c0c):** ROOM11C0, `F=1115 SPAWN anker=(-6720,-2474,-13266)`, im selben Tick `EV wand ...
  explosion sofort` — der Punkt liegt nur im Rechteck der Raute `typ2 x[-15400..4799] z[-13266..4921]`.
* **Beleg:** RE2 Formtest `jr` ueber Tabelle 0x80011104 @0x8004fe34-54 (N1.1); RE1.5-Formen = Handler 0x800b2858[1..9]
  (FUN_8003aea0 @0x8003af04-84), Flaechentabelle N1.1; Quadrantenliste FUN_8003b068.
* **Aenderung:** `strecke_zelle` (granate_r35.c) testet die Flaeche je Typ: Rechteck, Raute, Kreis, vier Dreiecke, zwei
  Kapseln — gegen die Liste des Quadranten, in dem das Streckenstueck liegt. `re15_collision_box_blocked` wird von der
  Granate nicht mehr benutzt (unveraendert fuer seinen Aufrufer, die Objekt-Schiebepruefung).
* **Nachher (exe, Lauf raute, gleiche Aufstellung):**
  ```
  F=1115 SPAWN granate art=2 slot=0 anker=(-6720,-2474,-13266) gier=467            (derselbe Wurf)
  T=1386 EV wand wpos=(-1280,-724,-17215) -> rueckzug (-1558,-974,-17015) -> explosion sofort von=(-1489,-17065)
  T=1386 EV resolver art=2 P=(-1558,-974,-17015) r=2000 eingriffe=1
  ```
  = 25 Flugbilder (T=1361..1386) statt 0; die Wand ist die echte Zelle `typ1 x[-23796..16603] z[-27296..-17095]`.
  Sonde 200-211 (je Typ 1-9: Punkt in der Flaeche blockiert, Punkt im Rechteck ausserhalb der Flaeche frei; GEGENPROBE am
  Port-Zwilling des RE1.5-Handlers: der Aufloeser FUN_8003b0a4 mit Radius 0 schiebt innen und schiebt aussen nicht —
  alle acht geprueften Typen stimmen), 220-222 (ROOM11C0: Rechteck-Test 1 = Vorher-Befund, Form 0; Rautenmitte 1; Wurf
  aus der Hand: 6 von 6 Bildern Flug). exe-Pin `raute`.

### Mangel 3 — Zellhoehe / niedrige Hindernisse: ALS ABWEICHUNG AUSGESCHRIEBEN (keine Hoehe in den RE1.5-Daten)
* **Befund bleibt (exe, Lauf tisch, unveraendert):** ROOM1140, Leon (0,-10300) Blick 1024, MITTE:
  `T=288 EV wand wpos=(-349,-2524,-11519) -> rueckzug (-389,-2454,-11147) -> explosion sofort` an der Tischzelle
  `typ1 x[-4650..7450] z[-17450..-11350]`, ein Bild nach dem Wurf, 2454 ueber dem Boden.
* **Beleg, warum keine Hoehenregel gebaut wurde:** RE2 nimmt die Hoehe aus der Zelle (Oberkante = Etage - 100 *
  (Zelle+10 >> 11) @0x8004ffb0-d0, Fenster @0x8004ffd4-0x8005000c). Die RE1.5-Zelle hat dieses Feld nicht (N1.1 "Hoehe":
  Zensus aller 4706 Zellen; einziger Leser des Klassen-Nibbles ist der Sichtstrahl FUN_8003dcc4 mit Maske/Wert
  (0xf00, 0x300) bzw. (0x400, 0x400), keine Zahl). In ROOM1140 traegt der Tisch dieselben Felder wie die Waende
  (u0=ff u1=00 floor=03). Eine Hoehe je Zelle waere eine Zahl nach Gefuehl -> nicht gebaut.
* **Stand:** die Granate haelt an JEDER Zelle der Maske u0&1 in jeder Flughoehe — gekennzeichnete ABWEICHUNG von RE2
  (Kommentar granate_r35.c "Hoehe", OFFEN N1.5 Punkt 1, Nutzerhinweis N1.6).

### Mangel 4 — Dossier §3.1/§4.1 ungenau: KORRIGIERT
§3.1 (Tabellenzeile Boden/Wand-Test), §3.2 (Sprung ueber step[2]), §4.1 (Zeilen `_wand`, `_flug`), §4.3 (re15_esp.c) sind
berichtigt; die vollstaendige Lesung von FUN_8004fba0 steht in N1.1.

### Mangel 5 — Birkin-exe-Pin misst den geparkten Boss: BEHOBEN
* **Vorher:** Slot 2 grid 0x33 (unarmiert, vom Modul nicht getickt), st 0 -> 2, danach 39 Bilder unveraendert.
* **Messung (Lauf birkin_c: RE15_DEBUG_JUMP=5090@250 RE15_DEBUG_SUB=4@255 RE15_FORCE_EXPLOSION=2@400:2):**
  ```
  debug_sub.log: [debug-sub] Frame 255: sub04 subs=5 ... Slot 2 rc=0        (Kampfstart; Vorspann pst=4 bis ~Bild 346)
  F340 [2 t=36 st=0 g=13 mo=1 af=0  @(-9000,-23400)] hp=600                 (armiert, grid 0x13)
  F380 [2 t=36 st=0 g=13 mo=1 af=33 @(-8808,-23400)] hp=600                 (kriecht)
  F400 [2 t=36 st=2 ss1=9 g=13 mo=1 af=53 @(-7373,-23400)] hp=520           (Treffer: 80 = RE2 Zeile 9 K0 @0x800A5F7C)
  F420 ... af=73 @(-4471,-23400)   F440 ... af=93 @(-3952,-23400)   F500 ... af=153 @(-3882,-23400)   hp=520
  ```
  Der armierte Boss wird getroffen und laeuft weiter (Clip-Bild und Lage aendern sich), kein Haenger.
* **Aenderung:** exe-Pin `birkin` armiert ueber `RE15_DEBUG_SUB=4@255`, Explosion im Bild 400, prueft (a) g=13 im Bild 399,
  (b) HP 600 -> 520 im Bild 400 und noch im Bild 440 (genau ein Treffer), (c) Reaktion: Weg > 200 und af anders 40 Bilder
  spaeter. Ergebnis: `[birkin] ok — 36 Slot 2 armiert (g=13): HP 600 -> 520, danach Weg 3421, af 53 -> 93`.

### Mangel 6 — Tautologie PRUEF 154: BEHOBEN
Die Sonde haelt jetzt die Weltlage im Explosionsbild fest und prueft SE-Lage = (x, y - 500, z) der Liegestelle
(`addiu v0,v0,-500` @0x800185a8) und Liegestelle auf der Ebene: `ok 154: SE-Lage (5078,-491,-16527) = Liegestelle
(5078,9,-16527) mit y - 500`.

### Mangel 7 — Vertrag 1.4 (re15_esp.c +37/-8): BEHOBEN
`git diff master --stat -- re15_port/engine/src/re15_esp.c` = **+10/-7**: include (1), Haken Routine 29
`if (re15_granate_r35_flug(f)) return;` (1), Routine 31 Zustellung (4 Zeilen -> 1, Logtext r=2000) und SE (2 Zeilen
geaendert), drei Dienste fuer das Modul (4). `esp_granate_sofort` und der Flugtest-Block stehen in granate_r35.c
(`re15_granate_r35_flug`).

### Mangel 8 — audio_pc.c-Kommentar: BEHOBEN
`if (!frei) return NULL;   /* ARMS_ZUSATZ_N Plaetze: ARMS10/11 (Aufschlag) + ARMS0F (Explosion) */`

### Mangel 9 — nicht beauftragte Verhaltensaenderungen: dem Nutzer vorgelegt (N1.6)

### Zusaetzlich (aus dem Hinweis der Abnahme zu Punkt 2 und dem alten OFFEN 2): Explosionspunkt der Wandzuendung
* **Vorher (exe, Lauf d_a vor der Aenderung):** `EV wand ... rueckzug (-21107,-3525,-10649)`, `EV resolver P=(-21107,-4025,
  -10649) eingriffe=0` — Routine 31 zieht 500 ab (`addiu v0,v0,-500` @0x800185a8, gedacht fuer die LIEGENDE Granate);
  damit lag P bei jeder HOCH-Wandzuendung (Flughoehe 3551..3831) ueber beiden Pruefhoehen stehender Gegner.
* **Beleg:** RE2 Op 47 liest die Lage des Geschosses ohne Versatz: `lh v0,52(v1)` / `sw v0,16(sp)`, `lh v0,54(v1)` /
  `sw v0,20(sp)`, `lh v0,56(v1)` / `sw v0,24(sp)` @0x80020cdc-fc; `addiu a0,sp,16` @0x80020d50 / `jal 0x800470c0`
  @0x80020d78; zweite Hoehe `addiu v0,v0,900` @0x80020d98.
* **Aenderung:** der Flug-Haken senkt den Platz vor der Sofort-Zuendung um `RE15_GRANATE_R35_R31_VERSATZ` 500 (= der
  Versatz der Routine 31 @0x800185a8), damit P = Rueckzugspunkt. Die liegende Granate bleibt RE1.5 (P = Liegestelle - 500).
* **Nachher:** exe d_a `EV resolver art=2 P=(-21107,-3525,-10649)`; Sonde 113 (P.y -1634 = Flugbahn, Abstand 6 zur Lage
  im Vorbild) und 234 (HOCH-Wandzuendung trifft den stehenden Zombie 700 vor der Wand ueber die zweite Pruefhoehe).

## N1.3 Umsetzung (Dateien, Konstanten)
| Datei | Aenderung | Beleg |
|---|---|---|
| `engine/src/granate_r35.c` | `strecke_vieleck` / `strecke_kreis` / `strecke_zelle` (Typ 1-9), `quadrant`, `strecke_liste`, `re15_granate_r35_strecke` / `_punkt`, `strecken_anfang`, `re15_granate_r35_wand` (Strecke), `re15_granate_r35_flug` (Haken: Wand -> Rueckzug -> Weltlage -> Versatz 500 -> Liegezustand + Zuender 7 -> Routine A) | Formen: RE1.5 0x800b2858[1..9] @0x8003af04-84; Quadrant FUN_8003b068; RE2 Form/Quadrant/Klasse FUN_8004fba0 (N1.1); Rueckzug RE2 @0x8001ef90-0x8001f0c8; Sofort-Zuendung RE2 step[3] @0x8001f0e0-104 auf RE1.5 @0x80018368-84 / @0x8001856c; P ohne Versatz RE2 @0x80020cdc-fc |
| `include/re15_granate_r35.h` | neue Schnittstelle (`_flug`, `_strecke`, `_punkt`), `RE15_GRANATE_R35_R31_VERSATZ 500` (@0x800185a8), Kommentar der Wand-Maske (RE2-Geschossklasse 0x2000), Dienste aus re15_esp.c | s.o. |
| `engine/src/re15_esp.c` | Haken auf 1 Zeile, drei Dienst-Einzeiler (N1.2 Mangel 7) | — |
| `platform/pc/src/audio_pc.c` | Kommentar (Mangel 8) | — |
| `tests/unit/test_r35_granate.c` | Abschnitt [6] strecke (200-211, 220-222, 230-234), 113 (P = Rueckzugspunkt), 154 ohne Tautologie, Werfer an der Hand in [1] | — |
| `tests/integration/test_r35_granate.cmake` | Laeufe `duenn_a`, `duenn_b`, `raute`; `birkin` armiert mit Reaktion; `wand` prueft P.y = Rueckzug-y; `SDL_ASSERT=always_ignore` + Wiederholung bei SDL-Assertion | — |
| `analysis/befunde_runde35/A_werkzeug/` | `sca_zensus.py`, `jal_scan.py`, `zustand_spur.py` | — |
Keine neuen Assets, keine Aenderung an shared_assets, keine neuen Bits/IDs/Ereignisse.
Einzige neue Konstante im Verhalten: 500 (`RE15_GRANATE_R35_R31_VERSATZ`, RE1.5 @0x800185a8, gegen RE2 @0x80020cdc-fc).
PORT-WAHLEN, gekennzeichnet im Code: Strecke statt Punkt (Nutzervorgabe), Wurfbild-Regel, Wand-Maske u0&1, keine Zellhoehe.

## N1.4 Messung nachher (Zusammenfassung; Einzelheiten N1.2)
| Lauf | Aufstellung | vorher (Abnahme 0) | nachher |
|---|---|---|---|
| d_a | ROOM1220 Leon x -19533, HOCH | Wand -21603 -> Rueckzug -21107 | gleich; P.y -3525 statt -4025 |
| d_b | ROOM1220 Leon x -19433, HOCH | KEINE Wand, Explosion x -25811 in der Zelle | Wand an der Strecke -21503 -> -21873, Rueckzug -21379 |
| raute | ROOM11C0 nach der Ada-Szene, MITTE | Explosion im Wurfbild (0 Flugbilder) | 25 Flugbilder, Wand an der echten Zelle z -17095 |
| tisch | ROOM1140 ueber den Konferenztisch, MITTE | Wand an der Tischzelle 1 Bild nach dem Wurf | unveraendert (Abweichung, OFFEN 1) |
| birkin_c | ROOM5090 sub04 armiert, Explosion Bild 400 | Pin traf den geparkten Boss | HP 600 -> 520, Boss kriecht weiter (-7373 -> -3952 in 40 Bildern) |
| wand / zombie / gator | wie §5 | gruen | gruen, unveraendert (Wand 8385 -> 8332; 3 Eingriffe, 2 zerrissen; 3000 -> 2000) |
Bilder: in dieser Nachbesserung wurden keine neuen Bilder gebraucht (alle Urteile tragen Logwerte); das Fenster-Capture
(gdigrab) liefert in dieser Sitzung weisse Bilder, fuer Bilder gilt RE15_FRAMEDUMP (Abnahme 0: `abn0_mess/*/kontakt_*.png`).

## N1.5 OFFEN (Stand nach Nachbesserung 1)
1. **Zellhoehe (Abweichung von RE2 @0x8004ffd4-0x8005000c):** die Granate haelt an jeder u0&1-Zelle in jeder Hoehe, auch an
   niedrigen Hindernissen (ROOM1140 Konferenztisch, Lauf tisch). RE1.5-Zellen tragen keine Hoehe (N1.1). Naechster Messweg:
   (a) DuckStation ROOM1140: verliert die Zielwahl (FUN_80011f50 -> FUN_8001b9b4, Maske 0xf00 Wert 0x300) ein Ziel
   jenseits des Tisches? Dann behandelt schon das Original den Tisch als volle Sperre; (b) Bedeutung des Klassen-Nibbles
   0/1/2/4..d an einem Treppenraum (ROOM4070/2080: Savestate, Gegner-Band +0x82 gegen die Zellen); (c) will der Nutzer
   ueber Tische werfen, braucht es eine NUTZER-VORGABE (Raum, Zelle, Hoehe) — die Form (RE2-Fenster Oberkante < y) ist belegt.
2. **Wand-Maske u0&1 = PORT-WAHL.** RE2 fuehrt eine eigene Geschossklasse (0x2000, N1.1); RE1.5-u0 kommt nur als
   01 (72 Zellen), 02 (2), 04 (4), fb (8), fd (14), ff (4572) vor. Mit Maske 1 haelt die Granate auch an den 72 reinen
   Spielersperren (u0=01). Naechster Messweg: diese 72 Zellen je Raum gegen das Hintergrundbild legen (Sperre mit oder ohne
   sichtbare Wand).
3. **Wurfbild-Regel = PORT-WAHL:** liegt zwischen Werfer und Hand eine Zelle, zuendet die Granate im Wurfbild (ueber dem
   Werfer, wenn der RE2-Rueckzug nicht auf die freie Seite fuehrt). RE2 hat dafuer kein Gegenstueck (die Runde startet an
   der Muendung und wird erst nach dem ersten Schritt getastet).
4. **Strecke an der Quadrantengrenze:** der Teilungspunkt ist auf 1 Einheit gerundet (ganzzahlig, Q16-Parameter).
5. **Niedrige Gegner bei hoher Wandzuendung:** P = Rueckzugspunkt; eine HOCH-Wandzuendung (y ~ -3500) trifft stehende
   Gegner ueber die zweite Pruefhoehe P + 900 (Sonde 234), liegende/niedrige (Hund, Kriecher) liegen unter dem Band
   (@0x8004716c-a4) — an der exe gemessen `eingriffe=0` gegen die beiden LIEGENDEN Zombies in ROOM1220 (d_a/d_b).
6. **Flugrichtung der zerrissenen Zombies** (Abnahme 0: "in Wurfrichtung, auch durch den Explosionspunkt"): nicht
   untersucht. Naechster Messweg: RE2 0x80109610 (Wegschleudern) lesen — welche Richtung nimmt der Handler (Gier des
   Treffers aus FUN_800470C0 a1 = `lh a1,34(v0)` @0x80020d6c, oder Lage zum Treffpunkt)?
7. Unveraendert aus dem ersten Stand: Spieler-Eigenschaden entfaellt (RE2-Zustellung); Birkin 0x30/0x36 ausserhalb ROOM5090
   reagiert mit dem RE1.5-Mutations-Schutz; RE2-Flugkontakt nicht verdrahtet (§4.2).

## N1.6 Fuer den Nutzer (ersetzt den Abschnitt "Fuer den Nutzer" oben, soweit er abweicht)
* Keine neuen Sprachdateien, keine neuen Assets fuer das Paket-/Android-Gate.
* **Wand:** die Granate explodiert an der ersten Wand ihrer Flugbahn — auch an duennen Waenden (Zellentrakt ROOM1220) und
  nicht mehr faelschlich im Freien neben schraegen/runden Hindernissen (Parkplatz ROOM11C0). Wer direkt an einer Wand
  steht und wirft (Hand in oder hinter der Wand), bekommt die Explosion sofort ueber Leon.
* **Bekannte Grenze:** die Raumdaten von RE1.5 kennen keine Hindernis-HOEHE. Ueber Tische und Theken laesst sich deshalb
  nicht werfen — die Granate zuendet an der Tischkante in Flughoehe (ROOM1140 Konferenztisch).
* **Zu entscheiden (Verhalten, das nicht beauftragt war):**
  1. Leon wird von der eigenen Granate nicht mehr verletzt (RE2-Regel; RE1.5: 1000 Schaden unter 950 Abstand).
  2. Zerrissene Zombies fliegen in Wurfrichtung, auch durch den Explosionspunkt hindurch (nicht vom Explosionspunkt weg).
  3. MITTE- und HOCH-Wuerfe ueberfliegen Gegner (kein Kontaktzuender) und enden an der ersten Wand; getroffen wird im
     Quadrat +-2000 um den Zuendpunkt. Treffen "im Nahbereich" geht mit TIEF.
  4. Soll die Granate ueber niedrige Hindernisse fliegen? Dann bitte die Hindernisse nennen (Raum + Stelle) — die Hoehe
     ist eine Nutzervorgabe.
* Bedienung unveraendert: Item-Debug SELECT + R1 im Statusschirm oder Fund; Zielhoehe hoch/mitte/tief bestimmt die Wurfweite.

## N1.7 Tests und Suite (Nachbesserung 1)
* `unit_r35_granate` (test_r35_granate alle): `test_r35_granate: ALLE PRUEFUNGEN GRUEN` — neu 113, 154 (echt), 200-211,
  220-222, 230-235. Auszug:
  ```
  ok   113: Explosionspunkt P=(8022,-1634) = Rueckzugspunkt: Flugbahn-y im Vorbild -1640 (Abstand 6 < 150; mit Routine-31-Versatz waeren es ~500)
  ok   201: Typ 2 Raute LAB_8003d00c: ... innen 1 (1), aussen 0 (0); Rechteck-Test aussen 1; RE1.5-Handler schiebt innen 1 (1) / aussen 0 (0)
  ok   220: ROOM11C0 Hand (-6720,-13266): Rechteck der Raute 1 (1 = Vorher-Befund), Form 0 (0 = frei)
  ok   231: ROOM1220 HOCH, 38 Phasen (Wurfstelle je 10 versetzt): 38 halten VOR der Zellenfront (x > -21550); Explosion x in [-21424..-21059]
  ok   232: davon 11 Phasen, in denen KEIN Bildpunkt in der 275 dicken Zelle liegt (der Punkttest je Bild verfehlte sie)
  ok   233: Wurfbild: Hand hinter der duennen Zelle (Punkt frei 1) -> Explosion im Wurfbild (Wand 1) ueber dem Werfer: x -21082 (-21082), z -10677 (-10677)
  ok   234: HOCH-Wandzuendung bei P.y -3525 (ueber der ersten Pruefhoehe -3100, unter -4000): stehender Zombie 700 vor der Wand getroffen, hp -100 (-100)
  ok   235: Werfer in der Zelle, Hand frei im Flur: kein Ausloeser im Wurfbild (Wand 0, Explosionen 0)
  ```
* `integration_r35_granate` (echte exe, 7 Laeufe): wand, zombie, gator, birkin (armiert), duenn_a, duenn_b, raute.
  Eigener Lauf: `[wand] ok — Wand x=8385 -> 8332`, `[zombie] ok — X=119, 2 Zombies zerrissen, 3 Eingriffe`,
  `[gator] ok — HP 3000 -> 2000`, `[birkin] ok — armiert (g=13): HP 600 -> 520, danach Weg 3421, af 53 -> 93`,
  `[duenn_a] ok — Wandpunkt x=-21603 -> Rueckzug -21107, kleinstes Granaten-x -21231`,
  `[duenn_b] ok — Wandpunkt x=-21873 -> Rueckzug -21379, kleinstes Granaten-x -21503`,
  `[raute] ok — Hand (-6720,-13266), 25 Flugbilder, Wandzeilen 1, Explosion Tick 1386`.
* **Umgebungsbefund (kein Spielbefund):** im ersten Gesamtlauf blieb `raute` bei Bild 779 stehen (Timeout 400 s);
  debug.log: `Assertion failure at WIN_AddDisplay (SDL_windowsmodes.c:380) 'index == *display_index'` — SDL oeffnet bei
  einer Aenderung der Display-Topologie (fremde Fenster paralleler Baeume) einen Assert-Dialog, der die exe anhaelt.
  Einzeln nachgefahren: gruen in 52,6 s. Der Pin setzt jetzt `SDL_ASSERT=always_ignore` (SDL-eigene Umgebungsvariable)
  und wiederholt einen Lauf mit dieser Assertion einmal. Fuer den Orchestrator: derselbe Dialog kann jeden anderen
  Fenster-Haken treffen (Symptom: Timeout, debug.log endet mit der Assertion).

### Suite (voller Lauf `bash re15_port/tools/local_build.sh all` im Baum, Endstand der Nachbesserung 1)
```
=== LOCAL-BUILD-OK (all) — Tests 480/480
```
Zwei volle Laeufe: Lauf 1 (Stand d72d020d, vor Sonde 234/235 und dem Werfer-in-Zelle-Schutz) `=== LOCAL-BUILD-OK (all) —
Tests 480/480`, darin `integration_r35_granate` mit allen sieben Laeufen; Lauf 2 (Endstand, Quellen = Abschluss-Commit)
die Zeile oben. Kein Fenster-Haken geflattert. Logs: `re15_port/build/r35a_mess/nb1/suite1.log` / `suite2.log`,
`re15_port/build/local_build_ctest.log`.

### Commits der Nachbesserung 1 (Zweig r35/granate, auf 6ffc1d41)
553f3174 RE-Belege (FUN_8004fba0, Formverteiler, Zensus) · 52ba0d6d Wandtest formgenau/Strecke/Quadrant, Haken nach
granate_r35.c, Sonde [6], PRUEF 154, audio-Kommentar · (wip) Handler-Gegenprobe · 9acc9c04 P = Rueckzugspunkt, Sonde 113 ·
d72d020d exe-Pins (Birkin armiert, duenn_a/b, raute), SDL_ASSERT · fd5b8845 Dossier N1.2-N1.7, Sonde 234 · a45a5a00
Werfer-in-Zelle-Schutz, Sonde 235 · (final) fix(r35-granate): Nachbesserung 1.
