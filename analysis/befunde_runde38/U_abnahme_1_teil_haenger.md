# Runde 38 Spur U "wasserzombie" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Datum 2026-10-17. Baum `.claude/worktrees/r38_wasserzombie`, Zweig `r38/wasserzombie`, geprueft HEAD `2e04fd53`
(merge-base mit master `cb782bda`). Vorgaenger: `U_abnahme_0.md` (HEAD a4809c35, 2 Maengel). Fortlaufend geschrieben.

## 0. Stand der Pruefung

- [x] AUFTRAG/VERTRAG/Dossier (inkl. Abschnitt 8 Nachbesserung 1) und Abnahme 0 gelesen
- [x] Code-Diff gelesen: `git diff master...HEAD` (12 Dateien; `git diff master` zeigt zusaetzlich Orchestrator-Dateien
      nur, weil master seit der Basis weitergelaufen ist) und `git diff 371f4dc8 HEAD` (Nachbesserung 1 allein)
- [x] Bau: `local_build.sh configure` OK, `local_build.sh build` -> `ninja: no work to do`; Arbeitsbaum sauber; exe
      06:53:01 neuer als alle Quellen (wasserzombie_r38.c 06:51:24, enemy_ai_common.c 06:36:42); exe enthaelt den
      neuen String `zwischenbild` -> die exe ist der Stand der Nachbesserung
- [x] Riegel der Spur: `unit_r38_wasserzombie` gruen (0.07 s), `integration_r38_wasserzombie` gruen (122.25 s, Laeufe a-e)
- [ ] Gezielte Gruppen der beruehrten Module
- [x] Mangel 1 an der exe nachgemessen (M5, Leon) — siehe 2
- [ ] Nutzer-Punkt erneut gemessen (Leon/Elza, beide KI, Bild), Nebenwirkungen
- [x] Mangel 2 (Hygiene) geprueft — siehe 3
- [ ] RE-Gate (Stichproben selbst disassembliert)
- [ ] Vertrag
- [ ] Urteil

## 1. Nutzer-Punkt (Wortlaut)

> Die Zombies in Stage2 die sich unten im Wasser befinden und zum Rand des Wassers laufen müssen sich an den Rand
> hochziehen können, wie im Original Resident Evil 1.5

## 2. Messung an der exe

Messkopie `re15_port/build/platform/pc/re15_pc_u_abn1.exe` (md5 2a5d6e34… = re15_pc.exe; eigener Name, kein
taskkill). Arbeitsordner im Scratchpad (nicht eingecheckt):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/U38abn1/`
(Skript `tools/lauf.sh`, je Lauf ein Unterordner mit debug.log/wz.log/gy.log/fd). Gemeinsame Umgebung:
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_WASSERZOMBIE_LOG=wz.log
RE15_GEGNER_Y_LOG=gy.log RE15_DEBUG_JUMP=2070@250 RE15_PLAYER_POS=-18500,-6400,2048,0`, Wegplan `RE15_WEGPLAN`
(`RAUM 2070 / WARTE 20 / BLICK 2048 / WARTE 10 / AKTION / RAUM 2000 / WARTE 10 / GEHE -20600 -10600 200 / BLICK 2048 /
WARTE 1500 / ENDE`) = ECHTER Tuerweg ROOM2070 -> ROOM2000 (Elza: 2071 -> 2001, `RE15_TITLE_CONFIRM_MS=1500
RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_SET_FLAG=3:69`).

### M5 — Mangel 1 nachgemessen: fremder Schreiber nach dem eigenen Tick (Leon, Standard-KI RE2)

Befehl: `tools/lauf.sh <exe> m5 "406#2000" leon RE15_FRAMEDUMP=396-404/1:fd/f`. wz.log (woertlich, Auszug):
```
F340 R2000 slot=3 t=16 re2-eintritt st=1/10/0 mo=0 af=63 @(-22068,0,-9970) b0 f1ba=0 rot=165 kontakt=09
F351 R2000 slot=3 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
F400 R2000 slot=3 t=16 zwischenbild st=1/9/0 mo=22 af=49 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
F401 R2000 slot=3 t=16 abbruch st=1/9/0 mo=22 af=50 @(-22068,0,-9970) b0 f1ba=0 rot=0 kontakt=00
```
Abnahme 0 (alter Stand, `m_domino/wz.log`): `F401 ... abbruch st=1/9/0 mo=22 af=9` — jetzt af=50 (Uhr im Zwischenbild
auf der RE1.5-Bank, Clip 0x16 = 159 Bilder). gy.log md5 `68c1de6c…` = **byte-gleich** mit dem gy.log der Abnahme 0
auf dem alten Stand (`m_domino/gy.log`, 720 Zeilen): die Nachbesserung aendert keine Ausfuehrung, nur Bank/Uhr im
Zwischenbild.
Bild (`U38abn1/m5_crop.png`, F396-F404, Ausschnitt x40-140/y80-230, 3x; `m5_full.png` F398-F401 ganz): F396-F399
haengt der Kletterer mit dem Oberkoerper auf der Stegkante, Beine im Kanal; **F400 haengt er unveraendert weiter an der
Kante** (vorher: bis auf ein Bein verschwunden). F401 steht er im Taumel-Clip 13 im Kanal (eigener Abbruch-Tick,
EXEC[9] P0 + Schub), F402-F404 taumelt er rueckwaerts. **Mangel 1 ist behoben.**

## 3. Mangel 2 (Code-Hygiene) geprueft

- `grep -rn "re15_enemy_ai_live_edge_fall|EDGE-FALL|edge_fall" engine/src include tests`: einziger Treffer ist die
  Vorher-Beschreibung im Kopf `include/re15_wasserzombie_r38.h:30` ("WAS DER PORT VORHER TAT") — die Funktion ist weg.
- Kommentar am Aufruf (enemy_ai_common.c ~4732): "HOCHZIEHEN an der u1=1-Kante (@0x8011f890[0xa]=FUN_801033c8 /
  STAGE2 0x8010325c): +0x38 -= 1800 und +0x82 += 1 = ein Band HOCH ..." — stimmt.
- `gcc -std=c11 -Wall -Wextra -fsyntax-only -DRE15_PLATFORM_PC=1 -Iinclude` auf wasserzombie_r38.c,
  enemy_ai_re2_zombie.c, schuss_r36.c, enemy_ai_common.c, test_r38_wasserzombie.c, probe_r38_wasserzombie_karte.c:
  keine Meldung aus einer Spur-U-Zeile oder aus `re15_wasserzombie_r38.h`; uebrig `-Wcomment` aus
  `include/re15_emd.h:37` und in enemy_ai_common.c Z. 1518/6113/11346 (git blame: 2026-07-06 .. 2026-08-25,
  vorbestehend). **Mangel 2 ist behoben.**

