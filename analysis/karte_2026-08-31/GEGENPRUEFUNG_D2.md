# GEGENPRUEFUNG D2 (Rolle: FOLGEN-PRUEFER) — "s_tfade_sub hat ausserhalb der Titel-Schleife keinen Release"

Datum 2026-08-31. Geprueft wurde NICHT der Beleg, sondern die FOLGEN: was geht kaputt, wenn
nach D2 + der abgeleiteten Empfehlung (§7 des Berichts `D_dark_start.md`) gehandelt wird.

**Ergebnis: kein Schaden gefunden — die Behauptung haelt.** ABER die Empfehlung hat einen
gemessenen Konstruktionsfehler: **Teil (1) allein ist auf dem Verifikations-Pfad ein NO-OP**,
und genau diesen Pfad schreibt §7 als Verifikation vor. Details unten.

---

## 0. Werkzeug (Mess-Sonden, keine Produktivdatei angefasst)

Scratchpad `.../scratchpad/D2/`:

* `patch.py` / `patch3.py` — legen PATCHED KOPIEN von `platform/pc/main.c` an (`main_fix1.c`,
  `main_fix2.c`, `main_fix12.c`, `main_fix3.c`). Das Repo-`main.c` blieb unveraendert.
* Uebersetzt mit den EXAKTEN Flags aus `re15_port/build/build.ninja:730-736` und gelinkt gegen
  die vorhandenen Objekte + `libre15_engine.a` + `libSDL2.a` (`build.ninja:837-839`)
  -> `re15_pc_fix1.exe`, `re15_pc_fix2.exe`, `re15_pc_fix12.exe`, `re15_pc_fix3.exe`
  (+ `re15_pc_base.exe` = Kopie des Dev-Builds).
* `bmp.py` — max/mean-Luminanz eines BMP.

### Deterministischer Ersatz fuer die gdigrab-Messung (empfohlen fuer §7)

    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=<af>
    RE15_INV_SHOT=inv.bmp RE15_AUTOSHOT_SMALL=1 ./re15_pc.exe

`RE15_INV_SHOT` (main.c:3749-3751 + main.c:8074-8099) oeffnet den STATUS-Schirm bei Spiel-Frame
30, schiesst bei Frame 34 und beendet mit `exit(0)`. **Gleicher Spiel-Frame, gleicher Inhalt,
keine Wanduhr, kein gdigrab** — die einzige Differenz zwischen zwei Laeufen ist der Leck-Wert.
Das ist stabiler als "max == 214 bei t=20 s" (die 214 ist Szenen-Inhalt zu einem Wanduhr-
Zeitpunkt; die Tabellen 3a/3b/3d des Berichts enthalten selbst 0/15/119/152/207/214/237/255).

---

## 1. Fakt D2 — unabhaengig bestaetigt

Schreiber von `s_tfade_sub`/`s_tfade_add` im GANZEN Repo (`grep -rn` ueber `re15_port/`, inkl.
`engine/`, `include/`, `platform/psx/`): **nur** `platform/pc/main.c:2602,2603,2611,2613,2618,
2638,2658,2670` (alle innerhalb der Titel-Schleife main.c:2540-2677) + die Setter
`render_pc.c:1301/1302`. `platform/psx/` und `engine/` haben **0 Treffer** — die Ebene existiert
ausschliesslich im PC-Renderer. Kein Release ausserhalb der Schleife. **D2 stimmt.**

Eigene Messung (Baseline = unveraenderter Dev-Build `re15_port/build/platform/pc/re15_pc.exe`):

| AF (= tblink beim Ausstieg) | B = 255-(AF>>1)*8 | Spiel-Frame 34, max | mean |
|---|---|---|---|
| 4  | 239 | **5**   | 0.01 |
| 22 (**Default**, main.c:2674) | 167 | **71** | 2.80 |
| 80 | 0   | **238** | 61.58 |

Subtraktiv exakt: 238 - 167 = **71** (getroffen), 238 - 239 -> Klemmung 0 (gemessen 5).
Damit ist der Mechanismus (ABR2-Quad `render_pc.c:1197-1205`) unabhaengig zweitbestaetigt.

Echter Nutzer-Pfad (`RE15_INPUT_SCRIPT="S0.2,W11,B6"`, `RE15_INPUT_SCRIPT_START=6`,
also Enter auf Titel-Frame 6): Baseline max **9** / mean 0.01 = schwarz.

---

## 2. DER BEFUND: Empfehlung (1) ist auf dem Verifikations-Pfad ein NO-OP

`main.c:2668-2670` schreibt den Fade-in. Der Auto-Advance schaltet den Modus ERST DANACH um:

    2670   re15_render_pc_title_fade_sub(B);      <-- Schreiben
    2671   re15_render_end_frame();
    2673   re15_render_pc_hide_title_menu();
    2674-5 if (t_shot && tblink == t_af) { screenshot; re15_gameflow_new_game(0); }   <-- Modus INGAME

Zum Zeitpunkt des Schreibens ist `re15_gameflow_mode()` also **noch TITLE** — das vorgeschlagene
Gate `if (re15_gameflow_mode() == RE15_MODE_TITLE) { ... }` ist auf diesem Pfad immer wahr.

Gemessen (Sonden-Exen, Spiel-Frame 34, max/mean):

| Variante | AF=4 | AF=22 | AF=80 | echter Nutzer-Pfad (START=6) |
|---|---|---|---|---|
| **base** (heute) | 5 / 0.01 | 71 / 2.80 | 238 / 61.58 | 9 / 0.01 |
| **fix1** = Empfehlung (1) allein | **5 / 0.01** | **71 / 2.80** | 238 / 61.58 | **238 / 61.58** |
| **fix2** = Empfehlung (2) allein | 238 / 61.58 | 238 / 61.58 | 238 / 61.58 | — |
| **fix12** = (1)+(2) | 238 / 61.58 | 238 / 61.58 | 238 / 61.58 | — |
| **fix3** = 1 Zeile nach der Schleife (s.u.) | 238 / 61.58 | 238 / 61.58 | 238 / 61.58 | **238 / 61.58** |

* Empfehlung (1) repariert den **echten** Nutzer-Pfad (9 -> 238) — dort faellt der Modus im
  `if (confirm)`-Block VOR Zeile 2670.
* Empfehlung (1) repariert den **`RE15_TITLE_SHOT`-Pfad NICHT**: byte-identische Zahlen.
  Der Bericht selbst listet diesen Pfad in §4 als betroffen (Default AF=22 => B=167).
* **Und §7 schreibt genau diesen Pfad als Verifikation vor** ("Tabelle 3a erneut fahren").
  Wer (1) anwendet und dann 3a misst, sieht NULL Aenderung und haelt die richtige Diagnose
  fuer widerlegt — der naechste Verdaechtige waere die Fade-in-Rate @0x80102054-64,
  die nachweislich unschuldig ist (siehe §4.1).

### Folge fuer die Test-Schiene

Alle drei ctest-Integrations-Pins fahren `RE15_TITLE_SHOT` mit dem Default **AF=22** => B=167:
`tests/integration/test_boot_bg_pin.cmake`, `test_relatch_pin.cmake`, `test_save_counter_pin.cmake`.
Nach Empfehlung (1) allein laufen sie weiter mit einem auf max 71 abgedunkelten Spiel
(`test_boot_bg_pin` schreibt in diesem Zustand sogar `exit34.bmp`). Jeder kuenftige Pixel-Pin
auf diesem Harness waere vergiftet.

---

## 3. Bessere Stelle (gemessen): EINE unbedingte Zeile nach der Titel-Schleife

`main.c:2678` (`re15_render_pc_hide_title();`) laeuft **unbedingt und genau einmal** auf JEDEM
Ausstieg der Titel-Schleife (NEW GAME, LOAD-Erfolg, `RE15_TITLE_SHOT`-Auto-Advance, jeder
kuenftige). Dort geklemmt (`fix3`):

    re15_render_pc_hide_title();
    { re15_render_pc_title_fade_sub(0); re15_render_pc_title_fade_add(0); }

Ergebnis: 238/238/238 + echter Pfad 238 (Tabelle oben). Das ist zugleich die **byte-true
Semantik von `FUN_800161e0`**: der Kanal-0-Kill `jal FUN_80021764` @0x80016420 (`a0=0` im
Delay-Slot @0x80016424) ist ein **EINMALIGER Kill beim Game-Init**, kein Pro-Frame-Nullen.

Zu Empfehlung (2) — funktioniert, sitzt aber schlechter: `main.c:4672-4675` liegt in
`if (md1_ok)` (`main.c:4214`; `md1_ok` = Spieler-MD1 geparst, `main.c:2878`) innerhalb
`while (running)` (`main.c:3639`). Das ist ein pro-Frame-Reclear, gegated auf das Spieler-Modell
— aus dem "unbedingten Kill" wird ein bedingtes. (Praktisch harmlos: ohne `md1_ok` rendert das
Gameplay ohnehin nicht. Aber es ist nicht das Gegenstueck zu @0x80016420.)

---

## 4. Aktiv gesuchte Gegenbeispiele — alle NEGATIV

1. **Titel-Fade-in selbst** (@0x80102054-64, 32 Ticks a 2 Vsyncs): Titel-Screenshots
   `RE15_TITLE_SHOT_AF=40` und `=64` sind fuer base/fix1/fix2/fix3 **md5-identisch**
   (`00580090612d4b4b7f11f71711d09b67` bzw. `2271b2730c76b73e136449fca094cca3`;
   max/mean 160/4.20 bzw. 255/20.55). Keine der Varianten fasst die Fade-in-Kurve an.
2. **OPTION-Rueckkehr** (`main.c:2653-2661`) und **LOAD-GAME-Abbruch** (`main.c:2650-2652`):
   setzen `tblink = 0` und bleiben in der Schleife => Gate von (1) ist wahr, fix3 klemmt erst
   nach der Schleife => der Wieder-Fade-in bleibt in allen Varianten erhalten.
   (`pc_run_config` und `pc_run_player_select` nutzen die ANDERE Ebene `s_title_fade` und
   nullen sie selbst: `main.c:1842` / `main.c:2320`.)
3. **Tod -> Titel -> 2. Boot** (`goto re_title`, `main.c:8279` / Label `main.c:2466`):
   `test_relatch_pin` faehrt genau diesen Zyklus — mit `fix3` **gruen**
   ("2 W-Bank-Bindungen ueber 2 Boots, Select-TIM-Re-Upload pro Entry").
4. **LOAD-GAME-Ausstieg**: `test_save_counter_pin` (3 exe-Laeufe, `RE15_CONTINUE_TEST`)
   mit `fix3` **gruen** (Slots 0..4 = 0/1/2/3/4).
5. **Boot-BG-Fenster**: `test_boot_bg_pin` mit `fix3` **gruen**
   ("F0 blit room1240#00", kein room1170, kein TEST.BSS-Fallback).
6. **Andere Konsumenten der Ebene**: keine. `grep -rn "tfade|title_fade"` ueber
   `re15_port/engine/`, `re15_port/include/`, `re15_port/platform/psx/` = 0 Treffer.
   Die Raum-Fades laufen ueber `g_fade_ch` / `re15_fade_tick` (`render_pc.c:878-900`,
   `engine/src/fade_common.c`) — eine voellig andere Ebene, von keiner Variante beruehrt.
7. **Zeichen-Reihenfolge** (`render_pc.c`): `s_fade_alpha` @975 -> Titel-Art/Menue ->
   `s_title_fade` @1174 -> `s_tfade_add` @1190 -> `s_tfade_sub` @1197 -> Auswahl-Cursor @1207 ->
   YOU-DIED-Grafik @1225. Das Leck dunkelt also ALLES ausser dem Cursor und der YOU-DIED-Grafik
   — diese zwei Elemente aendern sich durch den Fix nicht (kein Regressionsrisiko, aber ein
   nuetzliches Erkennungsmerkmal: "alles schwarz, YOU DIED trotzdem hell" = dieses Leck).

**Kein Unit-Test faellt**: `re15_port/tests/unit/*.c` linkt weder `main.c` noch `render_pc.c`;
die Ebene existiert dort nicht.

---

## 5. Empfehlung an den Auftraggeber

* Fakt D2 uebernehmen (bestaetigt).
* **Nicht** nur Empfehlung (1) anwenden — sie laesst den `RE15_TITLE_SHOT`-Pfad (und damit die
  komplette ctest-Schiene, Default AF=22 => B=167) leck.
* Statt (1)+(2): **eine** unbedingte Klemmung direkt nach der Titel-Schleife (`main.c:2678`),
  byte-true analog zum EINMALIGEN Kanal-0-Kill @0x80016420. (1) und (2) zusaetzlich schaden
  nicht, sind dann aber redundant.
* Verifikation NICHT ueber "max == 214 bei t=20 s" (Wanduhr + Szenen-Inhalt), sondern ueber den
  deterministischen `RE15_INV_SHOT`-Frame-34-Schuss aus §0: **AF=4 / 22 / 80 muessen alle
  max = 238, mean = 61.58 liefern** (heute 5/0.01, 71/2.80, 238/61.58).
