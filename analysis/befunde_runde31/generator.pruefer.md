# Gegenpruefung Runde 31 — Thema "generator" (ROOM11F0/11F1 Generator-Raetsel)

Pruefer-Dossier. Zweig r31/generator, Arbeitsbaum .claude/worktrees/r31_generator.
Gemessen wird der GEBAUTE Stand gegen den Nutzerauftrag, nicht gegen die Absicht des Bauers.

Nutzerauftrag (Teil dieses Themas):
> "... gleiches in room 11f0 bei den generator raetsel. warte erst bis der zeiger final auf 80 steht,
> bevor du mit ok das abnimmst, das Licht anschaltest etc."
("gleiches" bezieht sich auf "starte mit dem Aufnahme Dialog der items erst, wenn das Modell wirklich komplett hochgefahren ist" — also: erst das Ende der Bewegung, dann der Dialog.)

## 0. Status (laufend)
- [x] Dossier des Bauers gelesen
- [x] Commits gesichtet (git log r30/integration..HEAD)
- [x] Bau + Suite (local_build.sh all) — 406/406
- [x] Byte-/Adress-Stichproben — alle bestaetigt
- [x] Nutzerszenario im echten Spiel (Framedump), Bilder angesehen — P1 (Reihenfolge des Bauers),
      P2 (andere Reihenfolge, 90 -> 80 von oben, Tastenhaemmern in Ruhe + Abnahme)
- [x] Nicht gefahrene Faelle (No, Wiedereintritt, Save/Load, zweite Fahrt, Raumvariante) — §6
- [x] Tests: pruefen sie Verhalten oder nur Absicht? — Verhalten, §7
- [x] Regressionen Nachbarthemen — §6/§8
- [x] Urteil: haltbar (§9)

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
`bash re15_port/tools/local_build.sh all` im Baum r31_generator auf 80c71bd2 (Code-Stand bffb34b9):
configure OK, `ninja: no work to do` (build/ stammt vom Bauer auf demselben Code; kein Quelltext
seit bffb34b9 geaendert), Abschluss `test OK — 406/406 bestanden` /
`=== LOCAL-BUILD-OK (all) — Tests 406/406`, EXIT=0 (Log build/r31_generator/pruefer_build_all.log,
nicht committet). Kein GUI-Test flatterte.

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
Echte exe, beschleunigter Renderer, RE15_FRAMEDUMP (Readback vor Present), Serie jedes 2. Bild.
Skript generator_pruefer_belege/lauf_p.sh (DEBUG_JUMP 11F0 ueber das Debug-Menue, FIRE_AOT Slot 1
= Panel untersuchen, Meldungen blaettern, "Ja", dann echte Cursor-Fahrt + Quadrat je Schalter).

MESSFALLE (nicht dem Bauer anzulasten): die ersten zwei P1-Laeufe endeten mit EXIT=1 bei F30 bzw.
F1332, kein Absturzeintrag im Ereignisprotokoll. Ursache: re15_port/tools/local_build.sh:280
`taskkill //F //IM re15_pc.exe` — jeder Bau eines parallelen Agenten beendet ALLE re15_pc.exe der
Maschine, auch fremde Messlaeufe. Abhilfe: exe-Kopie unter eigenem Namen (re15_pr31gen.exe, per
cmp bitgleich) — danach alle Laeufe EXIT=0.

### P1 — Reihenfolge des Bauers (7, 9, 3, 1, 5; letzter Schritt 60 -> 80 von unten)
Panel-Protokoll F1180..F1236 BITGLEICH zum Beleg des Bauers (diff leer) —
generator_pruefer_belege/p1_panel_F1180-1340.log:
- F1183 letztes Schalterbit (maske 155), Sperre sofort; F1202 wert=80 (= k); F1203..F1232 ruhe
  1..30, geloest=0, msg=0; **F1233 geloest=1, strom=1, msg=1, Tonzaehler 1 = k+31**; F1234 Cut 8.
- Nach Quadrat in F1273 (Meldung weg): F1275 Cut 0x0D, F1287 Cut 0x0E (Licht), F1327 Cut 8.
Bilder (angesehen): p1_a.png — F1184 Zeiger bei 62 und faehrt; F1202 Zeiger an der roten 80;
F1232 Zeiger an der 80, keine Meldung; F1234 Raumansicht Cut 8, Meldung beginnt ("P").
p1_b.png — F1260 "Power supply O" tippt in Cut 8; F1276/F1290 Cut 13/14 = Licht-Montage (gegen die
Original-Hintergruende extracted/PSX/STAGE1/ROOM11F/ROOM11F13.bmp und ROOM11F14.bmp geprueft: das
Mosaik IST die Original-Kunst, kein Renderfehler); F1330 zurueck in Cut 8.

### P2 — andere Reihenfolge (7, 9, 1, 5 = 90, dann 3 -> 80 VON OBEN) + Tastenhaemmern
Eingaben generator_pruefer_belege/p2_eingaben.log, Protokoll p2_panel_F1040-1320.log:
- Maske 040 (F532) -> 140 (F682) -> 141 (F883) -> 151 = 90 (F1055) -> 155 (F1205, Schalter 3);
  Zeiger faehrt 89..80, **F1214 wert=80 (= k)**.
- Ab F1214 alle 4 Bilder UNTEN+QUADRAT (Tasten 0x8040) bis F1310 — ueber die ganzen 30 Ruhebilder
  und die Abnahme hinweg. Ergebnis: Maske bleibt 155 (kein Schalter kippt), Cursor bleibt auf
  Schalter 3 (Bild), **F1245 Abnahme = 1214+31**, F1246 Cut 8, F1262 "Power supply OK." ganz
  sichtbar, F1263 Meldung durch das Haemmern weg, F1265 Cut 0x0D, F1277 Cut 0x0E, F1317 Cut 8.
Bilder (angesehen): p2_a.png — F1206 Zeiger 88 ueber der 80, faehrt abwaerts; F1214/F1244 Zeiger an
der 80 und Cursor unveraendert auf Schalter 3 trotz gehaltenem UNTEN; F1246 Cut 8 "P";
F1262 "Power supply OK.".

Beobachtung (kosmetisch, vorbestehend): das Bild haengt dem protokollierten Wert 1 Bild nach
(F1202-Bild zeigt 79, F1214-Bild zeigt 81; sichtbar auf 80 ab F1203 bzw. F1215). Der sichtbare
Stillstand auf der 80 vor dem OK betraegt damit 30 Bilder = 1 s.

## 6. Nicht gefahrene Faelle
- **Anfahrt von oben** (90 -> 80): live gefahren (P2), korrekt k+31.
- **Eingabe waehrend Ruhe/Abnahme**: live gefahren (P2), Sperre haelt, kein Schalterwechsel.
- **"Nein" / zweite Fahrt**: sub16 @0x015AE Ifel_ck, @0x015B2 Ck(12,31)==0 — bei "Nein" wird
  nichts gesetzt (5:0 bleibt 0 -> keine Sperre). Nach dem Loesen macht sub18 @0x01776
  `46 01 01 31 ...` Aot_reset Slot 1 zu reinem Text -> keine zweite Fahrt moeglich.
- **EXIT mitten im Raetsel**: EXIT ist selbst Sce_key_ck 0x40 (sub01 @0x01296, Member 0x0C ->
  sub17) und liegt ausserhalb 0xf000 -> waehrend Fahrt/Ruhe gesperrt; danach loescht sub17
  @0x0168C..@0x016E4 Bank 5 Bits 0..22 -> 5:0 = 0 -> Sperre aus. Kein Haenger.
- **Dauer-Sperre "80 ohne SCD-Loesung"**: ausgeschlossen, 80 nur bei Maske 0x155 (§4).
- **Save/Load**: Save-Punkte (Telefone) nur in 1070/1120/1150/2010/30A0/30B0/4010/4011 (Memory
  reai-v2-save-load) — in 11F0/11F1 kein Speichern; Laden fuehrt in einen anderen Raum ->
  re15_panel_zeiger_tick setzt im ersten Bild ausserhalb 11F0/11F1 zurueck. Bank 5 ist
  raumlokal (Raumaufbau loescht sie). Kein Pfad traegt halbe Raetselzustaende mit.
- **Wiedereintritt nach Loesung**: Riegel E (Ziel fest 80 ab 4:238, keine Sperre, kein 2. Ton);
  live nicht gefahren. Logik: sperrt() bricht bei 4:238=1 ab, Tick setzt Ziel 80.
- **ROOM11F1 (Elza)**: live NICHT fahrbar — RE15_DEBUG_JUMP ignoriert das Varianten-Nibble
  (debug_menu_common.c:318 `want = room_id >> 4`). Nur Riegel F + cmp (byte-identisch).
- **PSX-Ziel**: nicht gebaut (weder Bauer noch ich). Aenderung nur in gemeinsamen Engine-Dateien.
- **Nachbarthemen/Integration**: `git merge-tree` HEAD gegen r30/integration und r31/tueren
  konfliktfrei; gegen r31/hebetisch EIN Konflikt in re15_port/tools/local_build.sh (beide heben
  RE15_MIN_TESTS 405 -> 406) — bei der Zusammenfuehrung 405 + Summe der neuen Tests setzen.
  game_step_common.c und scd_vm.c mischen automatisch. Beide Haken sind raumgebunden
  (11F0/11F1 + pc-Offset + Ankerbytes) und in allen anderen Raeumen wirkungslos.

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
Kein Befund gegen den Nutzerauftrag. Nebenbefunde (alle niedrig):
1. Messfalle local_build.sh:280 `taskkill //F //IM re15_pc.exe` beendet fremde Messlaeufe
   (gemessen: 2 abgebrochene P1-Laeufe). Infrastruktur, nicht Thema G.
2. Merge-Konflikt local_build.sh RE15_MIN_TESTS mit r31/hebetisch (trivial).
3. Sperre nach JEDEM Schalter (Fahrt |Delta| + 30 Ruhebilder, nach 16 Bildern Kippung; z.B. Schalter 9:
   F682..F740 = 59 Bilder gesperrt, P2-Protokoll) — RE2-belegt (@0x012A4 + Bank 2 Bit 7 ueber ganz sub04),
   vom Bauer unter "Offen" benannt, geht aber ueber den woertlichen Auftrag hinaus; der Nutzer
   koennte das Panel als traege empfinden.
4. ROOM11F1 und Wiedereintritt nur im Riegel, nicht live; PSX nicht gebaut.

## 9. Urteil
**haltbar.** Der Nutzer sieht im echten Spiel (zwei unabhaengige Schalter-Reihenfolgen, Anfahrt
von unten und von oben): Zeiger faehrt zur 80, steht dort sichtbar 30 Bilder (1 s, RE2 sleep 0x1E
@0x0171C/@0x0171D) ohne Reaktion, Tasten verstellen nichts, DANN erst die Abnahme — Kamera Cut 8,
"Power supply OK." und Bestaetigungston im selben Bild (Zaehler), nach dem Bestaetigen das Licht
(Cut 0x0D/0x0E, Original-Montage). Vorher kam die Abnahme bei Zeigerwert 62. Alle tragenden
Bytes/Adressen nachgelesen und bestaetigt; Suite 406/406.
