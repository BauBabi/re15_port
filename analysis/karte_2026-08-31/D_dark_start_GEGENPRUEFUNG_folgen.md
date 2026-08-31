# GEGENPRUEFUNG (Rolle: Folgen-Pruefer) zu D4 — "Restwert B = 255 - (tblink>>1)*8"

Datum 2026-08-31. Geprueft wurde NICHT der Beleg von D4 noch einmal, sondern die FOLGEN
des Handelns nach der Empfehlung (§7 in `analysis/karte_2026-08-31/D_dark_start.md`).

Gemessen auf `re15_port/build/platform/pc/re15_pc.exe` (Stand 2026-08-31 20:14).
Alle Messungen mit `RE15_FRAMEDUMP` (Ruecklesung INNERHALB `re15_render_end_frame`
unmittelbar VOR `SDL_RenderPresent`, `render_pc.c:1257-1294`) — also ein ANDERER Harness
als die Original-Untersuchung (gdigrab) und nicht der unzuverlaessige Post-Present-Pfad
von `RE15_AUTOSHOT`. Messdaten/Skripte: `<scratchpad>/Dchk/`.

## ERGEBNIS: refuted = FALSE

D4 ist mit einem unabhaengigen Harness byte-exakt reproduziert; ein Raum/Bildschirm/Test,
in dem die Empfehlung nachweislich das Falsche tut, existiert nach drei gezielten
Gegenversuchen nicht. ABER: die Empfehlung enthaelt eine Falle (§B) und eine ungenaue
Voraussetzung (§C).

---

## A. Unabhaengige Bestaetigung — 6.220.800 von 6.220.800 Kanaelen exakt

`RE15_TITLE_SHOT` ohne `_AF` => Default `t_af = 22` (main.c:2674) => Restwert
B = 255 - (22>>1)*8 = **167**. `RE15_TITLE_SHOT_AF=64` => tk=32 => B = 0 (== der Zustand
NACH dem Fix). Beide Laeufe je Raum, gleicher Frame, Pixelvergleich gegen die Vorhersage
`AF22 == max(0, AF64 - 167)` (subtraktiver ABR2-Vollbildquad, `render_pc.c:1197-1205`):

| Szene | Frame | Kanaele exakt | Abweichler | AF22 max/mean | AF64 max/mean |
|---|---|---|---|---|---|
| ROOM1240 Boot (Inventar erzwungen) | F34  | 2.073.600/2.073.600 | **0** | 71 / 2.84 | 238 / 62.12 |
| ROOM1140 (`RE15_DEBUG_JUMP=1140@40`) | F200 | 2.073.600/2.073.600 | **0** | 88 / 0.17 | 255 / 46.15 |
| ROOM1170 (`RE15_DEBUG_JUMP=1170@40`) | F200 | 2.073.600/2.073.600 | **0** | 47 / 0.13 | 214 / 2.01 |

Null Abweichler bei 6,22 Mio. Kanaelen ueber drei Raeume: die Wirkung ist eine EXAKT
uniforme, raum-unabhaengige Konstante. Damit ist zugleich beantwortet, ob es einen Raum
gibt, in dem die Empfehlung etwas anderes tut: **es kann keinen geben** — der Quad ist
raumfrei, kennt weder Zone noch Rechteck noch Kamera noch Tabelle.

Zusaetzlich (billiger A/B, bestaetigt Tabelle 3a der Untersuchung):
`AF=4` => Spielframe 34 max **5** / mean 0.01 (schwarz); `AF=64` => max **238** / mean 61.58.

## B. FALLE: Schritt (1) repariert den Weg NICHT, mit dem verifiziert werden soll

Reihenfolge im Schleifenrumpf (`re15_port/platform/pc/main.c`, woertlich):

    2668: { extern void re15_render_pc_title_fade_sub(int b);
    2669:   int tk = (int)(tblink >> 1); int B = 255 - tk * 8; if (B < 0) B = 0;
    2670:   re15_render_pc_title_fade_sub(B); }
    2671: re15_render_end_frame();
    2672: re15_audio_tick();
    2673: re15_render_pc_hide_title_menu();
    2674: { unsigned t_af = 22; const char *afe = getenv("RE15_TITLE_SHOT_AF"); ...
    2675:   if (t_shot && tblink == t_af) { re15_render_pc_screenshot(t_shot); re15_gameflow_new_game(0); } }

Der Auto-Advance ruft `re15_gameflow_new_game(0)` in **Zeile 2675**, also FUENF Zeilen
NACH dem Schreibzugriff in 2670. Zum Zeitpunkt von 2668 ist `g_gameflow.mode` in diesem
Pfad unveraendert `RE15_MODE_TITLE` (einzige Mode-Writer in main.c: 2647, 3861, 5375 —
2647 liegt im `if (confirm)`-Block, der im Harness-Pfad nie feuert; im Messlauf fehlt
folgerichtig jede `[flow] title confirm`-Zeile).
=> Die vorgeschlagene Klammer `if (re15_gameflow_mode() == RE15_MODE_TITLE) { ... }` ist
in diesem Pfad **immer wahr**, der Schreibzugriff bleibt bestehen.

Konsequenz: Die in §7 vorgeschriebene Verifikation ("Tabelle 3a erneut fahren — ALLE
AF-Werte muessen max == 214 liefern") liefert nach Schritt (1) **exakt die alten Zahlen**
0/119/207/207/214/214. Wer nur (1) umsetzt und dann 3a faehrt, liest "Fix wirkungslos"
und verwirft eine korrekte Aenderung.

**Schritt (2) ist damit nicht "zusaetzlich", sondern der einzige, der den in §4 der
Untersuchung selbst gelisteten dritten Weg (`RE15_TITLE_SHOT`-Auto-Advance, B=167)
schliesst.** Umsetzungsreihenfolge: (2) ist Pflicht, (1) ist die saubere Ergaenzung.
Wer nur den Nutzer-Pfad von Hand nachspielt, wuerde umgekehrt (1) fuer ausreichend halten
und den Harness-Pfad — den drei Integrationstests und praktisch jeder Screenshot im
Projekt benutzen — dunkel lassen.

## C. Ungenau: Schritt (2) sitzt NICHT unbedingt in jedem Frame

Brace-Analyse von main.c bis Zeile 4672 ergibt die offenen Bloecke
2325 -> `while (running) {` @3639 -> **`if (md1_ok) {` @4214** -> 4658 -> 4672.

`md1_ok` (main.c:2878) = "Spieler-MD1 (PL00) wurde geparst" (einmal vor der Spielschleife
ausgewertet). Der Frontend-Release-Block laeuft also nur, solange das Spielermodell
geladen ist. Die Aussage der Untersuchung "`s_fade_alpha` wird in der Spielschleife
**jeden Frame** genullt (main.c:4672-4675)" stimmt so nicht — sie gilt nur unter
`md1_ok`. Genau der Fall "Assets/Modell nicht gefunden" ist im Projekt schon aufgetreten
(Paket-Asset-Root v0.3.20). Der Original-Kanal-0-Kill @0x80016420 ist UNBEDINGT; wer ihn
byte-true nachbildet, zieht den Reset VOR das `if (md1_ok)`, nicht hinein.

## D. Welcher Test/Pin faellt? — KEINER (geprueft)

- Nur drei Tests fahren die exe ueber den Titel: `test_boot_bg_pin.cmake`,
  `test_relatch_pin.cmake`, `test_save_counter_pin.cmake` — alle drei mit
  `RE15_TITLE_SHOT` **ohne** `_AF`, laufen also heute mit B=167 im Bild.
  Alle drei pruefen ausschliesslich `debug.log` per Regex (bg-log-Zeilen, Latch-Zeilen,
  Save-Zaehler) — **kein Pixelwert, kein Referenzbild**. Gegen die Helligkeitsaenderung
  immun.
- `re15_port/tests/` enthaelt **keine** Referenzbilder (kein .bmp/.png/.ppm).
- **Kein** Unit-Test kompiliert `platform/pc/src/render_pc.c` (die pc-Quellen in den
  Test-Targets sind `skeleton_trig_pc.c`, `audio_pc.c`, `asset_root_pc.c`) — `s_tfade_sub`
  ist fuer die Unit-Suite unsichtbar.
- Vollstaendiger Xref bestaetigt: `title_fade_sub/add` haben ausserhalb main.c:2587-2670
  keinen Schreiber und ausser `re15_render_end_frame` keinen Leser. Blast-Radius eines
  Resets = Title-Fade-in + Confirm-Fade, sonst nichts.

## E. Was sich nach dem Fix messbar AENDERT (kein Schaden, aber wissen)

- Jeder automatisierte Screenshot, der ueber `RE15_TITLE_SHOT` (Default AF=22) am Titel
  vorbeifaehrt, wird byte-exakt um +167 heller. ROOM1140 F200: mean 0.17 -> 46.15.
- Bisher voellig unsichtbare Inhalte tauchen auf: der ROOM1240-Pre-Intro-Frame F0 hat
  AF22 max=**0**, AF64 max=**17** — die Montage-Stills (max <= 17) wurden vom Restwert
  komplett auf 0 geklemmt. **Jede frueher ueber diesen Harness getroffene Aussage der
  Form "Bild ist schwarz / Element fehlt / max=0" ist damit ungueltig** und muss neu
  gemessen werden, bevor sie als Befund weiterverwendet wird.

## F. Aktiv geprueft und WIDERLEGT (Schadens-Kandidaten, die keine sind)

1. **"Schritt (1) erzeugt einen 1-Frame-Hellblitz"** — Zeile 2671 praesentiert nach dem
   Fix einen Frame mit B=0. Geprueft: der Titel ist zu dem Zeitpunkt bereits versteckt
   (`re15_render_pc_hide_title()` in `pc_run_player_select` @main.c:1671 bzw. im
   Card-Screen @main.c:1155), und der letzte von diesen Screens praesentierte Frame ist
   per Konstruktion schwarz (Player-Select bricht erst bei `cz==5 && cz_black>=255` ab,
   main.c:1826; Card-Exit haelt 255, main.c:1376). Kein Blitz.
2. **"Schritt (1) killt den Titel-Fade-in in einem legitimen Pfad"** — die einzigen zwei
   Pfade mit `mode != TITLE` an Zeile 2668 sind NEW GAME (2634) und LOAD-Erfolg (2647);
   in beiden ist der Titel bereits weg und die Schleife bricht sowieso ab. LOAD-Abbruch
   (2653) und OPTION (2661) setzen `tblink = 0` und bleiben `mode == TITLE` — der
   Fade-in laeuft dort unveraendert. Todes-Pfad (5375 -> `goto re_title`) beginnt mit
   frischem `tblink = 0` (main.c:2482).
3. **"parity_net-Datensatz ist mit dem Restwert kontaminiert"** — geprueft:
   `.claude/skills/re15-parity-verify/parity_net/data/port/m_300.png` max 250 / mean 46.4
   (nicht abgedunkelt); die `pintro*`-Frames sind echt schwarz (Void-Kamera), nicht
   geklemmt. Keine Kontamination nachweisbar.

## G. Reproduktion (nachfahrbar, ~40 s je Lauf, kein gdigrab noetig)

    cd <scratchpad>/Dchk
    RE15_NO_INTRO=1 RE15_TITLE_SHOT=t.bmp                       RE15_FRAMEDUMP="34:g22.ppm" RE15_INV_SHOT=y.bmp <exe>
    RE15_NO_INTRO=1 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=64 RE15_FRAMEDUMP="34:g64.ppm" RE15_INV_SHOT=y.bmp <exe>
    # Vergleich je Kanal: g22 == max(0, g64 - 167)   -> 2073600/2073600

    # weitere Raeume (ohne INV_SHOT, mit timeout beenden):
    timeout 45 env RE15_NO_INTRO=1 RE15_TITLE_SHOT=t.bmp [RE15_TITLE_SHOT_AF=64] \
      RE15_DEBUG_JUMP="1140@40" RE15_FRAMEDUMP="200:r.ppm" <exe>
