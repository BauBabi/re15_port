# Release v0.8.20 — Dossier (laufend fortgeschrieben)

Arbeitsbaum: `.claude/worktrees/r34n_integration`, Zweig `r34n/integration`, Basis master `7d4d11dd`.
Inhalt: Runde 34 Nacht, sieben Spuren A..G (Rolltor-Sicherung 1050, Hebetisch-Cursor 1150, Generator 11F0,
Ada-Ruf 1050->10A0, vier Dokumente, Leichen-Munition 1110/1230, Leuchtschrift HEAVEN 1150 ueber Opcode 0x45).
Vorbild: `analysis/befunde_runde33/release_v0819.md`.
Absprache: v0.8.20 = diese Runde (Variante (b): heutige master-Kette), v0.8.21 = Granaten (reai-v2-b5),
r34a/android-gate (reai-v2-4b) merged master nach dem Tag.

NEU in v0.8.20 (Assets, nur hinzugefuegt, 0 geaendert): `re15_port/shared_assets/RE2/FILES/FILE26..29_*.TIM`
(23 Dateien) + `re15_port/shared_assets/RE2/LAMPE2130.TIM` = 24 Dateien.

## Ablauf
- [x] 0. Integration: 7 Spuren gemerged, Suite 463/463 (local_build.sh all, ein Durchgang), RE15_MIN_TESTS 463
- [ ] 1. Windows-Cross, Linux/Deck, Android bauen (Kette `build/rel0820/kette.sh`, Python-Shim vor PATH)
- [ ] 2. make_package.sh --version v0.8.20
- [ ] 3. Checkliste am ausgelieferten Artefakt
- [ ] 4. Archiv
- [ ] 5. Release-Commit, master (ff), Tag, Push
- [ ] 6. Hauptbaum-exe neu gebaut

## Protokoll

### 0. Integration
- Merges: rolltor, hebetisch (Konflikt Include-Zeile scd_vm.c), generator, dokumente (Include + Install-Haken
  scd_room_setup.c/main.c, beide behalten), leichen (Include), schrift1170 (Opcode-Tabelle: 0x62 von A und 0x45
  von G2 beide), adaruf (AUFTRAG-Nachtraege beide; scd_event_fire: Umleitung A = Ereignis 2 und D = Ereignis 13
  nacheinander — die einzige echte Zusammenfuehrung).
- local_build.sh auf master 7d4d11dd zurueck (PATH-Zusatz a2c3ec22 der Spur C ueberholt), RE15_MIN_TESTS 463.
- Suite 1 (A+B+C+E+F): 453/453; Suite 2 (alle sieben): **463/463** in 1115 s, ein Durchgang.
