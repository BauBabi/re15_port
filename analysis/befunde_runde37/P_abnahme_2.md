# Abnahme 2 — Spur P masken36 (Runde 37, 2026-10-16)

Unabhaengige Abnahme nach Nachbesserung 2. Baum `.claude/worktrees/r37_masken36` (Zweig r37/masken36, Basis bddd6b31,
HEAD 2cb1ad19). Gemessen nur am selbst gebauten Stand (`local_build.sh configure` + `build` -> "ninja: no work to do",
`=== LOCAL-BUILD-OK (build)`) mit eigenen exe-Kopien `re15_pc_abnP2_*.exe` und eigenem Scratchpad `abnP2/`.

## Stand der Abnahme (fortlaufend)

- [x] Pflichtlektuere: AUFTRAG.md (Orchestrator-Auftrag P), VERTRAG.md, Dossier P_masken36.md inkl. NB1/NB2,
      P_abnahme_0.md/P_abnahme_1.md, `git log master..HEAD` (57 Commits), `git diff master...HEAD` (Code gelesen)
- [x] Bau (§1)
- [ ] gezielte Tests (§2)
- [x] RE-Gate (§3)
- [ ] Messung an der exe (§4)
- [x] Vertrag (§5)
- [ ] Urteil (§6), Maengel (§7)

Scratchpad dieser Abnahme (nicht im Baum): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/
c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abnP2/`.

## 1. Bau und Code-Diff

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)` (Log `abnP2/build.log`). `git status --short` leer -> die exe entspricht HEAD.
- Tabelle in der exe nachgeprueft (eigenes Skript `abnP2/py/tab_in_exe.py`: die .inc-Zeilen als
  `struct {u16 raum; u8 cut; u8 anzahl; u32 bits[4]}` gepackt und in der exe gesucht): **re15_pc.exe enthaelt die HEAD-Tabelle
  (126 Zeilen + Endzeile, 1444 Bits, anzahl == popcount in jeder Zeile) bei Datei-Offset 0x49D9E0.** Die Vergleichs-exe
  des Bau-Agenten `re15_pc_nb2gegen.exe` enthaelt an DERSELBEN Stelle die NB1-Tabelle von fb4be428 (124 Zeilen, 1224 Bits),
  `re15_pc_nb2alt.exe` die NB1-Tabelle an anderer Stelle (Stand vor dem master-Merge). Kennungs-Strings: re15_pc.exe und
  nb2gegen tragen "Runde 37 Spur P" und "[kistenregel]" (gemergter Code), nb2alt nur "Spur P", die Hauptbaum-exe
  (v0.8.25, Code == e2cc7673 == Merge-Basis, `git diff v0.8.25 e2cc7673 -- re15_port` leer) nur "[kistenregel]".
- Laufzeit-Code seit Abnahme 1 (fb4be428): nur die Tabelle `engine/src/gen/masken36_ausschluss_r37.inc` (124 -> 126 Zeilen,
  1224 -> 1444 Bits; neu u.a. `{ 0x3050, 15, 36, { 0xFFFFFFFF, 0x0000000F, ... } }` und dasselbe fuer 0x3051) und ein
  Kommentar in `masken36_r37.c`. `bg_pc.c`, `platform/pc/main.c`, `platform/psx/main.c`, `re15_masken36_r37.h` unveraendert
  seit Abnahme 0. Generator/Riegel `tests/unit/test_r37_masken36.c` (+518): `figur_raster` pixelgenau (Spaltenzahl =
  Kastenbreite in Bildpunkten, je Spalte jede Bildzeile Kopf..Fuss), Grobsprung ueber Summentabelle, Schranken fuer
  "Kasten breiter als das Bild" und "Spalte halb hinter der Kamera", `sektion_pruefen`: Sektion ohne Stehplatz -> alle
  Rechtecke aus; neue Checks (NB2 M1/M2), Werkzeuge `punkt`/`zeuge`. Neu `tests/integration/test_r37_masken36_nb2.cmake`
  (registriert in `probes/r37_masken36.cmake`).
- master ist seit dem Merge 2bb1bbe3 nur um Doku-Commits weiter (98d35560, cb782bda; `git diff HEAD...master --name-only`
  nur `analysis/`); `git merge-tree --write-tree master HEAD` -> exit 0, ohne Konflikt.

## 3. RE-Gate (selbst disassembliert: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE =
Auslieferungsstand, `@0x80026e4c = 08 00 e0 03 21 10 00 00`)

| zitiert (NB2) | gefunden | haelt |
|---|---|---|
| FUN_800392d4 @0x80039324 lw / @0x8003932c bne -1 / @0x80039338 sb zero = "keine Maske" | `800392d8 lw a0,-14472(a0)` (RDT-Zeiger 0x800ac778), `800392e0 lh v0,4068(v0)` (Cut 0x800b0fe4), `80039310 lw v1,36(a0)` (RDT+0x24), `80039314 sll v0,v0,5`, `8003931c lw t5,28(v0)` (+0x1C), `80039324 lw v1,0(t5)`, `80039328 addiu v0,zero,-1`, `8003932c bne v1,v0,0x8003933c`, `80039330 srl t2,v1,16` (Delay), `80039334 j 0x8003955c`, `80039338 sb zero,0(a0)` (Delay: RDT-Byte 0 = Maskenzahl := 0); Bytes `00 00 a3 8d ff ff 02 24 03 00 62 14 02 54 03 00 57 e5 00 08 00 00 80 a0`. **Sprungziel 0x8003955c = Epilog** (`lw ra,100(sp)` ... `addiu sp,sp,104`, `jr ra`). Sonst `80039358 sb t2,0(a0)` (Zahl = obere Haelfte des ersten Worts) | ja |
| ROOM3050/3051 Cut 15: Kamera-Eintrag +0x25C -> 0x84C = FF FF FF FF | eigenes Skript `abnP2/py/rdt_cut_mask.py`: RDT+0x24 = 0x60, Cut 15 Eintrag @0x240, +0x1C @0x25C = 0x84C, Wort dort `ff ff ff ff` - in ROOM3050.RDT und ROOM3051.RDT (shared_assets, bytegleich zu info/Re1.5/PSX/STAGE3/ROOM3050.RDT, `cmp`) | ja - **aber nicht Cut-15-spezifisch:** in ROOM3050/3051 sind ALLE 16 Cuts NULL-Sektionen (0x810..0x84C je `ff ff ff ff`); jede Generator-Sektion liegt per Konstruktion auf einer NULL-Sektion (Zensus §1.1 Dossier "Original nicht NULL 0"). Der NULL-Beleg zeigt, dass "Cut 15 ganz aus" dem Original entspricht - die Auswahl GERADE dieses Cuts traegt allein die Port-Regel "Sektion ohne Stehplatz" (s. §6, Hinweis H1) |
| Atlas-Kette (Kopf re15_masken36_r37.h, unveraendert seit Abnahme 0) | `80021de8 ori v0,zero,0x15` + `80021dec jal 0x8004ee78` + `80021df0 sh v0,0(s0)` (s0 = 0x800aca4c); FUN_8004ee78: `8004eea8 sll a0,v1,6`, `8004eeb8 addiu a0,a0,-1024` (x = 320), `8004eed0 sll v0,v0,8` (y = 256), `8004eee0 jal 0x80068c88` (= LoadImage, ghidra1_V2.txt Z. 211128/211156); `80039630 ori a3,zero,0x95` + `80039634 jal 0x80069858` (= SetDrawMode, Z. 212249/212278) | ja |
| SCD ROOM3050 (Aufbau der Messung): K8 @0x01F90, Pruef-AOT @0x01FB4, Cursor @0x01FDC, Cut_chg @0x02596, sub11..14 | `21 03 b2 00` Ck(3,0xB2)=0; `2c 0a 03 31 .. d4 9a fc bd e8 03 d0 07 ff 00 18 0a` (Slot 10, (-25900,-16900) 1000x2000, Ereignis 0x0A); `2d 03 04 .. 9e b3 00 00 9c 58` Obj 3 Typ 4 (-19554,0,22684); `29 0f`; `2e 03 03 00 2f 02 c8 00 30 02 01 00` / `.. 2f 02 38 ff ..` / `.. 2f 00 c8 00 ..` / `.. 2f 00 38 ff ..` | ja |

Konstanten im Diff: neu sind nur Generator-/Messgroessen im Riegel (SP_MAX 321 = Bildbreite + 1, Messaufbau; Schwelle 24,
Fenster); FIGUR_HOCH 3000 / FIGUR_HALB 350 (an der exe gemessen, Dossier §3.3) und PR 450 (= DAT_80073e94[6], in Abnahme 1
nachgeprueft) unveraendert. Die Tabelle selbst ist Port-Massnahme auf NUTZER-VORGABE-Masken (R15M), im Kopf von
masken36_r37.c so begruendet, nicht als Original verkauft. Rate-Woerter (deferred|tunable|interim|for now|faithful|plausib|
TODO|vorlaeufig) in den +Zeilen von re15_port: **0**; in analysis/ nur die Meta-Erwaehnungen der frueheren Abnahmen.
Neue Env-Schalter (`getenv(` in +Zeilen): **0**.

## 5. Vertrag (statisch)

- Pfade (`git diff master...HEAD --name-only`, 16 Dateien): analysis/befunde_runde37/P_* (Dossier, Abnahmen, alle_cuts.txt),
  engine/src/masken36_r37.c, engine/src/gen/masken36_ausschluss_r37.inc, include/re15_masken36_r37.h, platform/pc/src/bg_pc.c
  (Masken-Lader: VERTRAG §1.4 erlaubt P genau das), platform/pc/main.c + platform/psx/main.c (je 2 Zeilen, Kommentar
  "Runde 37 Spur P"), tests/unit/probes/r37_masken36.cmake, tests/unit/test_r37_masken36.c + _szenen.inc,
  tests/unit/probe_r37_masken36_ppm.c, tests/integration/test_r37_masken36_verdeckung.cmake + _nb2.cmake (beide mit
  `cmake_minimum_required(VERSION 3.16)`). Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists,
  keine MASKS-Dateien. (`git diff master --name-only` zeigt zusaetzlich analysis/befunde_runde38/* - das sind die drei
  Doku-Commits, die master voraus ist, nicht Arbeit der Spur.)
- Keine Bank-9-Bits, Nachrichten-Ids, Ereignisse, Slots. Kein popen/_DEFAULT_SOURCE noetig (keine POSIX-Aufrufe in den
  neuen Tests).
