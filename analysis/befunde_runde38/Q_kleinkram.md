# Runde 38 — Spur Q "kleinkram" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r38_kleinkram` (Zweig `r38/kleinkram`, Basis 1bcbd0ff). Wortlaut: `AUFTRAG.md` (massgeblich),
Zuteilung: `VERTRAG.md` Spur Q (Ressourcen §1.2: 1151/30E0/30E1 nur vorhandene Nachrichten, keine neuen Ids).

## Punkte (Wortlaut des Nutzers)
1. "Das öffnen des Player Inventories hat keinen Sounds, wenn Resident Evil 2 dort Sound hat, möchte ich auch Sound haben."
2. "In Elzas Szenario bei ROOM 1151 kommt - wenn man Irons anklickt erst - he already passed - und dann beugt sie sich hin
   und es kommt "I'll be fine, just worry About yourself for now". Ist natürlich quatsch … sie soll sich nur hinbeugen, und
   es soll stehen "he already passed"..."
3. "In Room 30E0 soll beim aufnehmen des Flammenwerfers nicht mehr kommen - not available in the preview, sondern einfach
   der Flammenwerfer aufgenommen werden."

## Stand
- 2026-10-16: Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.
- 2026-10-16: Bau des Basisstands 1bcbd0ff im Baum (local_build.sh configure + build, EXIT 0). Messkopie
  `re15_pc_r38q.exe` neben der exe, Arbeitsordner im Scratchpad (nie im Baum).

## 1. Messung vorher (Stand 1bcbd0ff, echte exe)

### P1 Inventar-Ton
* Lauf a/b: RE15_DEBUG_JUMP=1000@250, Leon (21850,-13400) rot 2048, RE15_AUDIO_CAP_SYNC=cap.raw, RE15_SE_DEBUG=1;
  a mit `RE15_PRESS=start@400,start@500` (Inventar auf/zu, debug.log `[inv] Sicherung im Statusschirm ...` nach F390,
  walk-Zeilen F420..F480 eingefroren), b ohne Druck, beide bis `EXIT_AT=600#1000`.
* Ergebnis: beide Mitschnitte 854 Bilder, **0 abweichende Bilder** (bytegleich) — Oeffnen UND Schliessen sind stumm.
  ROOM1000 ist ab der Sprungstelle still (Spitzenwert 0 ab Mitschnitt-Bild 314), ein SE waere sichtbar. Kein
  `[se] Stimme`-Eintrag im debug.log.
* Ursache im Code: der einzige Oeffnen-Ton steht im DEBUG-Umschalter `re15_menu_toggle` (menu_common.c, d4b41678
  2026-08-31 "Inventar-OEFFNEN: Auswahl-Ton"); der echte Weg START -> `re15_menu_start_poll` -> Stufe 1..2 spielt
  keinen. Tab-Auswahl-Abbruch (START/Kreuz) ist RE1.5-stumm ("No SE", @0x800499d0-a18).

### P2 Elza ROOM1151 Irons
* Lauf: Elza-Spielstand (probe_r36_elza_karte) ROOM1151 bei der Gespraechslage aus sub03 (@0x00ECE Pos_set
  (-20538,-25147), @0x00ED6 Dir_set 1500), CONTINUE, `RE15_PRESS=square@60,...@120,...` = echter Aktionsweg.
* state.log: F60..F120 `msg(a=1 fsm=1 id=20)` ("He has already passed..."), nach dem Wegdruecken ab ~F202
  `msg(a=1 fsm=1 id=2)` (99 Bilder) = "Irons: I'll be fine. Just worry about yourself for now." — genau der
  Nutzerbefund. SCD-Spur: F122 slot=1 @0x0E9E Cmp faellt durch -> @0x0EA4 Ck(3,0x9D) -> @0x0EA8 Set -> @0x0EAC
  Evt_exec sub03 -> @0x0EC2.. (Pos/Dir/Plc_neck/Plc_motion(1,0x0B) = Hinbeugen, Cut 7, Message_on 2).
* Warum der Runde-36-Riegel das nie sah: integration/unit_r36_elza_irons feuern Slot 2 per RE15_FIRE_AOT /
  re15_aot_fire_slot — das umgeht den Vorwaerts-Stempel des Scans (s. Abschnitt 2).
* Regression? Nein: Mechanismus seit dem Runde-36-Bau 3eaf631d unveraendert (Stempel aot_common.c seit 289823a7
  2026-07-11 bzw. fa0d53a1 2026-07-20, Frame-Ende-Wisch scd_vm.c seit 2026-08-08); der Fehler ist der
  Runde-36-Entwurf selbst ("work_vars[0] wird nie 2" stimmt nicht). Verursachender Commit: 3eaf631d.

### P3 ROOM30E0 Flammenwerfer
* Lauf: RE15_DEBUG_JUMP=30E0@250, Leon (-25000,-26400) rot 2048 vor dem Panel (Slot 3 @0x00A5A), Eingabe
  `A0.1,W0.5,L0.8,W0.5,A0.1,W1,R0.13,W0.5,A0.1,W3` ab Bild 300 (BASIS spiel). Echter Ablauf: Viereck am Panel ->
  sub09 (Cut 2, Modus (5,0..4,6)); LINKS 24 Bilder = sub08 @0x1034 (x -200 je Bild); Viereck -> sub10 (Cut 3);
  RECHTS 4 Bilder = sub07 @0x1028; Viereck -> sub04 -> F410 `@0x0FEE op=0x2B` -> `[msg] room=30e0 id=15`,
  state.log `cam=3 msg(a=1 fsm=1 id=15)`. Framedump F460: Fernansicht des offenen Spinds mit dem Flammenwerfer im
  Gestell, Fadenkreuz darauf, Text "There's a Flamethrow..." (Schreibmaschine).
* ROOM30E1 (Elza) hat KEINEN Flammenwerfer: Zensus aller RDT-Nachrichten — der einzige Waffen-Preview-Text ist
  ROOM30E0 msg 15 @0x14D2 (die uebrigen 16 Preview-Texte sind "Itembox ..."); ROOM30E1 = John-Szene ohne Panel.
