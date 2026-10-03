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

(Fortsetzung: §4 Umsetzung, §5 Messung nachher, §6 Tests, OFFEN, Fuer den Nutzer — folgt)
