# Abnahme ZA 1: Zombie-Spawn-Variante ("duenne Stummelarme")

Geprueft: Zweig `r41/zombiearm`, Kopf `1100251c8` (Dossier `ZA_zombiearm.md`, Riegel `unit_za_spawnvariante`).
Pruefer: unabhaengige Abnahme, 2026-11-02. Belege: `ZA_belege/abnahme1/`. Am Spielcode wurde nichts geaendert.

## Gesamturteil: BESTANDEN, mit Maengeln in der Begruendung

Die Abschaltung ist richtig. Ich kann sie jetzt staerker belegen als die Spur. Die Stelle, die die Bits loescht und
die die Spur als "offen" gefuehrt hat, ist gefunden. Es ist **FUN_8001ee68**, aufgerufen @0x80100fe0 am Ende desselben
INIT, und zwar fuer **jeden** Zombie. Die Variante wird also im selben Funktionsaufruf gesetzt und wieder ueberschrieben,
bevor irgendein Bild gezeichnet wird. Das gilt auch auf der echten Disc (Version_40%).

Zwei Aussagen im Dossier und im Code-Kommentar sind falsch, aendern das Ergebnis aber nicht (M1, M2):
- Der Kommentar "nur in diesem Zweig" (enemy_ai_common.c:2184) ist falsch. Er stammt aus 53981ba5d und war die Ursache des Fehlers.
- "Ein Arm kaeme im Original nie dran" stimmt nur fuer die Mod-EXE.

---

## Pruefpunkt 1: RE-Beleg Zeichner, Schreiber, Savestates: BESTANDEN

### 1a Welche Datei hat die Spur benutzt?
`re15_disasm.py` liest fest `info/Re1.5/PSX.EXE` und `info/Re1.5/PSX/BIN`, also den Fan-Mod. Fuer diese Abnahme gibt es
`dis2.py`: dasselbe Werkzeug, aber wahlweise auf `Version_40%/` (`disc`) oder auf den Mod (`mod`) umgebogen.
- Vergleich EXE Mod gegen Disc: 3630 Worte in 81 Gruppen sind verschieden (`exe_diff_mod_vs_disc.txt`). Der Mod springt
  ausserdem in nachgeladenen Code @0x800c0000+ (z. B. `jal 0x800c00a8` @0x80031680, `jal 0x800c01c4` @0x800316d0).
- In **keiner** Diff-Gruppe liegen:
  - der Zeichner FUN_8001e9ec und die Teil-Schleife FUN_8001e8c8,
  - FUN_8001e5b0 und FUN_8001ee68,
  - Sce_em_set @0x80042524-0x8004267c.
  Die Disassembly des Zeichners ist ueber 160 Instruktionen zwischen Mod und Disc identisch.
- STAGE1-5: Der INIT mit Varianten-Tabelle, Routinen und Schwanz ist in beiden Baeumen gleich (`stage1-5_init_schwanz.txt`).
- **Abweichung, die die Spur uebersehen hat:** Der Zufallsgenerator FUN_8001af20 ist verschieden (siehe 1e).

### 1b Zeichner (Version_40%/PSX.EXE)
Zeichner FUN_8001e9ec, s2 = Teil-Zeiger (`addu s2,a0,zero` @0x8001e9f4):

| Adresse | Bytes | Instruktion | Bedeutung |
|---|---|---|---|
| @0x8001ebb0 | `00 04 42 30` | `andi v0,v0,0x400` | |
| @0x8001ebb4 | `0b 00 40 10` | `beq v0,zero,0x8001ebe4` | |
| | | `lh 136/138/140(s2)` | Skala laden |
| @0x8001ebdc | `fc 97 01 0c` | `jal 0x80065ff0` | ScaleMatrix |
| @0x8001ebec | `00 20 42 30` | `andi v0,v0,0x2000` | zweiter Skalen-Pfad (fehlt im Dossier) |

- Der einzige Aufrufer ist `jal 0x8001e9ec` @0x8001e990. Er liegt in der Teil-Schleife FUN_8001e8c8: `lw s1,392(s0)`,
  Schritt `addiu s1,s1,172`.
- Tinte: Bit 0x80 fuehrt zu `jal 0x8002441c` mit +0x94/+0x98 (@0x8001ed44-58).
- Ohne 0x400 und ohne 0x2000 wird nichts skaliert. Ohne 0x80 wird nichts getoent.

### 1c Wer loescht die Bits? FUN_8001ee68 am Ende JEDES Zombie-INIT (neu)
`disasm_belege.txt`:
```
FUN_8001ee68 (EXE, Disc == Mod)
8001ee90  lw   s1,392(s0)          ; Teile
8001eeec  lbu  s0,131(s0)          ; 83 00 10 92  Teilzahl +0x83 (Zombies: 15)
8001ef04  ori  s3,zero,0x1         ; 01 00 13 34
8001ef14  sw   s3,0(s1)            ; 00 00 33 ae  Teil-Wort = 1  (loescht 0x400|0x80)
8001ef1c  sw   s2,104(s1)          ; 68 00 32 ae  +0x68 = 0x808080
8001ef20  addiu s1,s1,172          ; ac 00 31 26
```
- Es gibt keinen Store auf +0x88/+0x8a/+0x8c (Skala) und keinen auf +0x94/+0x98 (Tinte).
- Genau das zeigen die Savestates: Skala und Tinte der Variante bleiben stehen, das Wort ist 1.

Aufruf im STAGE1-INIT FUN_80100688:
```
80100f64  lbu  v0,9(a0) / andi 0x1f / ori v1,zero,0xe
80100f70  bne  v0,v1,0x80100fd8    ; 19 00 43 14  jeder NICHT liegende Zombie springt direkt zum Reset
  ...     (Liege-Zweig 0x0e faellt durch bis 0x80100fd8)
80100fd8  lw   a0,g_entity(cur)
80100fe0  jal  0x8001ee68          ; 9a 7b 00 0c
80101000  jr   ra                  ; einziges jr ra des INIT
```
- `skipcheck.py`: Zwischen INIT-Anfang und `jr ra` gibt es kein weiteres `jr ra`, kein `jr reg` und keinen Sprung hinter
  oder vor den Reset. Das gilt fuer beide Baeume und fuer alle fuenf Stages:

  | Stage | Variante (jalr) | Reset (jal 0x8001ee68) |
  |---|---|---|
  | STAGE1 | @0x801008ec | @0x80100fe0 |
  | STAGE2 | @0x80100780 | @0x80100e74 |
  | STAGE3 | @0x801009d8 | @0x801010cc |
  | STAGE4 | @0x801008a0 | @0x80100f94 |
  | STAGE5 | @0x80100a20 | @0x80101114 |

  STAGE6 hat keine Variante.
- Zwischen Variante und Reset wird nicht gezeichnet:
  - Direkt aufgerufen werden nur 0x80019700, 0x8001af20, 0x8001af5c und 0x8001f314.
  - Transitiv (Tiefe 6, 18 Funktionen) wird weder 0x8001e8c8 noch 0x8001e9ec erreicht.
  - 0x8001e8c8 wird nur aus der Bildschleife @0x8001d09c/@0x8001d108 gerufen.
- Reihenfolge in Sce_em_set:
  1. Teil-Init `jal 0x8001e5b0` @0x80042530 (`6c 79 00 0c`)
  2. `sw s0,g_entity(cur)` @0x80042578
  3. erster Handler-Aufruf `jalr v0` @0x8004259c (`09 f8 40 00`), Zustand 0 = INIT.

  Der INIT laeuft also komplett (Variante, dann Reset) innerhalb von Sce_em_set.

**Folge:** Im Original bleibt die Variante in keinem Bild sichtbar. Das ist im Code belegt, nicht nur aus Savestates erschlossen.

### 1d Gibt es einen anderen Weg zu Bit 0x400? (`ori400_teilwort_und_skala.txt`, `ori400_alle_binaries.txt`)
Gescannt wurden alle `ori r,r,0x400|0x2000|0x480|0x2400`, alle Konstanten-Ladungen 0x400/0x2000 und alle `ori r,zero,0x5f4|0x7e8`
in der EXE und in STAGE1-6 (Disc und Mod identisch).

- **EXE:** Drei Treffer, keiner schreibt ein Teil-Wort:
  - @0x80025418 berechnet einen Rueckgabewert,
  - @0x80069ba4 und @0x8006b8c8 setzen GPU-Draw-Mode-Bits (`lui 0xe100`).
- **STAGE1:** Jeder Setzer von 0x400 auf ein Teil-Wort schreibt im selben Block alle drei Skalen-Halbworte desselben Teils.
  Ein Freilegen alter Varianten-Skalen ist damit ausgeschlossen. Ausser den Varianten-Routinen sind das:

  | Funktion | Setzer | Teile, Skala | Erreichbar ueber | Art |
  |---|---|---|---|---|
  | 0x80104b40 | @0x80104e68, @0x80104ebc | 1 und 4, 0x5f4/0xc18 | Tabellen @0x8011f9ec / @0x80120398; davor Effekt `jal 0x80019700` mit a0 = 0x05002000 @0x80104df4-e10 | Gore-Untermodus |
  | 0x80107ee0 | @0x80108094 / @0x80108174 / @0x8010820c | | | Schrot-Zerfetzen beim Tod (Port: zombiegore_r15.c:279) |
  | 0x80109e4c | @0x8010a01c | Teil 0 | Untermodus 0xf @0x8011f8cc; setzt `+0x93 |= 1` @0x80109ef0-f8 | Treffer-Untermodus |
  | 0x8010cb34 | @0x8010cc18 / @0x8010cc68 | 2 und 3, Skala 0x1000 = keine Verformung | Tabelle @0x80120990 | andere Kreaturen-Familie |

  Keiner dieser Setzer liegt im INIT (Zustand 0 @0x8011f7b4[0] = 0x80100688).
- **STAGE2-5:** Dieselben Muster an verschobenen Adressen. Auch dort kommt Bit 0x400 nie ohne Skalen-Store.
- **Setzer von 0x2000 in STAGE1:** Das sind Globals (@0x80113b50, `sh -13744(at)`) und Effekt-Argumente (`lui 0x0303/0x0500`).
  Keiner schreibt ein Teil-Wort.

### 1e Zufall: Disc und Mod sind VERSCHIEDEN (Dossier 2a gilt nur fuer den Mod)
- **Disc**, @0x8001af28: `00 00 44 94` = `lhu a0,0(v0)`. Der Generator liest seinen Zustand @0x800ac774, also echter Zustands-Zufall.
  - Zustand je Raum = 0x1c3: `ori v0,zero,0x1c3` @0x8003162c (`c3 01 02 34`), `sw` @0x80031634 (`74 c7 22 ac`).
  - Dieser Store ist auf der Disc nicht tot.
- **Mod**, @0x8001af28: `00 00 49 94` = `lhu t1,0(v0)`. Das ist der tote Read aus der Memory `reai-v2-rng-determinism`;
  der Generator hasht dann a0.
- Belegt am Disc-Savestate (1f): Auf der Disc wird tatsaechlich die **Arm**-Routine 0x801010a8 gewuerfelt.
  - Sie setzt Teil 8: `ori v1,v1,0x400` @0x801010c8, `sw v1,1376(v0)` @0x801010cc.
  - Sie setzt Teil 9: @0x80101110 / @0x80101114.
  - Skala 0x5f4/0x1000/0x5f4, Tinte 0x80/0x8080e0.

  Auch dieser Arm wird vom Reset aus 1c geloescht.

### 1f Savestates (selbst gelesen, `ssram.py` / `ssparts3.py` / `sweep.py`)
Die RAM-Basis wird ueber die Signatur des Zeichners gesucht; sie ist in Mod und Disc gleich. EXE-Kennung ist das Wort @0x8001af28.
Sauberkeit: @0x80026e4c = `03e00008` in allen Saves (keine Saves mit PATCHED-EXE).

| Savestate | EXE | Slot / Typ | Teil @Adresse | Teil-Wort | Skala x/y/z | Tinte +0x94/+0x98 |
|---|---|---|---|---|---|---|
| `stage_saves/mzd_stage1_briefing.sav` (1140) | Mod | 3 / 0x11 (dick) | T4 @0x8016dd5c | 0x00000001 | 0x7e8/0x1000/0x7e8 | 0x80/0x8080e0 |
| dto. | | 3 / 0x11 | T5 @0x8016de08 | 0x00000001 | 0x5f4/0x1000/0x5f4 | 0x80/0x8080e0 |
| `stage_saves/mzd_stage1_engage_live.sav` (1140, Cut 1) | Mod | 3 / 0x11 | T4 @0x8016dd5c, T5 @0x8016de08 | 0x00000001 | 0x7e8 / 0x5f4 | 0x80/0x8080e0 |
| `stage_saves/mzd_stage1_combat_death.sav` (1250) | Mod | 0 / 0x10 | T4 @0x80148f3c, T5 @0x80148fe8 | 0x00000001 | 0x7e8 / 0x5f4 | 0x80/0x8080e0 |
| dto. | | 1 / 0x10 | T4 @0x8014fae4, T5 @0x8014fb90 | 0x00000001 | 0x7e8 / 0x5f4 | 0x80/0x8080e0 |
| `stage_saves/room1140_entry.sav` | Mod | 0 / 0x16 | T4 @0x801493f4, T5 @0x801494a0 | 0x00000001 | 0x7e8 / 0x5f4 | 0x80/0x8080e0 |
| `stage_saves/mzd_demo/mzd_stage2_room.sav` | Mod | 2 / 0x16 | T4 @0x80157df8 | 0x00000001 | 0x7e8/0x1000/0x7e8 | 0x80/0x8080e0 |
| **`abnahme1/disc40_room11e0_HASH-DD767256674230B3_1.sav`** (Version_40%-EXE, STAGE1, Raum 0x1E) | **Disc** | 1 / 0x10 | **T8 @0x80152540, T9 @0x801525ec (Arm)** | **0x00000001** | 0x5f4/0x1000/0x5f4 | 0x80/0x8080e0 |

Durchlauf ueber alle Saves (`savestate_sweep.txt`):
- 27 Saves mit aktiven Zombies (26 Mod, 1 Disc), 131 Zombie-Eintraege.
- 99 Eintraege tragen einen Varianten-Rest (98 Mod wie im Dossier, 1 Disc). Bei **allen 99** steht jedes der 15 Teil-Worte auf 1.
- Drei Eintraege ohne Rest haben ueberschriebene Teil-Felder: `lamp_1170_stage1.sav` Slot 0/1 und `mzd_title_after_death.sav`
  Slot 0, mit Worten wie 0xfdfffeff. Das sind veraltete Entity-Reste, kein Zombie im Bild.

Der Framebuffer des Disc-Saves (`disc40_room11e0_fb.png`) zeigt die Zombies liegend, klein und dunkel am Gangende.
Fuer eine Aussage zum Arm taugt er nicht; der Beleg dort ist RAM plus Code.

## Pruefpunkt 2: Zufallsstrom unveraendert, alle Typen getroffen: BESTANDEN

- **Diff 1100251c8~1..1100251c8** (enemy_ai_common.c):
  - vorher: `if ((rand8()&3)==0) { ... int v = rand8()&7; ... re15_teile_markieren(); }`
  - nachher: `if ((rand8()&3)==0) (void)rand8();`, die Anwendung steht in `#if 0`.

  Die Wuerfe sind in Zahl und Reihenfolge gleich. Die Teil-Schleife und `re15_teile_markieren` ziehen keinen Zufall.
  Danach folgt unveraendert `re15_brand_init`.
- `unit_za_spawnvariante` (selbst ausgefuehrt), je Typ 0x10/0x11/0x12/0x16/0x18:
  ```
  400 Seeds, Varianten-Wurf 0 in 99, Teilschicht an 0, Teil veraendert 0, Strom-Abweichung 0
  ```
  Die Gegenprobe gegen master (PIN 1 rot, PIN 2 gruen) hat die Spur gemacht; ich habe sie nicht wiederholt.
  Die Gleichheit des Stroms folgt hier direkt aus dem Diff.
- **Alle Typen:** Die Abschaltung sitzt ohne Bedingung in `re15_enemy_ai_live_init`. Beide Gehirne rufen die Funktion:
  - RE1.5-Gehirn: enemy_ai_common.c:5918 (`RE15_AI_STATE_INIT`)
  - RE2-Gehirn: enemy_ai_re2_zombie.c:8586 (`re2z_init`). Dort laeuft `re15_teile_umordnen` nur bei `re15_teile_an`,
    bleibt also aus.

  Im Original laufen alle diese Typen durch denselben INIT. Belegt durch die Registrierung STAGE1
  (`sw v0` mit v0 = 0x80100424):

  | Store | Ziel | Typ |
  |---|---|---|
  | @0x8011e88c | 0x80072bec | 0x10 |
  | @0x8011e894 | 0x80072bf0 | 0x11, dicker Zombie EM11 |
  | @0x8011e89c | 0x80072bf4 | 0x12 |
  | @0x8011e8a4 | 0x80072c04 | 0x16 |
  | @0x8011e8ac | 0x80072c0c | 0x18 |
  | @0x8011e8b4-cc | 0x80072c1c-c28 | 0x1c-0x1f |

  Typ 0x13 geht nach 0x8010a8c8 (@0x8011e87c).
- Weitere Aufrufe von `re15_teile_markieren` im Port betreffen nur Treffer und Tod:
  - zombiegore_r15.c:66
  - enemy_ai_re2_zombie.c:7726 (80%-Zerfetzen)
  - Spinnen-Platzen @11104
  - Hand-ab beim Tod @13723
  - birkin_opfer.c:325

  Beim Spawn setzt der Port nichts mehr.

## Pruefpunkt 3: Sichtbeleg im echten Fenster: BESTANDEN, mit Einschraenkung

**Build und Aufnahme**
- `local_build.sh build`: `ninja: no work to do`.
  - `re15_pc.exe` stammt vom 2026-11-02 02:09; `enemy_ai_common.c.obj` (02:08:59) ist neuer als die Quelle (02:08:56).
  - Die Messzeile `ZA-MESS` ist nicht im Binary.
- Aufnahme mit `ffmpeg 8.1.1 -f gdigrab -i hwnd=<HWND>`. Das HWND gehoert dem Fenster der eigenen PID (Kopie `re15_pc_zaabn.exe`, `hwnd.py`).
  - Das umgeht die Titel-Verwechslung mit parallelen Agenten-Fenstern.
  - Ergebnis 960x720, ohne AUTOSHOT und ohne SOFTWARE_RENDER.
  - Rezept: `cap.sh`. Beendet wurden nur eigene PIDs.

**Weg**
- Umgebung: `RE15_NO_INTRO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_DEBUG_JUMP=1140@240` (Sprung ueber das Debug-Menue).
- `RE15_AI_MODELS=re15` = Hybrid mit RE1.5-Geometrie, wie in der Sitzung des Nutzers.
- `RE15_FORCE_CUT=1` heftet die Kamera auf Cut 1. Das ist dieselbe Kamera wie im Original-Save (`0x800b0fe4` = 1).
- Eingabe `R0.2,U5,W30`: Leon laeuft zu den Fressern, sie stehen auf.

**Bilder**

| Datei | Inhalt |
|---|---|
| `vergleich_1140_cut1_original_port.png` | links Original (`mzd_stage1_engage_live.sav`, Framebuffer), Mitte Port Bild 3, rechts Port Bild 5 |
| `port_1140_cut1_bild3_em10.png` | stehende EM10/EM16, Uniformaermel durchgehend |
| `port_1140_cut1_bild5_em11.png` | beide dicken EM11 (weisses Kurzarmhemd, nackte dicke Unterarme), beide Arme voll |
| `port_1140_em11_vier_laeufe.png` | EM11-Ausschnitt aus vier Laeufen: fester Seed Hybrid, zwei Laeufe mit `RE15_RNG_ENTROPY=1` Hybrid, fester Seed mit RE2-Modellen. In keinem ein duenner Arm oder ein duennes Bein |
| `port_1250_bild5_em10_em11.png` | zweiter Raum ROOM1250 (Typen 0x10 und 0x11), Gruppe EM10/EM11, Glieder normal |
| `../original_1250_todeskamera_slot0_1_fb.png` | Original-Gegenbild ROOM1250 aus `mzd_stage1_combat_death.sav`, Bild der Spur: beide EM10 tragen den Bein-Rest im RAM, beide Beine gleich dick |

**Einschraenkung:** Ohne Logzeile laesst sich im Bild nicht zeigen, welcher Zombie in diesem Lauf den Varianten-Wurf bekam.
Dass jeder Wurf folgenlos bleibt, belegt PIN 1 (je Typ 99 Varianten-Wuerfe, kein Teil veraendert). Das Vorher-Bild der
Spur (`../vergleich_1140_dicke_zombies_vorher_nachher.png`) zeigt die duennen Arme im alten Build.

## Pruefpunkt 4: Tests: BESTANDEN
- `unit_za_spawnvariante`: Passed (Ausgabe siehe Punkt 2).
- Gruppe `ctest -R "za_spawn|zombie|re2z|gore|1030_stuck_who|enemy|em11|brand|1140" -j 6 --timeout 120`, PowerShell mit
  `C:\msys64\mingw64\bin` vorn und `SDL_ASSERT=always_ignore`: **113/113 Passed**, 186,5 s. Parallel liefen andere Agenten
  (neun fremde re15_pc-Prozesse); es gab keinen Ausfall und deshalb keine Einzelwiederholung.

## Maengel

1. **M1 (Doku, mittel): Die eigentliche Loeschstelle fehlt, und ein falscher Kommentar steht noch im Code.**
   - Dossier 2b: "Wer die Bits ... loescht, ist offen". Es ist FUN_8001ee68 @0x80100fe0 (1c).
   - enemy_ai_common.c:2184 sagt `FUN_8001ee68 @0x80100fe0: nur in diesem Zweig — die Variante verfaellt`. Das ist falsch:
     `bne v0,v1,0x80100fd8` @0x80100f70 schickt alle anderen Zombies zum selben Aufruf. Der Kommentar stammt aus 53981ba5d,
     und genau dieser Irrtum hat die Variante in den Port gebracht.
   - Der ZA-Kommentar (um Zeile 2135) nennt als Reset FUN_8001e5b0 in Sce_em_set. Dieser Reset laeuft aber VOR der Variante;
     der massgebliche Reset ist FUN_8001ee68 nach ihr.
   - Empfehlung: nur die Kommentare und das Dossier berichtigen, kein Verhaltens-Eingriff.
2. **M2 (Doku, mittel): Zufall gilt nur fuer den Mod.**
   - Dossier 2a, der ZA-Code-Kommentar ("Slots 0..3 waehlen so immer 0x8010118c ... ein ARM kaeme im Original nie dran") und
     `ZA_belege/rng_hash_je_slot.txt` beruhen auf der Mod-EXE (`lhu t1` @0x8001af28).
   - Version_40% hat `lhu a0` (Zustands-Zufall, Startwert 0x1c3 je Raum), und der Disc-Save zeigt einen gewuerfelten Arm.
     Am Ergebnis aendert das nichts, weil der Reset jeden Wurf trifft.
   - **Folgt fuer andere Spuren:** Die Memory `reai-v2-rng-determinism` ("Original hat GAR KEINE Entropie", 0x1c3 = toter
     Store) beschreibt nur den Mod. Auf der 40%-Disc ist der Store bei @0x80031634 lebendig. Das sollte der Koordinator
     einordnen.
3. **M3 (Beleglage, gering):** Fuer die Bilder siehe die Einschraenkung bei Punkt 3. Der Beweis kommt aus RE und Riegel, das Bild ist die Plausibilitaetspruefung.
4. **M4 (Nebenbefund, nicht von dieser Spur):**
   - Die Reihenfolge der Wuerfe in `re15_enemy_ai_live_init` ist Poise, Stagger, HP, Variante.
   - Das Original hat Stagger (@0x80100774), HP (@0x801007bc), Poise (@0x80100824), Variante (@0x801008b0).
   - Ohne Einfluss aufs Aussehen; die Werte des Port-Generators sind ohnehin eine dokumentierte Substitution.
5. Unversioniert im Baum liegt `ZA_belege/vorher_nachher_1140.png` (von der Spur). Diese Abnahme hat es nicht committet.

## Belege (`ZA_belege/abnahme1/`)

| Datei | Inhalt |
|---|---|
| `disasm_belege.txt` | Zeichner, rng Disc/Mod mit Bytes, 0x1c3, Variante, INIT-Schwanz, FUN_8001ee68, Sce_em_set, Arm-Routine 0x801010a8 |
| `stage1-5_init_schwanz.txt` | Variante und Reset je Stage, Sprung-Pruefung, Disc und Mod |
| `ori400_alle_binaries.txt`, `ori400_teilwort_und_skala.txt` | alle Setzer von 0x400/0x2000 mit Teil-Wort- und Skalen-Store |
| `exe_diff_mod_vs_disc.txt` | 81 Diff-Gruppen PSX.EXE Mod gegen Version_40% |
| `savestate_sweep.txt` | 27 Saves, 131 Eintraege, 99 Reste, alle mit Wort 1 |
| `disc40_room11e0_HASH-DD767256674230B3_1.sav` | Kopie aus `%LOCALAPPDATA%/DuckStation/savestates`, Version_40%-EXE |
| `disc40_room11e0_fb.png` | Framebuffer des Disc-Saves |
| `original_1140_cut1_engage_live_fb.png`, Port-Bilder (Punkt 3) | Sichtvergleich |
| `dis2.py`, `xref.py`, `exediff.py`, `stagecheck.py`, `skipcheck.py`, `ori400.py`, `ori400ctx.py`, `ssram.py`, `ssparts3.py`, `sweep.py`, `fbdump.py`, `cap.sh`, `hwnd.py` | Werkzeuge |
