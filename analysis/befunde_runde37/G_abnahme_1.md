# Runde 37 — Spur G "beretta": UNABHAENGIGE ABNAHME 1 (2026-10-13)

Baum `.claude/worktrees/r37_beretta`, Zweig `r37/beretta`, Kopf 25e4004e (Basis = merge-base f1e019ca; master ist
inzwischen weiter, Spur-Diff daher ueber `git diff master...HEAD`). Geprueft wird die Nachbesserung 1 auf die Maengel
M1-M4 der Abnahme 0 (`G_abnahme_0.md`) und erneut der ganze Nutzer-Punkt.

Eigener Bau: `bash re15_port/tools/local_build.sh configure` + `build` -> `LOCAL-BUILD-OK (configure)`,
`ninja: no work to do`, `LOCAL-BUILD-OK (build)`. exe `re15_port/build/platform/pc/re15_pc.exe` 2026-10-13 21:55:27
(md5 1a2b95d657bccb2751aba752662c1231), alle Quellen der Spur aelter (beretta_r37.c 21:42:41, re15_beretta_r37.h
21:55:25, game_step_common.c / re15_damage.c 2026-10-12 15:28, test_r37_beretta.c 21:42:47); Baum sauber bis auf das
unversionierte `mess_r37g/`. Gemessen wird diese exe ueber eine Messkopie unter eigenem Namen.

## Ergebnis: NICHT BESTANDEN (ein Mangel, N1)
| Pruefung | Urteil |
|---|---|
| Punkt 1 (Beretta M93R: jeder Schuss des Feuerstosses trifft) | **erfuellt** — an der exe unter der Standard-KI drei Treffer E/E+3/E+6 fuer Zombie, Hund, Spinne, Gorilla, Kakerlake, 0x2b (auch Glock 18), Birkin, Alligator; Kraehe stirbt an Kugel 1; Browning unveraendert (Abschnitt 1) |
| M1-M4 der Abnahme 0 | behoben (1a, Abschnitt 2 Gegenprobe) |
| RE-Gate | **haelt NICHT vollstaendig** — 7 Belege selbst disassembliert und bestaetigt, keine Guess-Tells; aber die Nachbesserung aendert den Gitterarm 0x1A in jedem KI-Modus ohne Beleg, gegen das RE2-Vorbild (EM2D-HURT setzt +0x1D3 Bit 0x80 @0x80100fa8-b0, Applier-Gate 2 @0x80047138-40), mit falscher Header-Begruendung (N1) |
| Vertrag | haelt |
| Gezielte Tests | 89/89 Unit (Regex U), 5/5 exe-Integration (Regex X), Gegenprobe altes Kriterium 10 FAIL |

## 1. Punkt 1 (Nachtrag, Wortlaut)
> bei der Beretta M93R muss - wenn sie schon mehrfach schießt, das auch gelten das die Gegner mehrfach getroffen werden.
> Werden sie nur ein einziges mal getroffen bei 3 Schuss, ist es nur Schussverschwendung.

Messaufbau (eigenes Skript `abn1/run.sh` im Scratchpad der Abnahme, Messkopie `re15_abn1g.exe` neben der exe, Logs im
Scratchpad `abn1/mess/<lauf>/`; Auswertung eigenes `abn1/ausw.py`: Segment nach dem Raumsprung, je Bild Munitions-
und HP-Wechsel je Platz mit st/ss1, Muendung = wf.log `SPAWN id=2 sub=0`). Je Lauf:
`env -u RE15_AI_FLAVOR RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
RE15_DEBUG_JUMP=<raum>@250 RE15_PLAYER_POS=<x,z,rot> RE15_GIVE=<w>:15 RE15_EQUIP=<w> RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=M2.0,MA0.1,M3.0,W2 RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log
RE15_EXIT_AT=<bild>#<raum>` (Token-Semantik input_pc.c:61-90: M = R1, A = Square, 0.1 s = 3 Bilder = EIN Abzug).
"Standard-KI" = KEIN RE15_AI_FLAVOR (`env -u`), also so, wie der Nutzer spielt (Default RE2).

### 1a. Standard-KI, an der exe gemessen (Batch 1) — alle Laeufe `exit=0`, `[flow] EXIT_AT: Bild <n> in Raum <r>`
| Lauf | Raum / Ziel | Waffe | Munition | HP-Abzuege (Bild, dHP, Reaktion) | Muendung | Stoss |
|---|---|---|---|---|---|---|
| l_w5_std | 1140 Platz 5 Typ 0x11 | 5 M93R | F61 15 -> 12 | F61 -16, F64 -16, F67 -16 (st 2) | F61/F64/F67 | 48 |
| t2b_w5_std | 5040 Platz 1 Typ 0x2b hp 121, Leon (-14493,-13185) | 5 M93R | F61 15 -> 12 | F61 -10, F64 -10, F67 -10 (st 2 ss1 5) | F61/F64/F67 | 30 |
| t2b_w6_std | dito | 6 Glock 18 | F61 15 -> 12 | F61/F64/F67 je -10 (st 2 ss1 6) | F61/F64/F67 | 30 |
| t2b_w3_std (Gegenprobe) | dito | 3 Browning | F61 15 -> 14 | F61 -10 (einer) | F61 | 10 |
| roach_w5_std | 3040 Platz 2 Typ 0x29 hp 99, Leon (-13500,-10300) | 5 M93R | F61 15 -> 12 | F61 -14, F64 -14, F67 -14 (st 2 ss1 5) | F61/F64/F67 | 42 |
| birk_w5_std | 5080 Platz 1 Typ 0x30 hp 300, Leon (-18100,1700) | 5 M93R | F61 15 -> 12 | F61 -15, F64 -15, F67 -15 (st 2 ss1 5) | F61/F64/F67 | 45 |
Erholung nach dem Stoss (kein Haengenbleiben im HURT): 0x2b F76 st 2/5 -> 1/6, F77 -> 1/7, F87 -> 1/3; Kakerlake F79
st 2/5 -> 1/9; Zombie F89 st 2/4 -> 1/12. => Die Abnahme-0-Laeufe t2b_w5_def_a / roach_w5_def (vorher je EIN Treffer)
liefern jetzt drei Treffer im Takt der Muendungsblitze; M1 und M2 sind an der exe fuer 0x2b/0x29/0x30 behoben.

### 1b. Standard-KI, weitere Gegnerklassen an der exe (Batch 2) — ueber das hinaus, was der Bau-Agent gemessen hat
| Lauf | Aufbau | Waffe | Munition | HP-Abzuege | Muendung |
|---|---|---|---|---|---|
| gor_w5_std | `JUMP=11C0`, KEIN PLAYER_POS, Eingabe `W36.7,M1.0,MA0.1,M3.0,W2` (Abzug erst nach dem Raum-Ereignis: pm=2 bis ~F1100, dann pm=0/pst=1, Messlauf pos_11C0_l2), EXIT 1300#11C0 | 5 M93R | F1132 15 -> 12 | Gorilla 0x27 Platz 2 (hp 180): F1132 -12, F1135 -12, F1138 -12 (st 2 ss1 5) — Stoss 36 | F1132/F1135/F1138 |
| hund11d0_w5m | `JUMP=11D0 RE15_SET_FLAG=3:152`, Leon (-7000,-15900,0), Eingabe ab Bild 18 wie test_r35_redhawk.cmake (`M0.3,MA0.1,M0.6,MA0.1,...`) | 5 M93R | F49 15->12, F70 12->9, F111 9->6 | Stoss 1: Hund Platz 1 F49/F52/F55 je -18; Stoss 2: F70 -18, F73 -18 (hp -7, st 3 = tot), F76 kein Ziel mehr; Stoss 3: Hund Platz 5 F111/F114/F117 je -18 | je E/E+3/E+6 |
| hund11d0_w3m (Gegenprobe) | dito | 3 Browning | F49 15->14, F71 14->13 | F49 Platz 1 -18, F71 Platz 2 -18 (je einer) | F49, F71 |
=> Gorilla 0x27 (RE1.5-Gehirn, im Bau-Dossier nur Sonde) nimmt an der exe unter der Standard-KI alle drei Kugeln; der Hund
0x20 (RE2-Gehirn + RE2-Pausenfilter, Pause 0; von Abnahme 0 und vom Bau-Agenten nicht an der exe gemessen) ebenso. Die
Kugel, die nach dem Tod des Hundes kein Ziel findet (F76), ist die gewollte Regel "Sterbender wird nicht erneut getroffen"
(beretta_r37.c `e->hp < 0`, @0x80012520-2c), keine verlorene Kugel an einem lebenden Gegner.
Fehlversuche (Aufbau, nicht Spur): 2050-Spinne Platz 2 in Flucht-/Tief-/Hochzielen (spin_w5_std, _tief, _hoch) und
1120-Kraehen (kr_w5_std, kr1_*) — drei Blitze, kein Treffer, die Kraehen fliegen beim Naehern auf; 1230-Hunde und
2090-Alligator (hund_w5_std, gator_w5_std) — Leon wird vor dem Abzug getroffen (hp 80 / 50, pst 2), kein Abzug.

### 1c. Standard-KI, Alligator / Spinne / Kraehe (Batch 3)
| Lauf | Aufbau | Waffe | Munition | HP-Abzuege | Muendung |
|---|---|---|---|---|---|
| gator_c | `JUMP=2090`, Leon (-7200,-5200,1024), `M0.5,MA0.1,M3.0,W2` (frueher Abzug, bevor der Boss/Spinne 1 Leon trifft) | 5 M93R | F16 15 -> 12 | Alligator 0x23 Platz 20 (Boss-Modul, hp 3000): F16 -7, F19 -7, F22 -7 — Stoss 21 (= alter Einzeltreffer Spalte 5 = 21) | F16/F19/F22 |
| spin_e | `JUMP=2050`, Leon (-7000,-6082,0) quer zur Spinne Platz 2, vier Abzuege | 5 M93R | F16/F37/F58/F79 je -3 | Spinne 0x25: Platz 2 F16/F19/F22 und F37/F40/F43 je -17; Platz 1 F58/F61/F64 und F79/F82/F85 je -17 | je E/E+3/E+6 |
| spin_f | dito, Tiefzielen (`MD...`) | 5 M93R | F16/F38/F60 je -3 | wie spin_e, je drei Treffer -17 | je E/E+3/E+6 |
| kr_c / kr_d / kr_c_w3 | `JUMP=1120`, Leon (3900,1350,1024), gerade bzw. hoch | 5 M93R / 3 Browning | je -3 / -1 | KEIN Treffer — auch die Browning-Gegenprobe trifft nicht (Kraehe Platz 2 fliegt bei F16 schon, d=1360, r2272) | — |
=> Alligator (Boss-Modul, im Bau-Dossier nur Sonde) und Spinne nehmen an der exe unter der Standard-KI alle drei Kugeln.
Kraehe an der exe nicht getroffen — der Aufbau, nicht die Spur (Browning trifft dort ebenso wenig). Statt dessen Code-
Lage gelesen: RE2-HP der Kraehe fest 10 (`case 0x21: return 10;` re15_damage.c:956, @0x80100324/348), Schaden je Kugel
w5 = 15 (`s_re2_wpn_dmg_crow` re15_damage.c:644-647, Zeile 4 @0x800A45DC), Trefferpause 15 in allen 22 Spalten
(`s_re2_stun_crow` re15_damage.c:577-579). Die erste Kugel toetet also jede Kraehe; Kugel 2/3 gehen an den naechsten
Kandidaten (der tote ist mit HP < 0 ausgeschlossen). Der Riegel unit_r37_beretta Teil I Pruefung 96 (Kraehe mit 2000 HP:
nur Kugel 1) zeigt die Pausen-Regel; im Spiel kann sie an einer lebenden Kraehe nicht greifen. Kein Mangel.
Randnotiz: im Lauf hund_b steht die dritte Muendung im wf.log unter F21 statt F22 — dort fehlt die Kopfzeile von F22
(F21 traegt die Spawns zweier Bilder plus das Biss-Blut id=0 sub=1); in allen anderen Laeufen liegt sie exakt auf E+6.
Protokoll-Artefakt, kein Befund.

### 1d. Nebenwirkungen des Riegel-Loesens (Code gelesen, Laeufe 1a)
* riegel_loesen() fasst nur Plaetze an, die DIESER Feuerstoss getroffen hat (`s_getroffen`, nur Resolver-Rueckgabe > 0)
  und die leben (`hp < 0` -> bleibt, @0x80012520-2c); geloescht werden +0x93 Bits 0|1. Jeder Treffer setzt den Gegner
  neu auf st 2 (HURT), die Kette laeuft vom letzten Treffer an weiter — gemessen 0x2b: Einzelschuss F61 -> Erholung F70,
  Feuerstoss letzte Kugel F67 -> Erholung F76 (je 9 Bilder), danach normaler Ablauf (1/6, 1/7, 1/3).
* Bit 1 wird in den RE1.5-Gehirnen nur vom Gorilla im Sprung gelesen (enemy_ai_common.c:9217/9224, "im Flug getroffen:
  nur Gore"); ein Gorilla, den Kugel 1 getroffen hat, steht im HURT und springt in den 6 Bildern bis Kugel 3 nicht — der
  Fall ist nicht erreichbar. Die Kakerlake liest Bit 1 nicht (setzt es @0x801148a0-a4, loescht +0x93 @0x801149ac).
* Die Weiche `re2_pausenfilter_laeuft` ist der Spiegel des Bild-Nachlaufs (game_step_common.c:2383-2396: Filter nur fuer
  `re15_ai_re2_for_type`, Zombie-Familie `re15_re2z_owns_type` = 0x10/0x11/0x12/0x13/0x16/0x18 -> eigener Filter, sonst
  re15_re2_pause_filter_apply, dessen Typ-Tor re15_damage.c:3925 nur 0x20/0x21/0x25/0x26 durchlaesst). 0x1A (Gitterarm,
  RE2-Gehirn in jedem Flavor, enemy_ai_re2_zombie.c:172) faellt durch beide Filter — als Spiegel der Weiche stimmt das;
  dass der Arm dadurch die Kugeln 2/3 nimmt, die RE2 ihm per +0x1D3-Gate verweigert, ist Mangel N1 (1f).
  Uebrige Typen des Zensus: 0x24 = FX-Schwarm (harmlos, enemy_ai_common.c:13477), 0x2D = Efeu (Schadenszeile alles 0,
  re15_damage.c:769), 0x26 = Feuer (immun) — ohne Belang.

## 2. RE-Gate
Selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py` des Hauptbaums;
Binaerdateien `info/Re1.5/PSX.EXE` (Auslieferungsstand: `@0x80026e4c = 08 00 e0 03`), `info/Re1.5/PSX/BIN/STAGE1.BIN`,
`STAGE3.BIN`, `info/re2leon/PSX.EXE`):
1. Riegel `dis 0x800124e0 8`: `lbu v0,147(s1)` @0x800124e8, **`ori v0,v0,0x1` @0x800124F0**, `sb v0,147(s1)` @0x800124f8
   (+0x93 = Versatz 147). Bestaetigt.
2. Kakerlake (NEU in NB1 zitiert) `dis 0x80114898 5 --bin STAGE3.BIN`: `lbu v0,147(a0)` / **`ori v0,v0,0x2` @0x801148a0** /
   **`sb v0,147(a0)` @0x801148a4**; `dis 0x801149a4 4 --bin STAGE3.BIN`: **`sb zero,147(a0)` @0x801149ac** (erste
   Instruktion des Abschluss-Zweigs nach `j 0x80114a30` @0x801149a4 + Delay-Slot). Bestaetigt.
3. Zombie-Freigabe (Header) `dis 0x80105f94 8 --bin STAGE1.BIN`: `lbu v0,147(v1)` @0x80105f9c, `andi v0,v0,0xfe` @0x80105fa4,
   `sb v0,147(v1)` @0x80105fac (Delay-Slot des `jal 0x8001af20`). Bestaetigt.
4. RE2-Gate (NB1-Kommentar) `re2_disasm.py dis 0x80047130 6`: **`lbu v0,467(s0)` @0x80047138** (+0x1D3) /
   **`bne v0,zero,0x8004740c` @0x80047140**. Bestaetigt.
5. RE2-Kugelbilder `dis 0x80048908 12`: `lbu v1,333(s0)`, `beq v1,zero,0x80048958` @0x80048910, `addiu v0,zero,3` @0x80048914,
   `beq` @0x80048918, `addiu v0,zero,6` @0x8004891c, `bne v1,v0,0x800489c4` @0x80048920, `andi v0,s2,0x40` @0x80048928.
   Bestaetigt (BERETTA_KUGEL_ABSTAND 3).
6. RE2-Zeilen 4 (NB1-Kommentar) `bytes`: @0x800A4168 `10 3c e0 00 14 02 81 00` (Zombie: Schaden 16, Pause 1),
   @0x800A4154 `10 3c e0 00 14 0a 85 02` (Zeile 3: 16, Pause 5), @0x800A4460 `12 40 f0 00 0a 00 00 00` (Hund 18, Pause 0),
   @0x800A45DC `0f 3c f0 00 0a 1e 8f 07` (Kraehe 15, Pause 15), @0x800A4BCC `11 40 f0 00 0a 00 00 00` (Spinne 17, Pause 0);
   Pause = (w1 >> 9) & 0x7f, Schaden = w0 & 0x3ff (selbst gerechnet). Bestaetigt — und deckungsgleich mit den an der exe
   gemessenen Abzuegen (Hund -18, Spinne -17, Zombie -16).
7. RE1.5-Schadenstabelle (Unit-Test Teil I) aus PSX.EXE gelesen (u16 @0x8006e0d0 + Typ*0x58 + 3*4 / + 5*4): 0x23 7/21
   @0x8006ECE4/EC, 0x27 12/28 @0x8006EE44/4C, 0x29 14/25 @0x8006EEF4/FC, 0x2b 10/20 @0x8006EFA4/AC, 0x30 15/28 @0x8006F15C/64.
   Bestaetigt — gleich den gemessenen Abzuegen je Kugel (-7/-12/-14/-10/-15).
Guess-Tells in den HINZUGEFUEGTEN Zeilen (`git diff master...HEAD -- re15_port | grep '^+'` auf deferred|tunable|interim|
for now|faithful|plausib|TODO|FIXME|approx|getenv|sieht richtig): **keine**. Die NB1 fuehrt keine neue Konstante ein; die
Typ-Liste 0x20/0x21/0x25/0x26 ist die vorhandene Filter-Weiche (re15_damage.c:3925), keine Setzung der Spur.
Erklaert der Fix den Befund? Ja: Abnahme-0-Befund = 1 Treffer bei 0x2b/0x29 unter Standard-KI; Ursache = `riegel_loesen`
sprang im RE2-Flavor ueber JEDEN Typ (re15_ai_re2_for_type liefert dort 1), waehrend kein Filter den Riegel dieser Typen
freigibt. GEGENPROBE selbst gebaut (Scratchpad `abn1/neg/`: beretta_r37.c-Kopie mit dem alten Kriterium
`if (re15_ai_re2_for_type((unsigned)e->type)) continue;`, gelinkt vor libre15_engine.a mit test_r37_beretta.c, Flags aus
build.ninja): `test_r37_beretta: 10 FAIL` (72/73/76/77/80/81/84/85/88/89, je "n=0 ... Treffer 1, Riegel 0"); mit dem
ausgelieferten Kriterium `test_r37_beretta: OK`. Der Riegel faengt den Defekt also; und die exe-Laeufe 1a-1c zeigen
die Behebung. **RE-Gate fuer M1-M4: haelt.** Gesamturteil RE-Gate: **haelt NICHT vollstaendig** — die neue Einordnung
aendert den Gitterarm 0x1A ohne Beleg und gegen RE2 (Abschnitt 1f, Mangel N1).
(Die Abschnitte 1e-1g wurden nach diesem Abschnitt gemessen und stehen deshalb darunter.)

### 1e. Gehaltener Abzug (mehrere Feuerstoesse) gegen RE1.5-Gehirne, Standard-KI — Nebenwirkungspruefung
Eingabe `M2.0,MA2.0,M1.0,W2` (Abzug 2 s gehalten -> Stoesse F61/F82/F103), Gegenprobe Browning mit derselben Eingabe.
| Lauf | Waffe | Stoss 1 (F61) | Stoss 2 (F82) | Stoss 3 (F103) | Lage bei Stoss 3 |
|---|---|---|---|---|---|
| t2b_halten (5040, 0x2b) | M93R | F61/F64/F67 je -10 | F82/F85/F88 je -10 (Kette vom 1. Stoss endete F76) | kein Treffer | 0x2b bei (-15037,-14449), Leon x=-13709 Blick -z: ~1300 neben der Bahn (geht nach dem Treffer seitlich ab, st 1/7) |
| t2b_halten_w3 | Browning | F61 -10 | F83 kein Treffer | F105 kein Treffer | 0x2b bei F82 (-15149,-14318), Leon x=-13800: ebenfalls neben der Bahn |
| birk_halten (5080, 0x30) | M93R | F61/F64/F67 je -15 | kein Treffer | kein Treffer | Birkin bleibt bis F200 in st 2 (HURT, Riegel aus Kugel 3) |
| birk_halten_w3 | Browning | F61 -15 | F83 kein Treffer | F105 kein Treffer | Birkin bleibt nach EINEM Treffer ebenso bis F200 in st 2 |
=> Die Fehlschuesse der Folge-Stoesse treten mit der Browning genauso auf (Gegner tritt aus der Bahn bzw. Birkins lange
RE1.5-Trefferreaktion haelt jeden weiteren Treffer ab) — Bestand, nicht die Spur. Ein Folge-Stoss, der den Gegner nach
dem Ende seiner Kette trifft, trifft wieder dreimal (0x2b Stoss 2). Kein Mangel der Spur.

### 1f. ⛔ Gitterarm 0x1A (ROOM1210/1211): NB1 aendert sein Verhalten in JEDEM KI-Modus — gegen RE2, ohne Beleg
Lage: `re15_ai_re2_for_type(0x1A)` ist in jedem Flavor 1 (enemy_ai_re2_zombie.c:172), der Arm laeuft immer auf dem
RE2-Zellenarm-Gehirn (enemy_ai_re2_zellenarm.c). Das ALTE Kriterium uebersprang ihn deshalb in riegel_loesen() in jedem
Modus; das NEUE (`re2_pausenfilter_laeuft`) stuft ihn als "kein Filter" ein und loest seinen Riegel vor Kugel 2/3.
Der Header begruendet das so (re15_beretta_r37.h:53-54): "... und der Gitterarm 0x1a (ein Treffer toetet, HP < 0)".

Messung an der exe (Arm Platz 5 bei (-21490,-15747); Leon (-19500,-13000,1024), Eingabe `U0.8,R0.5,M1.0,MA0.1,M2.0,W2`
— Laufen weckt die Arme bei z ~ -13400, Arm 5 kommt F38 heraus; `RE15_RE2_TRACE=1`, Spur `re2_ki.log`, Kopie im
Scratchpad `abn1/re2_ki_arm.log`):
| Lauf | KI | Waffe | HP Arm 5 | Rueckzug (state-Wort 0x501 = 1/5, Trace) | Ende (0x701, hp -1, verborgen) |
|---|---|---|---|---|---|
| arm_r_w3 | Standard | Browning | F70 250 -> 245 | 5/0 -> 5/1 (Clip 4) einmal | F80 (E+10) bei (-21750,-15897) |
| arm_r / arm_r_tr | Standard | M93R | F70 -5, F73 -5, F76 -5 (250 -> 235) | 5/0 -> 5/1, dann bei JEDER Kugel erneut 5/0 (Clip 4 neu ab fr 1/2) — Rueckzug dreimal gestartet | F86 (E+16) bei (-21854,-15957), ~120 tiefer |
| arm_r_re15 | RE1.5 | M93R | F70/F73/F76 je -5 | wie arm_r_tr (Trace bitgleich) | F86 |
| arm_l / arm_a | Standard | M93R | je drei Treffer -5 | — | F86 / F95 |
=> Der Arm STIRBT nicht am Treffer (hp 245), sondern erst am Ende seines Rueckzugs (`e->hp = -1` enemy_ai_re2_zellenarm.c:785
= `sh -1,342` @0x80100E4C-50). Die Header-Begruendung ist damit falsch. Jede weitere Kugel startet den Rueckzug neu: arm_hurt
(state-Wort 0x501 + Ton 2/3, zellenarm.c:810-815) und B5 Phase 0 (Clip 4 von vorn + Ton 6, zellenarm.c:772-777) laufen je
Kugel — drei Treffer-/Rueckzugstoene statt einem, der Rueckzug dauert 16 statt 10 Bilder und endet ~120 Einheiten tiefer.
Die Kugeln 2/3 bleiben am ohnehin sterbenden Arm haengen, statt den naechsten Kandidaten zu erreichen.

Was RE2 Retail (das Vorbild der Spur UND des Arm-Gehirns) tut — selbst disassembliert:
* EM2D-HURT (`RE_OVERLAY_DIR=build/extracted/re2_ems re2_disasm.py dis 0x80100F90 14 --bin CDEMD0_EM2D_ai0.BIN`, ebenso ai1):
  `lbu v0,467(s0)` @0x80100f9c, `addiu v1,zero,1281` / `sw v1,4(s0)` @0x80100fa0-a4 (0x501), **`ori v0,v0,0x80` @0x80100fa8**,
  **`sb v0,467(s0)` @0x80100fb0** -> +0x1D3 |= 0x80 im Trefferbild.
* Applier-Gate 2 `lbu v0,467(s0)` / `bne v0,zero,0x8004740c` @0x80047138-40 (PSX.EXE RE2, Abschnitt 2 Nr. 4): GANZES Byte
  +0x1D3 != 0 -> naechster Kandidat.
=> In RE2 nimmt ein getroffener Arm die Kugeln 2/3 der Custom Handgun NICHT mehr; sie gehen an den naechsten Kandidaten.
Gegenprobe Kriterium alt/neu (Sonde `abn1/neg/sonde_arm.c`, Aufbau wie test_r37_beretta Teil I, Typ 0x1A, HP 2000):
`alt: RE2 Treffer 1 | RE1.5 Treffer 1`, `neu: RE2 Treffer 3 (Riegel geloest 2) | RE1.5 Treffer 3 (Riegel geloest 2)`.
Die Aenderung kommt also aus der Nachbesserung 1, gilt in JEDEM KI-Modus und steht im Dossier 8.3 falsch ("Unveraendert:
... alles unter RE1.5-KI und MIXED"). Der Unit-Test pinnt die Einordnung (`OHNE[] = {..., 0x1a}`, Pruefung 70) ohne Beleg;
einen Riegel fuer den Arm gibt es nicht. -> Mangel N1.
(Zum Vergleich ohne Befund: G5 0x36 — ohne Schuss-Sperre gibt das Modul Bit 0 im selben Bild selbst frei
(enemy_ai_boss_g5.c:1589-1623), die Kugeln treffen mit altem wie neuem Kriterium; Kraehe/Hund/Spinne/Zombies laufen ueber
ihren Filter wie vorher.)

### 1g. Urteil Punkt 1: **erfuellt** (Wortlaut) — mit Nebenwirkung N1 (Abschnitt 5)
An der exe unter der Standard-KI gemessen nimmt jede Gegnerklasse, die der Feuerstoss erreicht, mehrere Kugeln: Zombie
(1140), Hund 0x20 (11D0), Spinne 0x25 (2050), Gorilla 0x27 (11C0 nach dem Raum-Ereignis), Kakerlake 0x29 (3040), 0x2b
(5040, auch Glock 18), Birkin 0x30 (5080), Alligator 0x23 (2090, Boss-Modul) — je drei Treffer im Takt E/E+3/E+6, drei
Patronen, Reaktion je Treffer. Die Kraehe stirbt an Kugel 1 (10 HP / 15 Schaden, Code), es geht kein Treffer an einer
lebenden Kraehe verloren. Elza: integration_r37_beretta `elza_re2_w5` gruen (unten). Browning-Gegenproben unveraendert
(ein Treffer je Abzug). Die Maengel M1-M4 der Abnahme 0 sind behoben (M1/M2: 1a; M3: Riegel faengt den Defekt, Abschnitt 2;
M4: Header/Kommentar/Dossier 8.x neu — bis auf die 0x1A-Aussage, s. N1).
Echter Weg: Waffenmechanik, raumunabhaengig — gemessen ueber den Debug-Sprung; in 11C0 ueber den vollen Raum-Ereignisablauf
(Steuerung erst ab ~F1100), in 1210 ueber das Weck-Ereignis beim Durchlaufen des Flurs (nicht per Platzierung).

## 3. Vertrag (VERTRAG.md)
* Pfade (`git diff master...HEAD --name-only`, 22 Dateien): analysis/befunde_runde37/G_* (Dossier, Abnahme 0, Messskripte),
  engine/src/beretta_r37.c, include/re15_beretta_r37.h (neu), engine/src/game_step_common.c (+11/-3: Include, Tick-Zeile,
  Rueckgaben merken, Feuerstoss-Aufruf), engine/src/re15_damage.c (+6: eine Codezeile), tests/unit/probes/r37_beretta.cmake,
  tests/unit/test_r37_beretta.c, tests/integration/test_r37_beretta.cmake (neu), tests/unit/test_re2_hp_model.c (+5/-1,
  begruendet). Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists. Die Nachbesserung 1
  (`git diff 6caac126..HEAD -- re15_port`) beruehrt nur beretta_r37.c/.h und die eigenen Tests — gemeinsame Dateien
  unveraendert seit Abnahme 0. **Haelt.**
* Bank-9-Bits / Nachrichten-Ids / Ereignisse / Slots: keine (Suche im Diff). **Haelt.**
* Tests nur ueber `probes/r37_beretta.cmake`; Integration mit `cmake_minimum_required(VERSION 3.16)`, Messkopie
  `re15_pc_r37g_haken_<id>` (Muster test_r34n_b_cursor), Unit-Test ohne POSIX-Aufrufe. **Haelt.**
* Tests je Punkt vorhanden und messend (Abschnitt 4). Luecke: kein Riegel fuer 0x1A (N1).

## 4. Gezielte Tests (eigener Lauf im Spurbaum, keine volle Suite)
* Regex U des Bau-Agenten (eigene + alle Schuss-/Schadens-/Waffen-Gruppen + Gegnertypen der Klasse, 89 Tests):
  `ctest --test-dir re15_port/build -R "^(unit_r37_beretta|...|unit_pri_inventar_riegel)$" --timeout 600 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 89** (26.39 s). test_r37_beretta direkt: 50 x `ok`, `test_r37_beretta: OK`.
* Regex X (exe-Laeufe der beruehrten Schusspfade): `-R "^(integration_r37_beretta|integration_r36_schuss|
  integration_r36_elza_schuss|integration_r35_werfer|integration_r36_granate)$" --timeout 900` -> **100% tests passed, 0 tests
  failed out of 5** (527.83 s; r35_werfer 133.33, r36_elza_schuss 44.50, r36_granate 91.22, r36_schuss 125.04,
  r37_beretta 133.70). Keine Fenster-Flatterer, kein Nachfahren noetig.
* Gegenprobe gegen den Defekt (Abschnitt 2): Teil I mit altem Kriterium 10 FAIL, mit neuem OK.
* Aufraeumen: Messkopie `re15_abn1g.exe` und das von meinen Trace-Laeufen angelegte `re2_ki.log` (Geburt 22:23:34, nur meine
  Laeufe) im exe-Ordner geloescht; kein taskkill, alle Laeufe endeten ueber RE15_EXIT_AT mit exit=0.

## 5. Maengel (nummeriert, nachpruefbar)

**N1 — Gitterarm 0x1A: die Nachbesserung laesst Kugel 2/3 den getroffenen, sich zurueckziehenden RE2-Arm erneut treffen
— gegen RE2, ohne Beleg, mit falscher Begruendung; in JEDEM KI-Modus.**
* Messung: Abschnitt 1f (arm_r_tr / arm_r_re15 gegen arm_r_w3): M93R F70/F73/F76 je -5, Rueckzug (0x501) dreimal neu
  gestartet (Trace 5/0 -> 5/1 -> 5/0 ...), Ende F86 statt E+10, ~120 Einheiten tiefer, drei Treffer-/Rueckzugstoene.
* Ursache: `re2_pausenfilter_laeuft(0x1A)` = 0 -> riegel_loesen() loest +0x93 vor Kugel 2/3. Das alte Kriterium hatte 0x1A in
  jedem Modus uebersprungen (Sonde alt/neu: 1 -> 3 Treffer).
* Beleg RE2 (selbst disassembliert): EM2D-HURT `ori v0,v0,0x80` / `sb v0,467(s0)` @0x80100fa8/@0x80100fb0 (+0x1D3 |= 0x80 im
  Trefferbild) + Applier-Gate 2 `lbu v0,467(s0)` / `bne v0,zero` @0x80047138-40 -> in RE2 ist der getroffene Arm fuer Kugel
  2/3 kein Kandidat; sie gehen an den naechsten (z.B. den naechsten Arm). Im Port bleiben sie am ohnehin sterbenden Arm
  haengen (er stirbt am Rueckzugsende, `sh -1,342` @0x80100E4C-50) — fuer den Nutzer genau die "Schussverschwendung",
  die der Auftrag abstellen soll.
* Doku falsch: re15_beretta_r37.h:53-54 "der Gitterarm 0x1a (ein Treffer toetet, HP < 0)" (gemessen hp 245/240/235 waehrend des
  Rueckzugs); G_beretta.md 8.3 "Unveraendert: ... alles unter RE1.5-KI und MIXED" (fuer 0x1A falsch, Sonde + arm_r_re15).
* Test: Pruefung 70 pinnt `0x1a` als "kein Filter" ohne Beleg; kein Riegel fuer den Arm.
* Erwartung nach Behebung (nachpruefbar mit dem Aufbau aus 1f): unter Standard- UND RE1.5-KI ein Treffer am Arm je Stoss, der
  Rueckzug laeuft einmal (Ende E+10 wie Browning); Kugel 2/3 duerfen einen weiteren Kandidaten in der Bahn treffen. Die
  RE1.5-Gehirne 0x23/0x27/0x29/0x2b/0x30 muessen weiter drei Treffer nehmen (sie tragen nach einem Schuss kein +0x1D3:
  `re15_re2_stun_frames` -> 0, re15_damage.c:2696). Header, Dossier 8.3 und Pruefung 70 entsprechend; Riegel fuer 0x1A.

Kein Mangel (geprueft): M1-M4 der Abnahme 0 behoben (exe 0x2b/0x29/0x30, zusaetzlich 0x27/0x23/0x20/0x25 an der exe), RE-Belege
(Abschnitt 2), Kugel-Takt, Patronen, Gegenproben Browning, Folge-Stoesse (1e), G5, Kraehe, Vertrag, alle gezielten Tests gruen.
