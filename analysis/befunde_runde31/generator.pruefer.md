# Gegenpruefung Runde 31 — Thema "generator" (ROOM11F0 Generator-Raetsel + ROOM1170 Items)

Pruefer-Dossier. Zweig r31/generator, Arbeitsbaum .claude/worktrees/r31_generator.
Gemessen wird der GEBAUTE Stand gegen den Nutzerauftrag, nicht gegen die Absicht des Bauers.

Nutzerauftrag (Teil dieses Themas):
> "... gleiches in room 11f0 bei den generator raetsel. warte erst bis der zeiger final auf 80 steht,
> bevor du mit ok das abnimmst, das Licht anschaltest etc."
(ggf. auch: "starte mit dem Aufnahme Dialog der items erst wenn das Modell wirklich komplett hochgefahren ist")

## 0. Status (laufend)
- [ ] Dossier des Bauers gelesen
- [ ] Commits gesichtet (git log r30/integration..HEAD)
- [ ] Bau + Suite (local_build.sh all) — Summenzeile
- [ ] Byte-/Adress-Stichproben
- [ ] Nutzerszenario im echten Spiel (Framedump), Bilder angesehen
- [ ] Nicht gefahrene Faelle (No, Wiedereintritt, Save/Load, zweite Fahrt, Raumvariante)
- [ ] Tests: pruefen sie Verhalten oder nur Absicht?
- [ ] Regressionen Nachbarthemen
- [ ] Urteil

## 1. Was der Bauer behauptet
(Dossier analysis/befunde_runde31/generator.md)
- Vorher: RE1.5-Kette sub01 @0x012E6 Evt_exec(sub18) zog im Bild nach dem letzten Schalterbit
  (F1184, Zeiger 62), Cut 8 ab F1185, Zeiger erst F1202 auf 80 (unsichtbar).
- Nachher: op_evt_exec haelt @0x012E6 zurueck (SCD_R_IF_FALSE -> Blockende des Ifel_ck @0x012B6),
  bis Zeiger 80 und 30 Bilder Stillstand (RE2 sleep 0x1E @0x0171C/@0x0171D). Abnahme F1233 = k+31.
- Eingabesperre (SCD-Pad-Woerter & 0xf000, Inventar/START zu) waehrend Fahrt + 30 Bilder und bis
  zur Abnahme (RE2 Bank 2 Bit 7 @0x01110..@0x01818; RE1.5 @0x800304f8/0x80030514).
- ROOM11F1 (byte-identisch) traegt den Zeiger jetzt auch.
- Riegel unit_r31_generator (A..F), probe_gen_11f0 M6 angepasst, Suite 406/406.

## 2. Commits / Diff
Zweig-Commits (merge-base 7cb74897): 10d0c706 (Auftrag), 06021dbd, e7c39887, 8437638c (Dossier/Belege),
bffb34b9 (fix: Code), df09d1da, 80c71bd2 (Dossier). ⚠ `git diff r30/integration..HEAD` zeigt auch
release/-Loeschungen — das ist nur r30/integration, das weitergelaufen ist (cade0796); mit drei Punkten
(`...`) aendert der Zweig NUR: game_step_common.c (+12), panel_zeiger_common.c (+99), scd_vm.c (+16),
re15_panel_zeiger.h (+64), probe_gen_11f0.c, r31_generator.c/.cmake, local_build.sh (405->406).
Spielcode-Aenderung klein und lokal: Haken in op_evt_exec greift nur bei Raum 11F0/11F1 UND
pc-raw == 0x012E6 UND Bytes `04 ff 18 12 22 04 ee 01`.

## 3. Bau + Suite
(folgt)

## 4. Stichproben Bytes/Adressen
Alle selbst nachgelesen (xxd / re2_disasm.py / re15_disasm.py):
- RE1.5 ROOM11F0.RDT (md5 47cd154e... = info/Re1.5/PSX/STAGE1): @0x012B6 `06 00 36 00`, @0x012BA
  `21 04 ee 00`, @0x012BE..0x012E2 zehn Ck `21 05 0d 01`..`21 05 16 00`, @0x012E6 `04 ff 18 12`,
  @0x012EA `22 04 ee 01`, @0x012EE `08 00`. sub18 @0x016F6 `22 04 f3 01`, @0x01736 `22 02 07 01`,
  @0x0173A `29 08`, @0x01742 `2b 02 ff ff`, @0x01748 `29 0d`, @0x01784 `22 02 07 00`. BESTAETIGT.
- `cmp ROOM11F0.RDT ROOM11F1.RDT` -> identisch (152588 B). BESTAETIGT.
- RE2 ROOM2130.RDT: @0x01110 `22 02 07 01`, @0x011E0 `0f 06 36 00`, @0x01216 `02`, @0x01218 `10 00`,
  @0x012A4 `09`+`0a 1e 00`, @0x01708 `10 00`, @0x0170C `64 05 16 02`, @0x0171C `09`, @0x0171D
  `0a 1e 00`, @0x0172C `29 04`, @0x01752 `23 00 05 00 50 00`, @0x01758 `2b 00 07 00 ff ff`,
  @0x0175E `22 04 3c 01`, @0x01762 `36 02 0c 01`, @0x01818 `22 02 07 00`. BESTAETIGT.
- RE2-Tabelle @0x800a74c8: Eintrag 9 = 0x800539DC (sleep: Zaehler aus pc+2 anlegen, pc+=1,
  `addiu v0,zero,1`), Eintrag 10 = 0x80053A24 (sleeping: Zaehler-1, bei 0 pc+=3; IMMER
  `addiu v0,zero,2` = Bild abgeben). Letzter Zeigerschritt Bild k -> Sleeping k+1..k+30 ->
  Pruefung k+31. Herleitung des Bauers BESTAETIGT.
- RE2 Pad-Sperre @0x800391F8 `lw v1,0x800cfbdc` / @0x800391FC `lui a0,0x100` / @0x8003921C
  `andi v0,v0,0x3c00`; Bank-Tabelle @0x800a78c8[2] = 0x800CFBDC. RE1.5 @0x800304F4 g_pauseflags,
  @0x800304F8 `lui v1,0x100`, @0x80030514 `andi v0,v0,0xf000`. BESTAETIGT.
- Nutzer-Gewichte {20,-20,-10,-30,20,-40,20,-50,30,-60}: 80 nur aus 90-10 = {1,3,5,7,9}
  (Positivsumme max 90; 80 ohne Schalter 3 nicht erreichbar) -> ziel==80 <=> Maske 0x155 <=>
  RE1.5-Loesung. Damit kein Dauer-Sperre-Fall "Zeiger 80, SCD loest nicht" ueber die Gewichte.
- Sub01-Tasten (Sce_key_ck 0x01/0x02/0x04/0x08 Cursor, 0x40 Schalter UND EXIT @0x01296 mit
  Member 0x0C -> sub17) liegen alle ausserhalb 0xf000 -> Sperre hält auch den EXIT zu.
- Takt: SCD/Spiel-Tick 30 Hz (main.c:5299 ff.) = RE2-Skripttakt -> 30 Bilder = 1 s.
- RE2 message_on: Tabelle @0x800a7574 = 0x80054A8C, Ruecksprung @0x80054AD4 `addiu v0,zero,1`
  (weiter, kein Bild-Abgeben) -> Meldung/Flag/Ton im selben Skriptbild. BESTAETIGT.
- RE1.5 FUN_8003f038: @0x8003f064 `ori a0,zero,0x1`, @0x8003f070 `lw v0,68(v0)` (sub_scd[1]),
  @0x8003f07c `sw v0,0x800b3f70`, @0x8003f080 `jal 0x8003ee3c` -> sub01 jedes Bild neu (Gate
  nur g_pauseflags & 0x02000000 @0x8003f044). BESTAETIGT; damit ist das "Zurueckhalten per
  IF_FALSE, im naechsten Bild neu pruefen" mechanisch tragfaehig.
- Port-Dispatcher scd_vm.c:718 SCD_R_IF_FALSE: Pop des Blockstapels -> pc = block_end des
  Ifel_ck @0x012B6 (derselbe Weg wie ein falsches Ck). Kein Seiteneffekt ausser 'nichts tun'.

## 5. Nutzerszenario live (Framedump)
(folgt)

## 6. Nicht gefahrene Faelle
(folgt)

## 7. Tests
- unit_r31_generator (re15_port/tests/unit/r31_generator.c) gelesen: faehrt VM + re15_game_step in
  Hauptschleifen-Reihenfolge (scd_vm_tick vor game_step), echte RDT ROOM11F0/11F1.
  B/C/F pruefen das Ergebnis (4:238, 4:243, Tonzaehler) gegen eine SOLL-Zahl 30 mit Beleg
  @0x0171D, NICHT gegen das Port-Makro (Makro-Gleichheit nur als eigener Check) -> kein
  selbstbestaetigender Riegel. C enthaelt die Gegenprobe "durch die 80 gefahren" (60->90).
  D prueft die Sperre am Verhalten (Cursor-Prop dz, Schalterwechsel), mit Gegenprobe davor/danach.
- Schwaechen: Schalterbits werden per maske_setzen DIREKT gesetzt (sub06-Kippung mit 16 Bildern
  und Set @0x01340 laeuft im Riegel nicht) — der Live-Lauf deckt das ab. E (Wiedereintritt)
  setzt 4:238 von Hand und ruft reset: prueft nur den Tick, nicht einen echten Raumwechsel.
- probe_gen_11f0 M6 umgestellt (vorher: Abnahme nach 8 VM-Bildern = genau das abgestellte
  Verhalten). Neue Erwartung mit Beleg-Kommentar @0x0171D. Legitim.

## 8. Befunde
(folgt)

## 9. Urteil
(folgt)
