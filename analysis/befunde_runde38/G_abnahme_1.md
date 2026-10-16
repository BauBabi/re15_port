# Runde 38 — Spur G "tagebuch": UNABHAENGIGE ABNAHME 1

Pruefer: Abnahme 1 (2026-10-16/17). Baum `.claude/worktrees/r38_tagebuch`, Zweig `r38/tagebuch`, HEAD 70143c31,
Basis e2cc7673. Massstab: AUFTRAG.md (Wortlaut), VERTRAG.md §1/§2, Maengel 1-3 der Abnahme 0 (`G_abnahme_0.md`).
Nichts aus der Behauptung des Bau-Agenten wird ungeprueft uebernommen. Dieser Bericht wird FORTLAUFEND geschrieben.

## Stand
- [x] Auftrag/Vertrag/Dossier/Abnahme 0/Diff gelesen (`git diff master...HEAD`, 37 Dateien; `git diff master --stat`
      zeigt zusaetzlich geloeschte Orchestrator-Dateien = master-Commits NACH der Basis e2cc7673, nicht Teil der Spur)
- [x] Bau: `local_build.sh configure` + `build` = LOCAL-BUILD-OK, "ninja: no work to do"; Arbeitsbaum sauber
      (`git status --short` leer); exe md5 e81c5047f3c0da6642f2338f75c1a5aa (17:21, letzte Engine-Aenderung fb151062
      17:08); Test-exes enthalten die Nachbesserungs-Texte (`grep -c` je 1)
- [x] Maengel 1/2 der Abnahme 0 (Alt-Riegel) gemessen + eigene Mutationsproben (MA1, MD2, MD3) + RE-Gate der neuen Belege
- [ ] Gezielte Tests
- [ ] Punkt 1 an der exe (echter Weg)
- [ ] Punkt 2 an der exe
- [ ] RE-Gate (Stichproben selbst disassembliert)
- [ ] Vertrag / Pfade / Nebenwirkungen
- [ ] Gesamturteil

Messwerkzeuge (Scratchpad `scratchpad/G1A/`, nicht im Baum): `dis.py` (eigener Aufruf von `dis_one` aus
`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` fuer PSX.EXE, RE2-PSX.EXE und DEBUG.BIN mit Basis 0x800c0000),
`mut/` (eigene Mutanten: mutierte Kopie EINER Engine-Datei, mit den Bau-Flags kompiliert und VOR `libre15_engine.a`
an das vorhandene Test-Objekt gelinkt; das Bauverzeichnis bleibt unberuehrt).

## Nachbesserung 1 — Maengel 1/2 der Abnahme 0, selbst gemessen

### Mangel 1 (unit_re2doc_bildebene) — BEHOBEN
`ctest --test-dir re15_port/build -R "^(unit_re2doc_bildebene|unit_r26_inventar)$" --timeout 600 -V`:
    252:   Gegenprobe: erster Satz ohne Titelseite im Asset-Baum = 31 (hoechster Satz RE2/Port-Tabelle = 30)
    252:   PASS: der Probe-Satz 31 liegt hinter allen RE2-Saetzen und allen Saetzen der Port-Tabelle (hoechster 30) - davor fehlt keiner
    252:   PASS: der Nachbar Satz 30 meldet sich im Lader (1) unmittelbar vor der Probe
    252:   PASS: ein nicht vorhandenes Dokument (Satz 31) meldet 0 statt Muell - sonst waere jede Zusage oben wertlos
    1/2 Test #252: unit_re2doc_bildebene ............   Passed    0.12 sec
Code gelesen (`test_re2doc_bildebene.c:147-184`): Probe-Satz = erster nn ohne `FILEnn_title_page.TIM` (fopen); drei
Bedingungen (nn hinter RE2s 25 und jedem `bildsatz` der Port-Tabelle; Nachbar nn-1 laedt mit 1; Satz nn meldet 0).
Ein fehlendes Asset-Verzeichnis macht nn = 0 -> `nn > max_satz` rot (kein stilles Gruen).
EIGENE Mutationsprobe (verschieden von M1 des Bau-Agenten): MA1 = der Lader liefert fuer eine fehlende Datei die
zuletzt geladene Kachel (`if (!data || sz <= 0) { ... return &s_cache[(s_next+N-1)%N] ... }`, "veralteter Puffer"):
    MA1: FAIL: Satz 25: p15 vorhanden, p16 nicht ...
         FAIL: ein nicht vorhandenes Dokument (Satz 31) meldet 0 statt Muell - sonst waere jede Zusage oben wertlos
         RE2DOC: FAIL (2)  exit=1
    MA0 (unveraenderte Kopie, gleicher Link-Weg): 12 PASS, exit=0
-> die neue Gegenprobe hat Zaehne; keine feste Satznummer mehr (eine kuenftige FILE31 verschiebt die Probe auf 32).

### Mangel 2 (unit_r26_inventar B4) — BEHOBEN
    324:   (B4 Vergleiche gefahren: 846, Dokument-Ids ausgenommen: 54)
    324:   (B4 Dokument-Basis aus DEBUG.BIN @0x800c7370: 0x48; Nicht-Dokument-Ids darunter: 630 gefahren, 0 als Dokument geschluckt)
    324:   [OK  ] B4 kein Kollateral in den uebrigen Skripten
    324:   [OK  ] B4b genau die Dokument-Ids der Tabelle ausgenommen (0x48..), je Schluessel einmal
    324: === probe_r26_inventar: GRUEN (0 Fehler) ===      2/2 Test #324 Passed 0.02 sec
Code gelesen (`probe_r26_inventar.c:257-335`): Basis per fopen/fseek aus `shared_assets/PSX/BIN/DEBUG.BIN` Datei 0x7370
(fehlt die Datei: basis = -1 -> `basis == 0x48` rot, kein stilles Gruen); B4 = Gleichung (unveraendert) UND
`gefahren_unter_basis == 9 x (basis - 2) = 630` UND `dokumente_unter_basis == 0`. Die Nicht-Leere bleibt (>= 630
Vergleiche immer). Feste 850 entfernt.
EIGENE Mutationsproben (`re15_files_doc_from_item` = `item_id - RE15_FILES_FIRST_ITEM_ID`, re15_files.c:110-118 —
geschluckt werden kann nur ueber diese Abbildung):
    MD2 Off-by-one der Untergrenze (0x47 Blue Master Keycard -> Dokument 0):
         (B4 ... 837, ... 63) / (... 621 gefahren, 9 als Dokument geschluckt) [FAIL] B4, [FAIL] B4b -> ROT (2 Fehler)
    MD3 Engine-Basis 0x47 bei weiter 6 Dokumenten:
         (B4 Vergleiche gefahren: 846, Dokument-Ids ausgenommen: 54)          <- Gleichung haelt (blind)
         (... 621 gefahren, 9 als Dokument geschluckt) [FAIL] B4               <- NUR die neue Pruefung faengt es
         [OK  ] B4b ...                                                        <- blind
         [FAIL] B5b (Folgefehler) -> ROT (2 Fehler)
    MD0 (unveraenderte Kopie): GRUEN (0 Fehler), exit 0
    MD1 (nur das Tabellenfeld item_id 0x4D -> 0x30): B4 GRUEN — erwartungsgemaess, denn doc_from_item liest das Feld
         nicht; das Feld bewacht unit_r38_tagebuch_dokument: `FEHLER: Dokument 5 = Item-Id 0x30` (exit 1), Kontrolle
         21/0.
Wertung: die alte Grenze (Gleichung + >= 850) war fuer eine Basisverschiebung bei 5 Dokumenten selbst blind
(9 x 95 = 855 >= 850); die neue Pruefung faengt sie unabhaengig von der Tabellengroesse. Zweck erfuellt, strenger als vorher.

### RE-Gate der neuen Belege (selbst disassembliert, Rohbytes aus info/Re1.5)
* Ladeadresse DEBUG.BIN = 0x800c0000 (Voraussetzung fuer "Datei 0x7370 = @0x800c7370"): PSX.EXE
  `80013124 ori a0,zero,0x7` / `80013128 lui a1,0x800c` / `8001312c jal 0x80013b60` (CD_read(Datei 7, 0x800c0000)) /
  `80013130 ori a2,zero,0x0`; Dateitabelle @0x8006F43C Eintrag 7 @0x8006f474 = `00 00 04 00 ...` = 262144 B =
  Groesse von DEBUG.BIN (md5 c2c11aab... in info/ == shared_assets/). -> roh, ohne Kopf: Datei-Offset = Adresse - 0x800c0000.
* DEBUG.BIN: `800c72d0 lui v0,0x800c` / `800c72d4 lbu v0,0x6c95(v0)` (Seite) / `800c72dc addiu v1,v1,0x7370` /
  `800c72f4 addu v1,v1,v0` / **`800c72f8 lbu s1,0(v1)`** / `800c7304 andi t0,s3,0x1` / `800c7308 bne t0,zero,0x800c7320`
  / **`800c7320 jal 0x80028840`** + `800c7324 addu a0,s1,zero` / `800c7344 ori v0,zero,0xa` / `800c734c bne s0,v0,
  0x800c7304` / **`800c7350 addiu s1,s1,1`** (Delay-Slot); Datei 0x7370 = **`48 52 5c 00`**, dahinter `f0ff bd27`
  (Prolog der naechsten Funktion). -> 3 FILE-Seiten x 10 Zeilen, Zeile k = Name der Id basis+k (Bit k der Maske
  u16 @0x800c6c98+2*Seite, sonst Platzhalter @0x800c7916).
* PSX.EXE `80028840 andi a0,a0,0xff` / `8002884c addiu at,at,0x495c` / `80028854 lhu v1,0(at)` / `8002885c addiu
  v0,v0,0x4a28` / `80028864 addu v0,v1,v0` = Name(Id) aus u16 @0x800c495c + Blob @0x800c4a28. Selbst dekodiert:
  0x46 "Red Master Keycard", 0x47 "Blue Master Keycard", 0x48 "Chris' Diary", 0x49 "Operation Report", 0x4a..0x51
  "File 3".."File 10", 102 Namen.
* PSX.EXE Klemme: `8004a348 lbu v0,0(v1)` (v1 = 0x800b2669) / **`8004a350 sltiu v0,v0,0x48`** / `8004a354 bne v0,zero,
  0x8004a360` / **`8004a358 ori v0,zero,0x47`** / `8004a35c sb v0,0(v1)`. Kontext (ghidra1_V2.txt Z. 163380-163540):
  Debug-Itemwaehler — Zustand 1 laedt Datei 0xb (ITEMALL.PIX) nach 0x801a0000 (@0x8004a194-9c), Zustand 2 zeichnet Bild
  Id x 1200 (@0x8004a1bc-d8), Zustand 3 +-1/+-10 und Klemme auf 0x47. ITEMALL.PIX 86400 B = 72 x 1200, ITPS.ITP 884736
  B = 72 x 12288 -> 72 = 0x48 Items. BESTAETIGT.
