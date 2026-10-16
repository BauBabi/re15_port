# Runde 38 — Spur H "elzatoene": Abnahme 0 (unabhaengig)

Datum: 2026-10-16. Baum `.claude/worktrees/r38_elzatoene`, Zweig `r38/elzatoene`, Stand 8328ae1f (Basis e2cc7673).
Pruefer: Abnahme 0 (kein Code geaendert; nur diese Datei committet).
Messordner (ausserhalb des Baums): Scratchpad `ab0h/` (Laeufer `lauf.sh`, eigene Auswertung `py/kreuz.py`,
v0.8.25-Paket-Kopie `v0825/` aus `release/pkg-win/re15_port_v0.8.25`, Laeufe `runs/<name>/`).

## 0. Stand / Bau
- `git log master..HEAD`: 10 Commits (1468730f .. 8328ae1f). master ist seit e2cc7673 nur um Doku-/Orchestrator-Commits
  gewachsen (1bcbd0ff, 98d35560); die Spur-Aenderungen = `git diff master...HEAD` (19 Dateien, +1194/-29).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work
  to do`, `LOCAL-BUILD-OK (build)`. exe 17:18, letzter Code-Commit c08a9a47 (17:19; spaetere Commits nur Doku/Tests,
  `git log -1 -- engine platform include shared_assets` = c08a9a47); die exe enthaelt die Zeichenketten
  `[elzatoene] ROOM%04X Anker @0x%05X -> Aktion %u` (strings). ELZASE.VBS md5 ba96a66267286e4754c6b5a7481928a4,
  14232 B (= Dossier). v0.8.25-Paket: ELZASE.VBS md5 8e996ef2... (13144 B) = master = e2cc7673 (git show).
- ctest -N: 796 Tests (Suite v0.8.25 = 788, +8 = 6 Unit + 2 Integration, wie im Dossier).

## 1. Code-Diff (gelesen)
- `engine/src/elzatoene_r38.c` + `include/re15_elzatoene_r38.h` (neu): Anker-Tabelle (Raum, Datei-Offset, 8-Byte-
  Signatur, Aktion); Scan beim Raumladen (`scd_register_current_rdt`), Opcode-Haken vor jedem Opcode (nur Aktion 2),
  Message_on-Haken (nur Aktion 1: Ton statt Text, `pc += 4`). Log nur ueber `RE15_ELZA_LOG` (Messschiene, kein Verhalten).
- `scd_vm.c`: Include + 3 Haken-Zeilen + Kommentare (Raum-Scan, Opcode-Schleife, op_message_on HINTER dem
  Stimmen-Riegel und `re15_leiche_message_on`). Klein, wie VERTRAG §1.4 erlaubt.
- `tools/r36_l/re2_elza_se_cut.py`, `gen/re2_elza_bank.inc`, `shared_assets/RE2/ELZASE.VBS`: Satz 1 SCHUSS ROOM40A0 0x08
  -> ROOM4040 0x0A; VBD 10000 -> 11088 B (= 5264 + 5824).
- `elza_se_pc.c`, `re15_elza_r36.h`: nur Kommentare.
- Zur Wiederholung des Opcode-Haken: `op_plc_dest` (scd_vm.c:2514) gibt immer 1 zurueck (kein Parken) -> der Gang-Anker
  feuert genau einmal; der Message_on-Haken liegt hinter dem parkenden Stimmen-Riegel und feuert erst beim Durchlauf.

(Bericht wird fortlaufend ergaenzt.)
