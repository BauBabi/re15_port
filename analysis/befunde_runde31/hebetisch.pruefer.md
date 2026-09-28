# Gegenpruefung r31/hebetisch (ROOM1170-Hebetisch-Items + ROOM11F0-Generatorzeiger)

Pruefer-Dossier. Stand: laufend gefuellt. Pruefer aendert KEINEN Spielcode.

## 0. Auftrag (Nutzer, woertlich, relevanter Teil)
"Außerde baue mir danach room 1170 die items im model an, und packe mir die Granate links in die
hochfahrende Box und die Sicherung rechts. Außerdem starte mit dem Aufnahme Dialog der items erst
wenn das Modell wirklich komplett hochgefahren ist. gleiches in room 11f0 bei den generator rätsel.
warte erst bis der zeiger final auf 80 steht, bevor du mit ok das abnimmst, das Licht anschaltest etc."

Pruefpunkte:
- P1 Items (Granate links, Sicherung rechts) sichtbar IM Modell der hochfahrenden Box ROOM1170
- P2 Aufnahme-Dialog erst, wenn Modell komplett oben
- P3 ROOM11F0: Zeiger final auf 80, erst dann OK/Abnahme/Licht
- P4 Faelle, die der Bauer nicht gefahren ist (Lade-Weg, zweite Fahrt, No, xxx1, Wiedereintritt, Save/Load)
- P5 Konstanten mit Beleg (@0x / Datei-Offset / PORT-WAHL)
- P6 Tests pruefen Ergebnis, nicht nur Absicht
- P7 Regressionen Nachbarthemen

## 1. Bauer-Stand (Commits, Dossier)

Basis 7cb74897 (merge-base mit r30/integration; r30/integration ist inzwischen auf cade0796 weiter,
die release/-Differenz in `git diff r30/integration..HEAD` stammt von dort, nicht vom Bauer).
Bauer-Commits: 4ada307e, 424177f2, a380704a, 12a30af9, 9595b574, 137bd422 (Dossier
`analysis/befunde_runde31/hebetisch.md`, 409 Zeilen, gelesen).

Spielcode-Aenderung (git diff 7cb74897..HEAD -- re15_port/engine re15_port/include):
* neu `engine/src/hebetisch_1150.c` + `include/re15_hebetisch.h`: 56-Byte-Signatur-Suche im
  Raum-RDT (gerufen aus `scd_register_current_rdt`), `re15_hebetisch_ruht_oben()` = irgendein
  aktiver SCD-Thread mit PC in [Sig+0x0A, Sig+0x32) = ROOM1150 [@0x101A,@0x1042).
* `sicherung_1150.c`/`granate_1150.c`: y-Schranke `py > OBEN_BIS(-1100)` ersetzt durch
  `!re15_hebetisch_ruht_oben()`.
* Sitze: Sicherung (-280,-1062,1280) rot_y 1440 (vorher 1260/1024), Granate (-260,-1091,1140)
  rot_y 1792 (vorher -362/-1088/1260/1024). Alle als PORT-WAHL gekennzeichnet, Herleitung
  per Suchwerkzeug aus Geometrie-Bytes.
* `game_step_common.c`: nur Mess-Protokoll-Aufruf (RE15_HEBETISCH_LOG).

Geltungsbereich: Dieses Thema ist NUR "H" (Hebetisch). "G" (ROOM11F0 Zeiger auf 80) liegt im
Zweig r31/generator und wird hier NICHT geprueft. "room 1170" im Auftrag = ROOM1150/1151
(Runde 30 vom Nutzer bestaetigt, AUFTRAG.md Lesart H).

## 2. Bau + Suite
(folgt)

## 3. Stichproben Bytes/Adressen

### 3.1 sub04-Bytes (xxd re15_port/shared_assets/PSX/STAGE1/ROOM1150.RDT)
```
00000ff0: 0000 2f01 f6ff 0d00 0400 5b00 3002 0e00   Speed_set vy -10 @0xFF2, For 91 @0xFF6
00001000: 3602 0d00 0300 ...        2f01 0100       Se_on 0x0d @0x1000, Speed_set +1 @0x100C
00001010: 0d00 0400 0a00 3002 0e00 090a 1e00 2e03   For 10 @0x1010, Add_speed, Evt_next, Next, Sleep 30 @0x101A
00001020: 0000 3602 0a00 ...             090a       Work_set 0, Se_on 0x0a @0x1022, Sleep @0x102E
00001030: 0a00 3602 0c00 ...             2f01       Sleep 10, Se_on 0x0c @0x1032, Speed_set @0x103E
00001040: 0a00 0d00 0400 5a00 3002 0e00             vy +10, For 90 @0x1042 (Abfahrt)
```
Stimmt Byte fuer Byte mit Dossier §2.1 und `s_sig[56]` in hebetisch_1150.c. ROOM1151.RDT: dieselbe
Folge ab @0x0FEE (`0d00 0400 0a00 3002 0e00 090a 1e00 2e03 0000 3602 0a00 ...`), Fenster
[0x0FF8, 0x1020) - bestaetigt.
Vor dem Hub: For 15 @0x0FC0 (Work_set 1/2, Deckel je +-10 x 15 = 150), Sleep 30 @0x0FDE, Se_on
0x0c, Work_set 0 - die Deckel sind VOR dem Hub fertig offen; waehrend Hub/Setzen bewegt nur
Work_set 0 (Plattform). Also ruht zwischen @0x1018 und @0x1048 WIRKLICH das ganze Modell.

### 3.2 Freeze-/Sleep-Mechanik (re15_disasm.py, info/Re1.5/PSX.EXE)
```
8003f040: lw v0,-13760(v0)   g_pauseflags     8003f044: lui v1,0x200
8003f048: and v0,v0,v1                        8003f04c: bne v0,zero,0x8003f090   (Skript-Schritt ueberspringen)
8001db94: addiu t1,t1,-13760 g_pauseflags     8001db98: lui t0,0xff00
8001dbb8: or v0,v0,t0                         8001dbc8: sw v0,0(t1)              (|= 0xFF000000)
8003f3e8: addiu v0,a2,1 (PC+1)  8003f3f8: sw v0,28(a0)  8003f414: lhu a0,2(a2) (Dauer)  8003f424: ori v0,zero,0x1
8003f454: addiu v0,v0,-1  8003f460: bne v0,zero,0x8003f488  8003f470: addiu v0,v0,3  8003f48c: ori v0,zero,0x2
```
Alle vom Bauer zitierten Adressen/Instruktionen stimmen. PC im Sleeping = Sleep+1 = 0x101B
(= Logzeile `sub04-PC @0x101B`).

## 4. Sichtpruefung echtes Spiel (Framedump)
(folgt)

## 5. Nicht gefahrene Faelle
(folgt)

## 6. Befunde
(folgt)

## 7. Urteil
(folgt)
