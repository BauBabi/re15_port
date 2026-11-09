# Integration Runde 41 - Labor-Birkin: Biss trifft nach dem Haken wieder (2026-11-09)

Baum `.claude/worktrees/r41_integration`, Zweig `r41/integration`, Basis a3051c902, Fix-Commit 145316fc9.
Laufordner (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/labor_check/angriff`
(RE-Bericht `RE.md`, Disasm mit Bytes `s5_*.txt` / `exe_*.txt`, Laeufe `f_*` = Fix, `o_*` = alter Stand, Riegel-Laeufe `riegel/neu`, `riegel/alt`).

## 0. Ergebnis

- **Befund:** Im Labor (50E0 Leon, 50F1 Elza) traf der Birkin nach dem ersten Treffer (Haken) den stehenden Spieler nie wieder.
  Er stand bei Abstand ~2480 in der Schleife ss1 4 -> 1 -> 4, Spieler-hp blieb 90.
- **Ursache:** `re15_bk_kontakt` (Angriffsbox FUN_8001a5e0, seit 555556172) posierte die Klaue (Teil 15) am Keyframe N = +0x95
  nach anim_set. Das Original prueft die GEZEICHNETE Welt-Matrix = Keyframe N-2. Die Klaue ist bei KF 0x21 am Spieler, das
  Bissfenster ist 0x23..0x2a; der Port pruefte also, waehrend die Klaue schon zurueckschwang.
- **Fix (ein Eingriff):** `re15_bk_kontakt` posiert Teil 15 bei `(anim_frame + fc - 2) % fc` (fc = `re15_birkin_fc`),
  derselbe Versatz, den `re15_birkin_fuss_schritt` fuer das alte Bild schon nimmt. Keine neue Konstante; Satz-Werte unveraendert.
- **Riegel:** `integration_r41_labor_birkin_angriff` (echter Weg ab der 50D0-Tuer, Leon 50E0 + Elza 50F1): gruen mit Fix,
  ROT mit a3051c902.
- **Kommentare** auf STAGE5 berichtigt (INIT @0x80117094 / @0x801170a4, DECIDE @0x80117854, Saetze und Box-Aufrufe).

## 1. Beleg (Disasm, selbst nachgelesen)

Bytes gegen `info/Re1.5/PSX/BIN/STAGE5.BIN` (raw @0x80100000) und `info/Re1.5/PSX.EXE` (t_addr 0x80010000) geprueft:

| Adresse | Bytes | Bedeutung |
|---|---|---|
| EXE 0x8001a5ec | `40 00 04 26` | addiu a0,s0,64: Teil+0x40 = Welt-Matrix |
| EXE 0x8001a604 | `8a 9e 01 0c` | jal ApplyMatrixSV(Teil+0x40, Versatz) |
| EXE 0x8001a610 | `54 00 02 96` | lhu v0,84(s0): + Teil+0x54 (T.x) |
| EXE 0x8001a638 | `5c 00 04 96` | lhu a0,92(s0): + Teil+0x5c (T.z) |
| EXE 0x8001a6b4 | `2a 10 50 00` | slt: Treffer = d < Radius + Reichweite |
| EXE 0x80073e94 | `00 00 06 fa 00 00 c2 01 fa 05 c2 01` | Radius-Satz, +6 = 450 |
| EXE 0x8001f35c | `95 00 02 91` | anim_set: lbu v0,149(t0), Bild = +0x95 (alt) |
| EXE 0x8001f36c | `68 01 06 ad` | sw a2,360(t0): Frame-Eintrag dieses Bildes |
| EXE 0x8001f618 / 61c | `01 00 42 24` / `95 00 62 a0` | erst danach +0x95 += 1 |
| S5 0x8011be88 / 0x8011bea8 | `54 00 64 8e` / `5c 00 64 8e` | Fussvortrieb liest die ALTE Welt-Lage +0x54/+0x5c gegen frisch +0x18 |
| S5 0x801183a8 | `c5 7c 00 0c` | Biss: jal anim_set |
| S5 0x801183e0 / e4 | `dd ff 42 24` / `08 00 42 2c` | Fenster +0x95 in [0x23..0x2a] |
| S5 0x80118400 / 404 | `78 69 00 0c` / `14 0a 84 24` | jal 0x8001a5e0, a0 = Teil 15 (+0xa14) |
| S5 0x801181fc / 0x8011822c | `04 04 a5 24` / `0c 04 c6 24` | Satz 0x80100404 = `84 03 51 01 00 00 00 00` (900,337,0), Reichweite 0x8010040c = `f4 01` (500) |
| S5 0x80117854 / 878 | `c4 09 42 2c` / `01 04 02 34` | DECIDE dist < 0x9c4 -> 0x401 (Biss) |
| S3 0x80117040 / 064 | `c4 09 42 2c` / `01 04 02 34` | dieselbe Schwelle in STAGE3 (Port-Kommentar nannte @0x80117038) |
| S5 0x80117094 / 0x801170a4 | `95 00 82 a0` / `05 00 50 a0` | INIT Nibble 3: +0x95 = 0x10, Sub 9 (STAGE3 @0x80116880 / @0x80116890) |
| S5 0x80117f24 / 0x801186b4 / 0x801192f0 | `78 69 00 0c` | Box-Aufrufe Kralle / Wurf / Tackle |

Zwischen 0x801183ac und 0x80118400 steht kein jal/jalr (selbst gescannt). Der Tick mit +0x95 = N (nach anim_set) hat lokal
KF N-1 in +0x18 und die gezeichnete Matrix von KF N-2 in +0x40/+0x54. Der Box-Satz ist in STAGE3 und STAGE5 bytegleich (+0x48),
der Typ-0x30-Code bis auf Relokationen gleich (RE.md 1.1: 123 Diff-Woerter, nur j/jal +0x814, Zeiger +0x48/+0xfc4).

## 2. Aenderung

- `re15_port/engine/src/enemy_ai_common.c`, `re15_bk_kontakt`: Teil 15 bei KF `(anim_frame + fc - 2) % fc`, danach anim_frame
  zurueck. Ohne Bank weiter der alte Proxy (Unit-Harness), fuer 0x36 unveraendert der Proxy-Zweig.
- Kopfkommentar der Box: STAGE5-Saetze, Box-Aufrufe, Beleg N-2 mit Bytes.
- INIT-/Spawn-Kommentare: STAGE3 @0x80116880 / @0x80116890 bleibt, STAGE5 @0x80117094 / @0x801170a4 dazu.
- DECIDE-Kommentar Sub 4: STAGE3 @0x80117040 statt @0x80117038, STAGE5 @0x80117854 / @0x80117878.
- Neuer Riegel `re15_port/tests/integration/test_r41_labor_birkin_angriff.cmake`, registriert in `tests/unit/CMakeLists.txt`.

## 3. Betroffene Raeume (ausdruecklich)

Der Fix wirkt auf jeden Typ 0x30 mit Bank, der ueber `re15_birkin_ai_tick` laeuft, an allen vier Box-Stellen
(Kralle, Biss, Wurf, Tackle):
- STAGE5: 5080, 50E0, 50F1, Hangar 50A0 (EM30_HANGAR).
- **STAGE3 3070/3071 aendern sich ebenfalls - belegt richtig:** der STAGE3-Code ist derselbe (gleiche Box FUN_8001a5e0, gleiche
  anim_set-Reihenfolge, STAGE3-Biss-Box @0x80117bec `78 69 00 0c`, Saetze @0x801003bc/@0x801003c4); der Port pruefte dort also
  ebenfalls das falsche Bild.
- Nicht betroffen: 0x36 (3080 Szene, 5090 G5) - Proxy-Zweig `e->type != 0x30`; Bahnsteig-Form (`re15_bb_tick`, eigenes
  `bb_nah`); Endkampf-G5 (`enemy_ai_boss_g5.c`).
- Folge im Kampf: der Haken (Kralle) trifft zwei Bilder spaeter (gemessen af 37 -> af 39 = +0x95 0x27, wie RE.md vorhersagt);
  der Biss trifft beim ersten Fensterbild +0x95 = 0x23.

## 4. Messung

Echter Weg: Debug-Sprung nur fuer den Raum, Spieler an der Tuer-Ankunft aus 50D0 (50E0 (-25450,-14700); 50F1 (-24550,-23650)),
Route der Gegenpruefung durchs Ausloesefeld, danach stehen und schiessen (R1 + Quadrat, R1 je Abschnitt kurz los).

| Lauf | exe | Haken | naechster Treffer | Birkin tot | Bits 3:73/3:84 |
|---|---|---|---|---|---|
| f_l4 Leon 50E0, Pistole | Fix | F929 (Kralle af 39) | F1074 Biss af 35, hp 90 -> 80; weiter F1277, F1444 | F3723 | F3844 |
| o_l4 Leon 50E0, Pistole | a3051c902 | F927 (Kralle af 37) | keiner bis zum Tod | F3232 | F3353 |
| f_e4 Elza 50F1, Pistole | Fix | F874 | F1016 Biss af 35; danach 8 weitere Treffer bis hp 0 | nicht bis F5000 (hp 270) | - |
| f_e5 Elza 50F1, Super Redhawk | Fix | F873 | F1014 Biss af 35 | F1885 | F2006 |
| riegel/neu Leon / Elza | Fix | F931 / F873 | F1075 / F1014 Biss af 35 -10 | F1785 / F1885 | ja |
| riegel/alt Leon / Elza | a3051c902 | F929 / F871 | Biss F1061..F1124 / F1000..F1062 ohne Treffer | F1310 / F1391 | (ROT vorher) |

Spieler-hp sinkt im Fix-Lauf mit stehendem Spieler bis 1/0 (Klemme `hp < 0 -> 1`); der Birkin ist mit der Pistole und einem
staendig getroffenen Skript-Schuetzen in 50F1 nicht in 5000 Bildern zu toeten (f_e4; f_e6 hp 150 bei F9000). Der Riegel nimmt
deshalb die Super Redhawk (Id 7, Birkin-Schaden 50 aus s_wpn_dmg_birkin, Tabelle @0x8006e0d0). Hinweis fuer den Release: der
Laborkampf ist mit dem Fix wieder deutlich schwerer (wie v0.8.29, dort traf der Abstands-Proxy zufaellig bei N = 0x23).

## 5. Riegel `integration_r41_labor_birkin_angriff`

Je Lauf (Leon 50E0, Elza 50F1, 3000 Bilder): (3,61) 0 -> 1; erster Spieler-Treffer (Haken); die ERSTE Biss-Episode danach
(ss1 = 4) trifft mit af = 35 (+0x95 = 0x23) und -10; Birkin st = 3 mit hp <= 0; (3,73) und (3,84) 0 -> 1.
Gegenprobe mit der exe von a3051c902 (`<SP>/exe_head`, Kopie neben der Bau-exe): ROT,
"leon: Biss F1061..F1124 nach dem Haken F929 verfehlt den stehenden Spieler", "elza: Biss F1000..F1062 nach dem Haken F871 verfehlt ...".

## 6. Tests

Bau: `RE15_BUILD_DIR=.../r41_integration/re15_port/build bash re15_port/tools/local_build.sh configure` / `build`:
`=== LOCAL-BUILD-OK (configure)`, `=== LOCAL-BUILD-OK (build)`.

`ctest -R "birkin|labor|sichel|bahnsteig|form|r41_birkin|g5" -j 2 --timeout 1800` (Summenzeilen woertlich):

    100% tests passed, 0 tests failed out of 40
    Total Test time (real) = 1335.71 sec

darunter integration_r41_labor_birkin_angriff (221 s), integration_r41_birkin_pistole, integration_r41_sichel_form,
integration_r41_g5_westen, integration_r41_g5_tod, alle integration_r41_form_wagenweg*, unit_birkin_opfer, unit_birkin_ai,
unit_r41_birkin_wand_*. Kein roter Fenster-Riegel, nichts nachzufahren.

`ctest -j 6 --timeout 600 -R "^(unit_|prop_|probe_)" -E "^(unit_r38_releaseskript_|unit_r30_android_|unit_r35_android_|unit_r39_titelversion_cmake|unit_r39_titelversion_ablauf$)"`
(nach dem letzten Bau, der nur die Kommentar-Aenderung zum Tackle nachzog):

    100% tests passed, 0 tests failed out of 1100
    Total Test time (real) = 266.03 sec

Keine bestehende Riegel-Erwartung an den Trefferzeitpunkt musste neu vermessen werden (Birkin-Opfer- und Sichel-Riegel gruen).

## 7. Offen

- **Schritt 2 (eigene Abnahme):** Fussvortrieb FUN_8011bdd8 Modus 0 in ACT[3] (@0x801181c0-c8), ACT[4] (@0x80118508-520),
  ACT[5] (@0x80118770-778), ACT[6] (@0x801190dc-e4) und Modus 1 + 0x14 im WALK (@0x80117aa0-ac0). Aendert Anlauf und
  Kampfabstand in allen 0x30-Raeumen und beruehrt die Wandklemme; nicht in diesem Commit.
- **Ueberblendung +0x8f** (@0x8001f474 / @0x8001f4a8-4b0): KF 0x21 ist zu 1/8 noch alte Pose; nicht simuliert (Abstand 221
  bzw. 506 gegen Schwelle 950).
- **Lageanteil der gezeichneten Matrix:** sie traegt die Wurzel-Lage des Vorbilds. Kralle/Biss/Wurf bewegen die Port-Wurzel im
  Fenster nicht; der Tackle schiebt im Port 0x50 vor der Box, im STAGE5-ACT[7] liegt zwischen anim_set @0x80119230 und der Box
  @0x801192f0 kein jal 0x800245d8. Welche Lage dort gilt, ist nicht geprueft.
- **Bahnsteig-Form** (`bb_nah`, RE2 FUN_80015834): dieselbe Bildfrage, eigens gegen RE2 zu pruefen.
- ROOM50E0/50F1 gibt es nur im Mod-Baum (GEGENPRUEFUNG 3); gemessen ist das Verhalten am Repo-Baum.
- 3070/3071 und 50A0 sind nach dem Fix nicht eigens im Spiel gemessen (nur durch den gleichen Code belegt).
