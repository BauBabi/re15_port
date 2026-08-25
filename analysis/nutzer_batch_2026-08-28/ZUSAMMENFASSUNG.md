# Zusammenfuehrung Nutzer-Batch 2026-08-28

Drei Nutzer-Findings, je ein RE-Agent, je drei adversariale Pruefer. Wo Finder und
Pruefer sich widersprachen, habe ich **selbst disassembliert** und entschieden — nicht
nach Mehrheit. Solche Entscheidungen sind unten mit **[eigen]** markiert.

Vollstaendige Belege je Finding:
`1210-arme.md` · `1090-feuersound.md` · `ada-animation.md`

Werkzeuge: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`
(`--bin ../../../../info/Re1.5/PSX/BIN/STAGE1.BIN` fuer 0x8010xxxx),
`info/Re1.5/PSX.EXE`, `xxd`, eigener BGM/RDT-Parser, `re15_pc.exe` mit
`RE15_AUDIO_CAP_SYNC`.

**Kein Produktivcode von mir geaendert.** ⛔ Waehrend dieser Adjudikation hat ein
paralleler Agent alle drei Fixes committet (`f4010659`) — siehe den Abschnitt direkt
unter der Kurzfassung.

---

## Kurzfassung

| Finding | Ergebnis |
|---|---|
| **1090-feuersound** | ✅ **Erledigt, committet und von mir unabhaengig nachgemessen.** Der A/B-Lauf divergiert jetzt ab Spielbild 762, der Stille-Boden steigt von 0.69 auf 579, die 1,13 s Stille verschwindet, und mit geloeschtem Feuer reproduziert der Port exakt die alten Zahlen. Nur noch: paketieren. |
| **ada-animation** | ⚠ **Kern richtig, Grundgesamtheit falsch.** In ROOM1090 spawnt der Aktor wegen `grid_id = 0x40` in State 4 / Sub 6, nicht in der Eskorte **[eigen]** — die Sonde erzwingt State 1. Aber State 1 ist dort sehr wohl erreichbar: sub03 `Plc_ret` @Datei 0x26C4 auf Work-Slot = Gegner 0 **[eigen]**. Acht der neun Fix-Punkte sind implementierungsreif, einer ist falsch und drei Konstanten/Adressen gestrichen. |
| **1210-arme** | ❌ **Der Fix haette so nicht kommen duerfen.** Die zentrale Praemisse („8 von 10 Armen unerreichbar") ist durch einen gruenen ausgelieferten Pin widerlegt, die tragende Sichtbarkeits-Messung misst die falsche Groesse, und die Aenderung ist ausdruecklich nicht byte-true. Er ist trotzdem committet (f4010659). Dafuer ist der Mechanismus jetzt vollstaendig belegt (AOT-Kette, netto-Null von B[2], Einmaligkeit). **Zwingend nachzuholen: der visuelle Beleg.** |

---

## ⛔ WAEHREND DIESER ADJUDIKATION: alle drei Fixes wurden bereits committet

`f4010659  fix: Feuer-Sound 1090, ROOM1210-Arme kommen raus, Adas Eskorte-Animation`
(2026-08-25 14:37, 16 Dateien, +2472/−148) — ein paralleler Agent hat alle drei Findings
umgesetzt, waehrend ich sie geprueft habe. Mein Bericht ist damit teils eine
**Nachpruefung eines bereits gelieferten Stands**. Was das je Finding bedeutet:

| Finding | Committet | Verhaeltnis zu dieser Adjudikation |
|---|---|---|
| **1090** | `ss_mix`-Wache, Einschalter-Pin | ✅ **Gedeckt.** Ich habe den Fix unabhaengig nachgemessen (A/B divergiert ab Spielbild 762, max \|diff\| 6574 — dieselbe Zahl, die die Commit-Message nennt). |
| **ada** | Bank/Phasen-Tor/Doppel-Takt/Subs 0,1,2,3,5 | ⚠ **Ueberwiegend gedeckt** — `re15_npc_channel_anim` wurde korrekt NICHT angefasst, Sub 3 ist korrekt Halb-Tempo, der `+0x6=1`-Eintritt @0x8004f3d8 ist drin. **Zwei Punkte bleiben:** (1) die Erweiterung des globalen Advancer-Skips auf `0x45/0x49/0x4b/0x4d` ist **unbelegt** (nur 0x42 und 0x47 haben einen Beleg); (2) das grid-Bit **0x40 @0x8011ce24** kommt nirgends vor — in ROOM1090 spawnt der Aktor deshalb weiterhin nicht in der Eskorte, sondern in State 4 / Sub 6. |
| **1210** | Lunge +2420 + „Nachruestung" (wiederholte Ausloesung, Positions-Ruecksetzung) | ⚠ **Praemisse teilweise widerlegt.** Die Commit-Message fuehrt die Emitter-Kette (`0x800254a0`/`0x800256b0` nur im Ja-Zweig `FUN_8001e9ec`) korrekt — besser als der Ursprungs-Befund. Aber die tragende Messung („in NULL von NEUN Vierecken") misst weiterhin die **falsche Groesse** (der Cull testet nur das Viereck des **aktiven** Cuts), der Port-Kamera-Zustand von ROOM1210 ist unvalidiert, und die Behauptung „8 von 10 Armen unerreichbar" war schon vorher durch den gruenen Pin `test_1210_gitterhaende` widerlegt. Die Aenderung ist zudem **ausdruecklich nicht byte-true** (das Original loest die Lunge genau einmal aus, auf alle zehn gleichzeitig). |


**Test-Stand nachgeprueft [eigen]:** `ctest` auf `f4010659` = **238/238 gruen**
(82 s). ⚠ Ein erster Lauf meldete `integration_relatch_pin` als Fehlschlag
(`re15_pc.exe exit=1`); der Test besteht isoliert 3/3 und der ganze Suite-Lauf danach
238/238 — es war eine **Last-Flake**. Passend dazu: die `re15_pc.exe` beendete sich in
dieser Sitzung mehrfach unvorhersehbar mit exit 1 mitten im Lauf (Bild ~510, ~706,
~1170, ~1749), immer dann, wenn parallel gebaut/getestet wurde. Das ist eine
**Harness-Fragilitaet**, kein Code-Regress — aber sie macht jede laengere
Laufzeit-Messung unzuverlaessig und sollte behoben werden, bevor der naechste
A/B-Beweis gefahren wird.

**Konsequenz fuer den naechsten Schritt:** die unten aufgefuehrten „NAECHSTER SCHRITT"-
Punkte gelten unveraendert — sie sind jetzt **Nachmessungen an einem gelieferten Stand**
statt Vorarbeiten zu einem Fix. Der visuelle Beleg fuer 1210 (Punkt 1) ist damit
**dringender**, nicht weniger dringend: es steht Code im Repo, dessen Sichtbarkeits-
Begruendung nicht end-to-end gemessen ist.

---

# FINDING 1: 1090-feuersound

## URSACHE (belegt)

Der Feuer-Sound in ROOM1090 ist **SUB_03.BGM SEQ#2**, eine eigene BGM-Spur:
58 Note-Ons, ausschliesslich die Noten 60/61/62 **[eigen, eigener Parser]**. Ihr erstes
Ereignis ist `00 b0 00 05` (SUB_03.BGM @0x97) = **Bank-Select 5**, das sie per
`_SsSetControlChange` Fall 0 (`sh s6,0x4c(s5)` @0x8005dbd4) auf **VAB 5 = MAIN15.BGM**
umhaengt; `_SsNoteOn` liest genau dieses Feld (`lh a1,76(s0)` @0x8005da50).
MAIN15s Programm 0 traegt genau drei Tones mit `min = max = 60/61/62`
(VAG 15/16/17, Datei 0x1734/0x1754/0x1774) und ist in der Datei **stumm**
(mvol-Byte @0xF35 = 0, als einziges der acht Programme) **[eigen]**. Aufgedreht wird es
nur vom Raumskript: `ROOM1090.RDT @0x22EE  54 00 00 01 78 33` → `ProgAtr[0].mvol = 119`
(Handler @0x80044f28, `addiu v1,s2,255` / `sb v1,-15(v0)` @0x80044f6c-70), gegatet vom
Feuerloescher-Flag `Ck bank 3 bit 0x81 == 0` @0x22EA **[eigen, xxd]**.
Live-Gegenprobe im Original (`stage_saves/room1090_orig.sav`): `ProgAtr[0].mvol = 0x77`,
und das SEQ#2-Objekt @0x800b88cc traegt `+0x4c = 5` und `+0x90 = 1`.

Im Port war die Spur **tot**: `ss_mix` stieg mit `if (!s->vab_ok) return;` aus, und
`s_ss_sub2.vab_ok` wird in `re15_bgm_load_sub` explizit auf 0 gesetzt (SEQ#2 hat keine
eigene VAB, sondern `tone_src`). `ss_advance` hat genau einen Aufrufer — die Zeile hinter
diesem Guard. SEQ#2 wurde nie getickt, nie gekeyt, nie gemischt.

**Alle drei Pruefer: `haelt_stand = true`. Ich schliesse mich an.**

## BESTRITTEN

Nur Neben-Zahlen und Etiketten — nichts davon traegt den Befund:

* „`21 03 80 01` @0x22F4" → dort stehen `06 00 0c 00`; die Bytes liegen bei **0x22F8** **[eigen]**
* „ProgAtr[0] … mpan = 1, attr = 0x42" → `+2` ist `prior`, `+3` ist `mode`, **mpan liegt
  auf `+4` und ist 64** **[eigen]** — widerspricht der eigenen Disasm-Herleitung des
  Befunds (`sb v1,-12(v0)` = ProgAtr+4)
* „~0,73-s-Raster" → gemessen **1.018 s** Mittel / 1.029 s Median (ppqn 48,
  tempo 0x0AC55A = 705882 us) **[eigen]**
* „~1,2 s pro Note" → gilt nur fuer das Rohsample; center 72/73/74 gegen gespielte
  60/61/62 = eine Oktave tiefer → effektiv ~2,4-2,8 s
* „0x801eed00 + 0x88 = 0x801eed98" → das ist 0x801eed88; 0x801eed98 ist der Lesecursor
* „die MAIN-VAB ist byte-identisch zur Datei" → falsch an genau den zwei Bytes, die den
  Befund ausmachen (mvol, mpan)
* Lautstaerke „78 → q15 20130" → 77 → 19866
* „**DER** entscheidende Test" fuer die A/B-Bit-Identitaet → sie ist nicht
  diskriminierend (ein Null-Ergebnis entsteht identisch bei vier verschiedenen Ursachen).
  Die Ursache steht allein durch die Code-Lesung fest.
* „CC0-Rebind ist korrekt modelliert" → der Port prueft hart `s->bank[chan] == 5`,
  im Original ist 5 eine Laufzeit-VAB-Id aus `DAT_800b21f1`. In sich stimmig, aber
  **kein byte-true Modell** — gehoert als Einschraenkung gemeldet.

## OFFEN

* **Regressionsflaeche derselben Zeile**: der Guard aktiviert JEDE `tone_src`-Instanz,
  also auch `SUB_0A` (16 Noten), `SUB_11` (107), `SUB_3F` (107). Ob deren SEQ#2 im
  ORIGINAL in ihren Raeumen hoerbar sind, ist ungemessen. (Diese drei haben kein CC0,
  laufen also auf der SUB-VAB.)
* **Kein Hoertest, keine PSX-Aufnahme.** „Feuer-Sound" bleibt eine Daten-Inferenz
  (drei Ein-Noten-Tones, vom Feuerloescher-Flag gegated; die VAGs sind rauschartig und
  tieffrequent — mit Feuer vereinbar, nicht beweisend). Lautstaerke-/Pan-Byte-Treue
  unverifiziert.
* **PSX-Target** (`platform/psx/src/audio_psx.c:814`) unbeschaut.
* **Zeitpunkt des CC0-Konsums** im Original nicht dynamisch nachgemessen.
* **Nicht-SCD-Ambiente**: der SCD-Weg ist sauber ausgeschlossen (kein `Se_on`, kein
  `Sce_espr_on` im ganzen Raum-Walk); ob die Engine-Seite eine zweite Tonquelle keyt,
  hat niemand geprueft.

## NAECHSTER SCHRITT

**Der Fix ist committet** (`f4010659`, `platform/pc/src/audio_pc.c`):

    ss_mix():
    -   if (!s->vab_ok) return;
    +   if ((!s->vab_ok && !s->tone_src) || !s->seq) return;   /* == ss_start-Wache */

**Ich habe ihn nachgemessen [eigen]** — Fenster absolute Bilder 800..1300,
Pro-Bild-RMS, deterministische Aufnahme (`RE15_AUDIO_CAP_SYNC`, 1470 Stereo-Frames
je Spielbild), Sprung `RE15_DEBUG_JUMP="1090@700"`:

| Aufnahme | mean | min | <RMS 5 | laengste Stille |
|---|---|---|---|---|
| VOR dem Fix, Feuer AN (`capA.raw`) | 507.6 | **0.69** | 8.4 % | **34 Bilder = 1,13 s** |
| VOR dem Fix, Feuer AUS (`capB.raw`) | 507.6 | 0.69 | 8.4 % | 34 Bilder |
| **NACH dem Fix, Feuer AN (`capA2.raw`)** | **1365.0** | **579.44** | **0.0 %** | **0 Bilder** |
| NACH dem Fix, Feuer AUS (`capB3.raw`) | 507.6 | 0.69 | 8.4 % | 34 Bilder |

`sha256(capA.raw) == sha256(capB.raw)` — die Vor-Fix-Bit-Identitaet reproduziert exakt.
Nach dem Fix divergiert der A/B-Lauf: **2.976.815 abweichende Samples**, max |diff| 6574,
**erste Abweichung bei Spielbild 762** (= 62 Bilder nach dem Raumsprung).
Und die vierte Zeile ist der Regressions-Beleg: mit geloeschtem Feuer liefert der Port
nach dem Fix **exakt** die alten Zahlen.

Damit ist die groesste offene Frage aller drei Pruefer geschlossen: **die Ebene spielt.**

Zu tun:
1. Paketieren (Memory `reai-v2-immer-paketieren`). Der Einschalter-Pin
   (`test_1090_flame_out_pin`) ist in f4010659 bereits ergaenzt.
2. In der Commit-Message **nicht** die widerlegten Zahlen wiederholen
   (0,73-s-Raster, mpan = 1, „byte-identisch zur Datei", 0x22F4).
3. `SUB_0A` / `SUB_11` / `SUB_3F` gegenpruefen (Regressionsflaeche).
4. PSX-Target nachziehen.

---

# FINDING 2: ada-animation

## URSACHE (belegt)

* **Identitaet:** Typ 0x42 → Root 0x8011cb70 (`sw v0,11444(at)` @0x8011e92c →
  `0x80072cb4`); Modell CDEMD0.EMS Blob 19 = LBA 1501 / 162524 B, Tabelleneintrag
  `0x80072f38 + (0x42-0x10)*8 = 0x800730c8` **[eigen]**.
  (Der Name „Ada" ist eine Community-Etikettierung; das Skript nennt sie „Woman".)
* **Die Eskorten-Subs posieren aus BANK 1** = Kanalpaar `+0x170`/`+0x174` = `dir[4]`/`dir[3]`:
  alle fuenf EXECs laden `a0 = +0x170` / `a1 = +0x174` (@0x8004f384/88, @0x8004f5c0/c4,
  @0x8004f7bc/c0, @0x8004fb14/18, @0x8004ff68/6c); `FUN_80022300` legt
  `dir[3] -> +0x174` @0x800224b8, `dir[4] -> +0x170` @0x800224c8.
  ⚠ **Nicht „ausschliesslich"** — der INIT selbst posiert aus BANK 0 (@0x8011cdd0/d4/d8).
* **Phasen-Tor `+0x6`** (dreiwertig): @0x8004f4f0/f4f8/f500/f508. Nur Phase 0 schreibt
  `+0x94`, `+0x95 = 0`, `+0x8f = 7` und setzt `+0x6 = 1`.
* **`anim_set` ist der einzige Frame-Stepper**: `+0x95` Post-Inkrement @0x8001f618,
  Wrap an der Clip-Laenge aus `+0x174`; `+0x8f--` @0x8001f5b4.
* **Sub 3 ist Halb-Tempo-Gehen — auf BEIDEN Eintritts-Pfaden [eigen]**: der
  `+0x8c = Tabelle>>1 = 37`-Schreiber (@0x8004fa7c/@0x8004fa84/@0x8004fa88) liegt im
  **gemeinsamen** Rumpf ab 0x8004fa54, in den der Phase-0-Block durchfaellt. Nur der
  Clip-Reseed `+0x94 = 5` @0x8004fa30 ist phase-0-exklusiv.
  → Der Pruefer [mechanismus] hielt beide Schreiber fuer unerreichbar; **er irrt.**
* **DECIDE-Schwellen lesen `+0x1d0`** (Folgepunkt-Distanz, gesetzt @0x8011ce94), nicht
  die Spieler-Distanz **[eigen]**: `sltiu 0x1f4` @0x8004f3bc → Sub 3 (`+0x5=3` @0x8004f3c8,
  **`+0x6=1`** @0x8004f3d8); `arc_test(+0x1dc/+0x1de, 0x400)` @0x8004f3e8-f0 → dito;
  `sltiu 0xbb9` @0x8004f434 → Sub 5 (`+0x5=5` @0x8004f440, `+0x6=0` @0x8004f450);
  Sub 5 zurueck bei `sltiu 0x3e8` @0x8004fd54 (`+0x6=0` @0x8004fd70).
* **Sub 5 (Laufen):** `+0x94 = 0` @0x8004fe88, `+0x8c = 0x80076c80[4] = 200` @0x8004fedc.
* **Sub 2 (Drehen):** `+0x94 = 5` @0x8004f734, `Yaw += arc_test(Spieler, 96)`
  @0x8004f780-a8, **kein** `pos_advance`.
* **`+0x9 == 1`-Zweig** (0x8004f100) endet ohne `anim_set` (`jr ra` @0x8004f1fc).
* **Port-Defekte, die real sind:** Renderer (`main.c:6836-6900`) und
  `re15_actor_clip_len` (`:4926-4932`) haengen an `state == 4` / `walk_active` →
  in State 1 wird die CONTAINER-Bank posiert; `re15_npc_escort_walk` setzt
  Clip/Frame/Blend in jedem Bild neu (kein `+0x6`-Tor); der globale Advancer
  (`player_common.c:975-976`) ueberspringt 0x42 nur bei `state == 4`; die Subs 2 und 5
  fehlen; DECIDE[1] schreibt im Port einen Clip, den das Original dort nicht schreibt.

## BESTRITTEN

* ⛔ **Die Grundgesamtheit der Messung [eigen].** Der INIT @0x8011ccac setzt `+0x4 = 1`
  zwar unbedingt (@0x8011ccc0) — testet aber am Funktionsende `grid_id & 0x40`:

      8011ce1c: lbu  v0,9(v1)
      8011ce24: andi v0,v0,0x40
      8011ce28: beq  v0,zero,0x8011ce44
      8011ce30: sb   v0,4(v1)      ; +0x4 = 4
      8011ce40: sb   v0,5(v1)      ; +0x5 = 6

  `Sce_em_set` legt `+0x9 = pc[3]` (`lbu v0,1(s2)` mit `s2 = pc+2` @0x800420f4,
  `sb v0,9(s0)` @0x80042164) **[eigen]**, und ROOM1090 @0x22CC ist
  `44 00 42 40 01 00 00 ff` → **grid_id = 0x40** **[eigen, xxd]**.
  → Der Aktor spawnt dort in **State 4 / Sub 6**, nicht in der Eskorte. Die Sonde des
  Befunds erzwingt `state = 1` und schaltet danach den SCD ab.
  Von allen Typ-0x42-Rekorden traegt genau einer `grid_id = 0`: **ROOM3070 @0x343c**.
  Weder der Befund noch der Pruefer [messung] hatte diesen Test (er las nur bis 0x8011cd90).
* **Aber:** die Folgerung des Pruefers [messung] („State 1 in ROOM1090 unerreichbar") ist
  **ebenfalls falsch [eigen]**. ROOM1090 sub03 (Datei 0x24CE..0x26E6) macht
  `Work_set kind=2 idx=0` @0x26A4 (= Gegner-Slot 0 = der 0x42-Aktor), dann Plc_neck /
  Sleep, dann **`Plc_ret` @0x26C4** → `+0x4 = 1` = ESKORTE.
  Seine Messung („0 von 8 Subs") misst den **Port**, nicht das Original.
* **Fix-Punkt 1 ist zur Haelfte falsch [eigen]:** `re15_npc_channel_anim`
  (enemy_ai_common.c:8660-8695) endet mit einem **ungegateten**
  `if (bank && bank->own_ok) return &bank->anim_own;` — in State 1 liefert die Funktion
  **bereits BANK 1**. Betroffen sind nur Renderer und `re15_actor_clip_len`.
  Nebenfolge, die der Befund uebersah: die AI-Uhr taktet schon heute gegen BANK 1
  (Laenge 30), waehrend der Renderer den Container-Clip (Laenge 20) posiert.
* **CDEMD1-Eintrag „@0x800732f8"** → falsche Adresse. Basis 0x80073178 (@0x800223e0),
  Eintrag fuer 0x42 = **0x80073308** = `[1505, 144032]`; an 0x800732f8 steht
  `[1439, 133400]` = Typ 0x40 **[eigen]**.
* **„Blob 19 mit exakt dieser Laenge"** → das Laengenwort im Blob ist 162488; die 162524
  entstehen erst mit der 36-Byte-Verzeichnis-Fussleiste.
* **„Ab Phase 1 ruft der Sub nur noch anim_set"** → der Rumpf ab 0x8004f548 liest die
  Tempo-Tabelle neu, macht den Yaw-Slew (`jal 0x8001aac4` @0x8004f5ac) und ruft
  `pos_advance` @0x8004f5d0. Nur die vier Seed-Schreiber sind phase-0-exklusiv.
* **„Ziel-Abstand 5200..1400 → Sub 5 haette feuern muessen"** → falsche Groesse
  (Spieler-Distanz statt `+0x1d0`). Die Schlussfolgerung ueberlebt (`+0x1d0` gemessen
  6926 → 430, 66/92 Bilder ≥ 3001), die Zahl gehoert nicht zur Schwelle.
* **„laesst den Spieler 60 Bilder weglaufen"** → `pl->x/pl->z` werden von
  `re15_game_step` jedes Bild ueberschrieben; der Spieler steht in allen 120 Bildern still.
* **Die Spalte `Bank=CONTAINER`** stammt aus einem Hand-Nachbau in der Sonde, nicht aus
  dem Renderer; er laesst `re15_actor_uses_loco_bank()` aus und modelliert
  `clip_override = (anim_flags & 0x04) ? motion : -1` (anim_select_common.c:256) nicht.
* **„Der Port setzt in Sub 3 Clip 2 — falscher Clip"** → es ist ein **DECIDE**-Write.
  Das Original schreibt in DECIDE[1] **ueberhaupt keinen** Clip (@0x8004f3a4-f3dc).
  Der Write gehoert **ersatzlos gestrichen**, nicht auf 5 umgestellt.
* **Messwert (4) „alle Subs erreicht → die KI laeuft korrekt"** → der Port-`case 1` ist
  eine andere Maschine; der Messwert prueft den Port gegen sich selbst.

## OFFEN

1. **Erreicht der PORT in ROOM1090 State 1?** Das Original tut es ueber sub03 @0x26C4.
   Wenn der Port nie ankommt, ist „Ada folgt mir nicht" ein **Work_set/Plc_ret-Routing**-
   Defekt und **kein** Animations-Bank-Defekt.
2. **In welchem Raum der Nutzer-Report spielt** — unbelegt.
3. **Sub 6** (0x8004ffc0) schreibt `+0x94 = 6` @0x8005001c, BANK 1 hat aber nur 6 Clips
   (0..5) → ausserhalb. Phasen-Tabelle @0x800112bc nicht zu Ende verfolgt.
   **Jetzt wichtiger als vorher, weil ROOM1090 direkt in Sub 6 spawnt.**
4. **`DAT_800aca3c` Bit 31** (CDEMD0 vs. CDEMD1) — kein Schreiber lokalisiert. Solange
   das offen ist, gelten alle Clip-Laengen nur fuer CDEMD0.
5. **Kein DuckStation-Savestate** mit einer laufenden Typ-0x42-Eskorte.
6. **Pose-Amplituden** (897 / 154 / 442) sind Sondenrechnungen, nicht nachgerechnet.
7. **`entity+0x8 == Typ`** ist angenommen, nicht belegt — der Tempo-Tabellen-Index
   haengt daran.
8. **Zensus** „20 Rekorde in 15 Raeumen" nicht nachgezaehlt.
9. **PSX-Renderpfad** ungeprueft; **Kollision/Geometrie** der NPC-Bahn ungeprueft.

## NAECHSTER SCHRITT

**Zuerst messen, VOR jedem Fix:** erreicht der Port in ROOM1090 State 1 fuer den
0x42-Slot? Weg: Raum mit vollem SCD ticken, `op_work_set`/`op_plc_ret` tracen,
`t->work_slot` an Datei-0x26C4 in sub03 pruefen, und feststellen, was sub03 startet.
Parallel: **Sub 6** aufloesen (Phasen-Tabelle @0x800112bc) — dort spawnt sie in ROOM1090.

**Implementierungsreif (unabhaengig davon), jede Konstante mit Adresse:**

1. Renderer (`main.c:6836-6900`) und `re15_actor_clip_len` (`:4926-4932`) muessen den
   own/BANK-1-Kanal auch fuer `state == 1` waehlen (@0x8004f384/88, @0x8004f5c0/c4,
   @0x8004f7bc/c0, @0x8004fb14/18, @0x8004ff68/6c; @0x800224b8, @0x800224c8).
2. Phasen-Tor `+0x6` dreiwertig (@0x8004f4f0/f4f8/f500/f508, @0x8004f320ff,
   @0x8004f9fc/fa04/fa0c/fa14).
3. Ein einziger Advancer — 0x42 auch in State 1 im globalen Advancer ueberspringen
   (`player_common.c:941-953` / `:975-976`); Beleg `anim_set` @0x8001f618 / @0x8001f5b4.
4. DECIDE[1]: `re15_npc_clip(e,2)` **ersatzlos streichen** (@0x8004f3a4-f3dc hat keinen
   `+0x94`-Store) und `+0x6 = 1` setzen (@0x8004f3d8).
5. Sub 3 = Halb-Tempo (`+0x8c = 75>>1 = 37` @0x8004fa7c/fa84/fa88) auf beiden Pfaden.
6. Sub 5 (Laufen): `+0x94 = 0` @0x8004fe88, `+0x8c = 200` @0x8004fedc, Dreh-Rate 72;
   Ein-/Austritt @0x8004f434/@0x8004f450 bzw. @0x8004fd54/@0x8004fd70.
7. Sub 2 (Drehen): `+0x94 = 5` @0x8004f734, `Yaw += arc_test(Spieler, 96)` @0x8004f780-a8,
   kein `pos_advance`.
8. `+0x9 == 1`-Zweig: nicht advancen (`jr ra` @0x8004f1fc).
9. **Neu und im Befund nicht enthalten:** das grid-Bit **0x40** @0x8011ce24 →
   `+0x4 = 4` @0x8011ce30 / `+0x5 = 6` @0x8011ce40 muss jede Eskorten-Arbeit respektieren.

**Gestrichen (unbelegt oder falsch):**

* Fix-Punkt 1, Teil `re15_npc_channel_anim` — beschreibt den Ist-Zustand.
* Die Typen-Liste `0x45/0x47/0x49/0x4b/0x4d` in Fix-Punkt 3 — nur fuer **0x47** gibt es
  einen Beleg (Root 0x8011dc68, Tabelle @0x80121808). Fuer 0x45/0x49/0x4b/0x4d **kein
  Beleg**; ein pauschaler Skip waere ein Rate-Risiko.
* Die Adresse `@0x800732f8` fuer den CDEMD1-Eintrag (korrekt: 0x80073308) — und der
  ganze CDEMD1-Zweig bleibt unbelegt erreichbar.
* „Sub 3 setzt Clip 2 im Port" als EXEC-Aussage (es ist ein DECIDE-Write).
* Die Perzeptions-Etiketten („kein Gangzyklus", „50-Bilder-Sturz", „feiner Atem-Idle") —
  Interpretationen von Amplituden, kein Disasm-Beleg.

---

# FINDING 3: 1210-arme

## URSACHE (belegt)

Der Mechanismus des Originals ist jetzt **vollstaendig** belegt — mehr als im Befund:

* **Dispatch [eigen]:** `grid_id & 0xf` @0x8010c4dc → `0x8012095c` →
  Dual-Dispatch A-Tabelle `0x80120968[+0x5]` (@0x8010c520/530) dann B-Tabelle
  `0x80120984[+0x5]` (@0x8010c554/564). A[0] (der einzige `+0x5 = 1`-Setzer, @0x8010c608)
  laeuft nur bei `+0x5 == 0`.
* **Lunge B[1] @0x8010c714, netto +2420:** `+0x9c = 3` @0x8010c7a8, `+0x8c = 800`
  @0x8010c7b8 (3 Bilder), dann `+0x8c = 20` @0x8004f7f8… (`@0x8010c7f8`), `+0x9c = 30`
  @0x8010c808, `+0x94 = 2` @0x8010c818, 30 Bilder mit param 0x800 @0x8010c8b4,
  `+0x8c = 200` @0x8010c860, 4 Bilder mit param 0 @0x8010c914,
  Ausgang `+0x5 = (rand&1)+2` @0x8010c8e4.
  `3*800 + 20 − (30*20 + 200) + 4*200 = +2420`.
* **Bewegung:** `FUN_800245d8` — `+0x8c` als skalare Schrittweite (@0x800245f0),
  Drehung um `+0x6a + param` (@0x80024658/60/64) per GTE-MVMVA, Addition auf `+0x34`
  (@0x800246e4) und `+0x3c` (@0x800246f8).
* **B[2] @0x8010c938 ist netto NULL [eigen — gegen den Pruefer [mechanismus]]:**
  `+0x9e` und `+0x9f` bekommen **denselben** Wert `N = (rng&3)+1` (@0x8010c9a8 und der
  Verzoegerungsschlitz @0x8010c9b0), `+0x8c` wird **einmal pro Zyklus** geschrieben
  (@0x8010c9c4). Phase 2 = `N+1` Bilder rueckwaerts (param 0x800 @0x8010ca78),
  Phase 3 = `N+1` Bilder vorwaerts (param 0 @0x8010cb10). Summe 0.
* **B[3] @0x8010cb34 bewegt gar nicht [eigen]:** im ganzen Bereich `0x8010cb34..0x8010ce44`
  kein `jal 0x800245d8` und kein `+0x8c`-Schreiber.
* **Die Lunge ist EINMALIG pro Raum-Instanz [eigen — schliesst ein OFFEN des Befunds]:**
  ich habe **alle neun** `+0x5`-Schreiber im Writher-Baum (0x8010c1ec..0x8010d4bc)
  aufgeloest — @0x8010c628 (=1), @0x8010c65c (=2), @0x8010c8e4 (=(rnd&1)+2),
  @0x8010cacc (=3), @0x8010ce0c (=2), @0x8010cf60 (=5), @0x8010cf94 (=2),
  @0x8010d08c (=6), @0x8010d164 (=4). **Keiner schreibt 0 oder 1**, HURT und Submerge
  eingeschlossen.
* **Die AOT-Kette, vollstaendig [eigen — die groesste Luecke des Befunds]:**
  Opcode `0x2c` @0x80040534 latcht `pc+2` in `0x800ac9b0[pc[1]]` (`sw v0,0(v1)` @0x80040584).
  Deskriptor @Datei 0x1EB0: sce 3, flags 0x41, Rechteck (x −22100, z −15000, w 5200, d 1700);
  Feld-Semantik bewiesen in FUN_80042bac @0x80042e60-0x80042e9c, Rechteck-Test
  `FUN_80042b64` (`(px−x0) u< w && (pz−z0) u< d`).
  sce 3 → `0x8007469c[3] = 0x800430f0` → `lhu a0,0(v0)` / `lbu a1,3(v0)` auf
  Deskriptor+12 (Datei 0x1EBC = `ff 00 18 02`) → `FUN_8003ee3c(0x00ff, 2)` →
  `FUN_8003edec` `sw v0,28(a0)` @0x8003ee38 = **sub_scd[2]**.
  sub02 @Datei **0x1ED6..0x1F25** = zehnmal `Work_set(2,NN)` + `Member_set(12,1)`
  → **grid = 1 auf ALLE ZEHN gleichzeitig**.
* **Region-Cull [eigen]:** `@0x8001e97c` ist eine **Weiche** zwischen zwei Teil-Schleifen
  (`jal 0x8001e9ec` @0x8001e990 in-Region, `jal 0x8001ef54` @0x8001e9b4 ausser-Region),
  **kein** Ueberspringen. Dass trotzdem nichts gezeichnet wird, folgt erst daraus, dass
  die Mesh-Einreicher `0x800254a0` / `0x800256b0` / `0x80023410` / `0x80023708`
  ausschliesslich in `FUN_8001e9ec` vorkommen — ich habe alle fuenf Callees von
  `FUN_8001ef54` gescannt.
* **Der Port-Zustand ist eine beauftragte Abweichung**, kein Defekt:
  `enemy_ai_common.c:10758-10780` traegt woertlich „⛔ BEWUSSTE NACHRUESTUNG
  (Nutzer-Auftrag), klar getrennt von byte-true" und zitiert den Nutzer, der das
  „alle auf einmal" des Originals **verworfen** hat.

## BESTRITTEN

* **„Teil-Zeichnen UEBERSPRINGEN" / „verwerfen BEVOR ein Mesh gezeichnet wird"** —
  Weiche, nicht Cull. Ergebnis stimmt, Beweisfuehrung nicht.
* **Datei-Offsets `0x1ED5..0x1F23`** → `0x1ED6..0x1F25` **[eigen]**.
* **„Radius 300 @0x8012091c"** → dort steht 0; die **300 liegt @0x80120922**
  (der Aufrufer laedt `lhu a1,6(v0)` @0x8010c31c) **[eigen]**.
* **„Griff-Tor `< 0x4b0` @0x801018f4"** → in STAGE1.BIN steht dort `nop`; die Instruktion
  liegt in `info/re2leon/COMMON/BIN/EMOVL10_S0.BIN` (RE2) **[eigen]**.
* **„FUN_8010c938 (netto 0)"** ohne Adresse — inhaltlich richtig, aber im Befund eine
  blosse Behauptung (RE-Gate-Verstoss).
* **Messwert (6)** zaehlt „in wie vielen der 9 Vierecke ueberhaupt"; der Cull testet nur
  das Viereck des **aktiven** Cuts. In der Harness des Befunds blieb der aktive Cut ueber
  alle 273 Bilder auf 0 **und der Spieler lag in 254/273 Bildern ausserhalb seines
  eigenen Vierecks** — kein Spielzustand. Auf einer Route, die RVD-Zone z1 @0x019c
  wirklich kreuzt, ergibt sich 258/329 „drin" fuer die Lunge und 0 fuer den Spawn:
  die Schlussfolgerung reproduziert, aber erst mit einer Messung, die der Befund nicht
  gemacht hat.
* ⛔ **„Kleinster Spieler-Abstand 5418 → das Griff-Tor kann nie feuern"** — widerlegt
  vom **eigenen Abschnitt (1) des Befunds** (Arm 3 Abstand 0, Arm 6 Abstand 849) UND vom
  **gruenen ausgelieferten Pin** `test_1210_gitterhaende.exe` (3 Zugriffe, 288
  Opfer-Bilder, 6 Bisse a 20 HP). **Damit faellt Fix-Schritt 5 komplett.**
* **Messwert (5)** misst Bone-Ursprungs-Weiten, nicht Mesh-Vertices (probe_1210_griff:
  442 bzw. 680 Einheiten Radius).
* **Fix-Punkt 7** („Einzel-Gate behalten UND byte-true") — in sich widerspruechlich:
  sub02 feuert alle zehn.
* **„Die Begruendung im Kopfkommentar ist widerlegt"** — der Kommentar nennt als ERSTEN
  Grund den Nutzer-Auftrag. Der Befund behandelt eine beauftragte Abweichung als Defekt.
* **„Der Port verwirft den Arm VOR Bank-Auswahl"** — falsch, der EMD-Bank-Load steht
  absichtlich davor (`main.c` ~6588, mit Kommentar).

## OFFEN

1. **Kein visueller Beleg — auf beiden Seiten.** Kein gdigrab des Ports, kein
   DuckStation-Bild des Originals. Die Kernaussage „wird gar nicht gezeichnet" ruht
   ausschliesslich auf Code- und Datenschluss.
2. **Masken-/OT-Prioritaet ungemessen** (Mask-OT `depth*1` @0x80039658 gegen Char-OT
   `otz>>4` @0x8002565c) — entscheidet, ob eine Aenderung ueberhaupt sichtbar wuerde.
3. **Y-Achse** nie gegen die Gitterhoehe im BSS abgeglichen.
4. **`FUN_8010c714` laeuft genau einmal pro Bild** — nur strukturell erschlossen. Die
   Bildzaehlung 3/1/30/1/4 und damit die 2420 haengen daran.
5. **Der Port-Kamera-/Region-Zustand von ROOM1210 ist nicht validiert** (Uebergang
   Cut 3 → 4, RVD-Zone z10 @0x0250, feuert nicht). **Solange das offen ist, taugt KEINE
   Sonde dieses Raums als Sichtbarkeits-Orakel.**
6. **Spawn-Flag-Tor** `record[7]` (@0x80042120/@0x80042130/@0x80042138) — wie viele Arme
   ueberhaupt spawnen, haengt am Story-Flag-Stand. Nicht nachgemessen.
7. **B[3]-Skalier-Kanaele** (@0x8010cc0c-0x8010cc70, @0x8010cd34-0x8010cd9c) — ein
   zweiter, nicht portierter Sicht-Mechanismus; Render-Wirkung nicht hergeleitet.
8. **HURT-Pfad** 0x8010d0f8 / `pos_advance(0x800)` @0x8010d2f8 — weitere
   Translationsquelle, nicht disassembliert (kann die Einmaligkeit nicht kippen).

## NAECHSTER SCHRITT

**Kein Fix.** Begruendung, in dieser Reihenfolge:

1. Der Port-Zustand ist eine **stehende Nutzer-Entscheidung**.
2. Die zentrale Praemisse des Vorschlags ist durch einen **gruenen ausgelieferten Pin**
   widerlegt.
3. Eine byte-true einmalige Lunge (+2420 dauerhaft) ist mit dem gruenen Pin und mit dem
   Nutzer-Wunsch „sie kommen und gehen beim Vorbeigehen" **nicht vereinbar**.

**Zuerst zu messen:**

| # | Was | Weg |
|---|---|---|
| 1 | Was der Nutzer tatsaechlich sieht | Skill `re15-port-visual-verify` (gdigrab, plain exe, `RE15_DEBUG_JUMP="1210@<frame>"`) + DuckStation-Bild derselben Stelle |
| 2 | Port-Kamera-Zonen ROOM1210 | Uebergang 3→4, RVD-Zone z10 @0x0250 |
| 3 | Masken-/OT-Prioritaet | @0x80039658 gegen @0x8002565c fuer die Lunge-Position |
| 4 | Spawn-Flag-Tor `record[7]` | `FUN_8004efe4` + Flag-Bank 0x800b48bc |

**Falls der Nutzer spaeter byte-true 1210 beauftragt**, ist alles unter „URSACHE" oben
implementierungsreif (eigene Writher-Felder fuer `+0x8c/+0x9c/+0x9e/+0x9f/+0x1d0`,
Radius **300 @0x80120922**, Maske **4 @0x8010c318**, Ausloeser sub02 @0x1ED6 auf alle zehn).

**Gestrichen:** Fix-Punkt 5 (Praemisse widerlegt), Fix-Punkt 7 (widerspruechlich),
`@0x801018f4` ohne Datei-Angabe, und die konkrete Formel
`x += (S*cos_q12(yaw+p))>>12` — `0x800659d0 = RotMatrixY` ist ein **behauptetes Symbol**,
die Vorzeichen-/Achsenkonvention damit ungesichert.
