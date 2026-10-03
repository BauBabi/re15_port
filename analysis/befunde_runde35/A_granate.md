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
| Boden/Wand-Test | @0x8001ee60-a0 | `addiu a0,sp,16` (Lage), `addiu a1,zero,2`, `addiu a2,zero,8192`, `addu a3,zero,zero`, `jal 0x8004fba0`; FUN_8004fba0 loescht `sw zero,-13368(at)` 0x800dcbc8 @0x8004fc34 (Wand-Flag) und setzt es bei Zellkontakt |
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
`bne s0,zero,0x8001ef38` @0x8001ef14 -> Status |= 0x80 @0x8001ef44-50 -> Explosion (Op 47) im selben Bild.
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
| `re15_granate_r35_wand(f)` | Granaten-Weltlage (slot+0x28/+0x2c) gegen die soliden SCA-Zellen des Werfer-Bandes (Band = -(granate_boden/0x708) wie FUN_8001c2dc), Punkttest r=0, Maske u0&1 | RE2 `jal 0x8004fba0(&Lage,2,8192,0)` @0x8001ee60-a0 setzt DAT_800dcbc8; `lw/beq` @0x8001ef84-8c. Maske 1 = PORT-WAHL (Zellklasse des Spielers, FUN_8003b0a4 a2=1) |
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
| `engine/src/re15_esp.c` | ~14 | include; Vorwaertsdeklaration `esp_fx_weltlage`; `esp_granate_sofort()` (Liegezustand @0x80018368-84 + Zuender 7 @0x8001856c + Routine A im selben Tick = RE2 `jr Op[47+Art]` @0x8001f0e0-104); Haken am Kopf von `esp_fx_dispatch_b_29` (Wand -> Rueckzug -> Weltlage neu -> sofort); `case 31` Zuender 7: Zustellung `re15_granate_r35_explosion` statt `re15_resolve_attack(r=500)`, SE `re15_granate_r35_explosion_se` statt `0x04080001`; gr.log-Zeilen `EV wand ... -> rueckzug ...`, `resolver ... r=2000`, `se code=01110001` |
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

## OFFEN
1. **Wand-Maske 1 (Spielerklasse)** ist PORT-WAHL: RE2 FUN_8004fba0 testet die RE2-SCA mit a1 = 2 / a2 = 8192, deren
   Klassenbits nicht 1:1 auf RE1.5-u0 abbilden (RE1.5: Spieler 1, Objekte 2, Gegner 4). Naechster Messweg: FUN_8004fba0
   @0x8004fba0-0x8004fd?? vollstaendig disassemblieren (Zellscan ab @0x8004fc3c) und die Klasse der RE2-Runde mit
   den RE1.5-Zellklassen eines Raums (gen_map_zones.py) vergleichen.
2. **Explosionspunkt der Wandzuendung** = RE1.5-Regel (Welt-y - 500 @0x800185a8) am Rueckzugspunkt; RE2 Op 47 nimmt die
   Lage selbst (+ y-470 bei Lebensdauer 255 @0x80020df0). Fuer eine Granate, die hoch an der Wand zuendet, liegt P 500
   darueber — sichtbar nur bei sehr hohen Treffpunkten; gemessen n1b: Treffpunkt y -12, P y -508 (Bodennaehe).
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
