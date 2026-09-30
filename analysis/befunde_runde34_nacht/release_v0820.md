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
- [x] 1. Windows-Cross, Linux/Deck, Android bauen (Kette `build/rel0820/kette.sh`, Python-Shim vor PATH)
- [x] 2. make_package.sh --version v0.8.20
- [x] 3. Checkliste am ausgelieferten Artefakt
- [x] 4. Archiv
- [x] 5. Release-Commit b998f6c3 + Tag v0.8.20 (master ff + Push folgen im Hauptbaum)
- [ ] 6. Hauptbaum-exe neu gebaut

## Protokoll

### 0. Integration
- Merges: rolltor, hebetisch (Konflikt Include-Zeile scd_vm.c), generator, dokumente (Include + Install-Haken
  scd_room_setup.c/main.c, beide behalten), leichen (Include), schrift1170 (Opcode-Tabelle: 0x62 von A und 0x45
  von G2 beide), adaruf (AUFTRAG-Nachtraege beide; scd_event_fire: Umleitung A = Ereignis 2 und D = Ereignis 13
  nacheinander — die einzige echte Zusammenfuehrung).
- local_build.sh auf master 7d4d11dd zurueck (PATH-Zusatz a2c3ec22 der Spur C ueberholt), RE15_MIN_TESTS 463.
- Suite 1 (A+B+C+E+F): 453/453; Suite 2 (alle sieben): **463/463** in 1115 s, ein Durchgang.

### 1. Bauten (Kette 12:57, Logs `build/rel0820/{win,linux,android}.log` mit eigener EXIT-Zeile)
- Letzter Port-Commit f66fb773 (12:10:40) -> Frische-Schranke; alle Binaries danach gebaut.
- Windows-Cross 12:57:36 -> 12:59:36, EXIT=0, `WIN-CROSS-BUILD-OK: 4573593 Bytes`; `release/win_out/re15_pc.exe`
  sha256 `18daca847dd690a20c3d1eaf172106addd76f383263c43f97284001531cc88b6`; PE Magic 0x20b, Subsystem 2.
  Groesse gegen v0.8.19 4381178 B: +192415 B (+4,4 %) — sieben Spuren, darunter eingebackene Modelle (Dokumente
  ~123 KB, Hebetisch-Cursor, Lampen); Linux im Gleichschritt (+193880 B).
- Linux/Deck bis 13:14:58, EXIT=0, `LINUX-BUILD-OK`: `100% tests passed, 0 tests failed out of 463`,
  `Tests: 463 gelaufen = 463 registriert (Untergrenze 463)`; `release/linux_out/re15_pc` 4028752 B, sha256
  `2ce0ae3f76ec929cd270f6b5156ac5d1a5e994e94980901bada12aba5971ca7a`, glibc_max GLIBC_2.29, ldd 0x "not found".
- Android: ⛔ erster Lauf EXIT=1 `build_android.sh: line 93: LOCALAPPDATA: unbound variable` — Ursache
  GEMESSEN: mit `/c/msys64/usr/bin` VORN im PATH startet `bash` = MSYS2-bash, und der Uebergang Git-Bash ->
  MSYS2-Laufzeit verliert LOCALAPPDATA (`bash -c 'echo ${LOCALAPPDATA:-UNGESETZT}'` -> UNGESETZT; mit Git-bash
  gesetzt). Neu gefahren mit `PATH="build/py3shim:$PATH"` (ohne msys64 vorn): `BUILD SUCCESSFUL in 2m 32s`,
  `ANDROID-BUILD-OK`, EXIT=0 (13:19:48). Fuer den naechsten Paketbau: msys64 NUR fuer make_package (zip) vorn.
- make_package (13:21, PATH Shim + msys64): EXIT=0; Optimierungs-Gates Linux 4 / Windows 3 SDL2-Pfade; glibc
  2.29; 27 RE2/DOOR + 30 RE15DOOR je Paket; LF-Gate; Laufzeit-Gate Windows in_pkg 26/26 + foreign_cwd 26/26;
  Linux 3630 / Windows 3631 Dateien (v0.8.19: 3606 / 3607, je +24 = die 24 neuen Assets); x-Bit gesetzt und
  zurueckgelesen; "6 alte Paketdatei(en) aus dem Repo entfernt, 6 neue vorgemerkt". Kein Python-Installer
  (kein release/Python, kein python_install_*.log).

### 3. Checkliste (am AUSGELIEFERTEN Artefakt, Split-Saetze zusammengefuehrt + entpackt im Scratchpad `rel0820_art/`)
| Pruefung | Messwert | Ergebnis |
|---|---|---|
| sha256 Paket == Bau, Windows-exe | beide `18daca84…cc88b6` | OK |
| sha256 Paket == Bau, Linux-Binary | beide `2ce0ae3f…71ca7a` (entpackt IN debian:11, re15-inspect) | OK |
| sha256 Paket == Bau, APK | beide `371f75df…dcd2feb` (APK aus dem Android-Split) | OK |
| PE-Subsystem | Magic 0x20b, Subsystem 2 | OK |
| Laufzeit aus dem Paket | make_package 26/26 + 26/26; zusaetzlich Kopie `r20_art.exe` IM entpackten Paket, fremdes cwd, RE15_ASSET_ROOT/CD_ROOT/RE2_ASSET_ROOT entfernt, Nutzerkarte 2026-09-27 per CONTINUE, `RE15_DOC_REQUEST=260:1` (Dokument 1): rc=0, debug.log cd-root/shared[0]/base[0] = Paket, shared[1] = /src/... (Container-Pfad), **0 Treffer** `C:/workspace`/`worktrees`; Framedumps F310/F400 Titelseite "POLICE OFFICER'S FINAL DIARY ENTRY" auf dem gruenen Tagebuch 1/4, F490 Seite 2/4 mit dem Nutzertext woertlich — angesehen | OK |
| neue Assets (24) | Windows 24/24, Linux 24/24, APK 24/24 sha256-gleich mit `re15_port/shared_assets`; APK 3628 Asset-Eintraege (v0.8.19: 3604, +24) | OK |
| keine Logs im Paket | win 0 / linux 0 (3824 Eintraege) / android 0 | OK |
| LF + x-Bit + GLIBC (debian:11, unzip) | `-rwxr-xr-x` re15_pc + run.sh, run.sh 0 CR, Shebang `#!/usr/bin/env bash`; Direktstart rc=1, run.sh rc=1, **Gegenprobe Modus 644 rc=126**; objdump max GLIBC_2.29 | OK |
| neuer Code im ausgelieferten Code | 12 neue Funktionen (op_col_chg_set, re15_adaruf_install/_ereignis, re15_dokumente_install, re15_hebetisch_cursor_install, re15_leiche_tick/_message_on, re15_mg_aufbauen/_sichtbar, re15_rolltor_ereignis/_op_item_lost, re15_panel_lampen_pc_zeichnen) als T: exe 12/12, Linux 12/12, libmain.so arm64-v8a 12/12 + x86_64 12/12 (1270 T-Symbole, v0.8.19: 1220) | OK |
| APK-Kennung | `de.re15.port` versionCode 82000, versionName v0.8.20, sdk 24 / target 35, arm64-v8a + x86_64 | OK |

### 4. Archiv
- `C:/workspace/Re15Data/re15_packages_archiv/v0.8.20/`: 6 Split-Volumes + APK + SHA256SUMS.txt +
  SHA256SUMS_android.txt; `sha256sum -c`: 6x OK + APK OK, rc=0. Unmittelbar vorherige Version v0.8.19 dort
  entfernt (liegt weiter in Git, Release-Commit 64170c05). Verbleibend: v0.8.7-v0.8.14, v0.8.20.

### 5. Release-Commit + Tag
- `b998f6c3` `release: v0.8.20 (Windows + Linux/Steam Deck + Android)` per `git commit -F`: 6x R (v0.8.19 ->
  v0.8.20) + 2x M (SHA256SUMS*); Blob-Gleichheit `git rev-parse :<f>` == `git hash-object <f>` fuer alle 6.
  Annotierter Tag `v0.8.20` auf b998f6c3. Dieses Dossier als eigener doc-Commit danach.
