# RE1.5 Port — v0.2 (Early Preview)

## Neu in v0.2 (gegenueber v0.1.2) — RE2-KI-Modus fuer Zombies, Hunde, Kraehen

Im OPTIONS-Menue schaltet der Eintrag **AI: RE1.5 / RE2** die drei Gegnertypen,
die es auch im fertigen Retail-RE2 gibt, auf deren **vollstaendiges
RE2-Original-Verhalten** um — Gehirn, Animationen, Sounds und Effekt-Ablaeufe
byte-genau aus den RE2-Overlays re-engineert (EMZ0 / EMD0G_MOD0 / EMOVL21,
jede Konstante mit Disassembly-Zitat im Code, Master-Doku `RE15_RE2_AI.md`):

- **Zombie** (Wellen A+B): RE2-Gangarten-Maschine, Angriffs-Leiter, 10-Phasen-
  Grab mit Biss-Frames/Schaden aus dem Original-Parameterblock, Partner-
  Aufweck-Domino, dreistufige Trefferreaktion, RE2-Modelle/Clips + ENEMSE-SEs.
- **Hund/Cerberus** (Welle C): 17 Zustaende — Stalking mit Aggro-Meter,
  4-Phasen-Verfolgung mit Ermuedung, Sprungattacke mit echter Flugbahn,
  **Kehlen-Latch als Finisher** (nur bei toedlichem Biss; per Tasten-Hammern
  abschuettelbar), Rudel-Heul-Koordination.
- **Kraehe** (Welle D): 15 Zustaende — Schwarm-Koordination (es greift immer
  nur EINE an), Sichtpruefung, Sturzflug-Hieb, Festkrallen mit Pick-Schaden,
  Spiralsturz mit Erholung, zuckende Leiche mit wachsender Blutlache.
- Die **Spinne** folgt in einer spaeteren Version.

Der RE1.5-Modus (Default) bleibt byte-identisch unberuehrt. Qualitaetssicherung:
je Welle ein adversarieller Multi-Agent-Review (alle bestaetigten Divergenzen
vor dem Commit gefixt), 132 Engine-Tests inkl. A/B-Proben RE1.5-vs-RE2 in
ROOM1140/1190/10C0 (Biss exakt 20 HP, Peck exakt 5 HP, keine Doppel-Claims).

Paketierung: `shared_assets/RE2/` (CDEMD0.EMS + ENEMSE.VBS, ~18 MB) ist neu im
Paket; die Startskripte exportieren `RE15_RE2_ASSET_ROOT`, und
`release/make_package.sh` bricht ab, wenn die RE2-Assets fehlen.
Die Dateinamen unten heissen entsprechend `re15_port_v0.2_*`.

---

## Neu in v0.1.2 (gegenueber v0.1.1) — Linux-/Steam-Deck-Paket repariert
Das v0.1.1-Deck-Paket war in vier Punkten falsch geschnuert. Alle vier sind
belegt und behoben; `release/make_package.sh` bricht jetzt ab, statt sie
noch einmal auszuliefern:

| Defekt in v0.1.1 | Wirkung auf dem Deck | Fix |
|---|---|---|
| Binary enthielt `%sBSS/%s/BG%02d.BSS` (kleingeschriebener Raumordner), der Baum heisst `BSS/ROOM1170/` | ext4 ist case-sensitiv -> **kein Raumhintergrund**, alles schwarz | `bg_pc.c` schreibt den Pfad gross (wie der PRI-Lader daneben); Gate `check_binary_paths` |
| `shared_assets/extracted_fx/` fehlte im Paket (0 Dateien) | **Blut, Muendungsfeuer, Rauch, Huelsen rendern nicht** | wird mitpaketiert; Gate `check_tree` |
| `run.sh` war 53 Bytes (`cd` + `exec`) — kein `RE15_CD_ROOT`, kein Render-Backend | Effekt-Texturen unauffindbar; Fades/Cutscene-Balken **weiss statt schwarz** (Mesa kann den subtraktiven Blend nur unter `opengles2`) | `release/pkg_files/linux/run.sh`, jetzt im Repo versioniert |
| Build gegen `ubuntu:22.04` -> `GLIBC_2.34` | startet auf SteamOS 3.4 (glibc 2.33) **gar nicht** | Build auf Debian-11-/sniper-Basis (`GLIBC_2.29`); Gate `check_glibc` |

Ursache dahinter: `pkg-linux/` ist gitignoriert und wurde von Hand befuellt —
Startskript und Paketinhalt hatten keine versionierte Quelle. Jetzt:
`release/pkg_files/` (Startskripte + README-Vorlagen) + `release/make_package.sh`.

Ausserdem enthalten:
- 1-Frame-ROOM1170-BG-Leck zwischen Player-Select und Intro beseitigt.
- `test_em_status_persist` repariert (nur Test, keine Engine-Aenderung). Er stuerzte in
  ~1 von 150 Laeufen ab und blockierte damit das Release-Gate. Ursache (ASan:
  stack-buffer-underflow in `scd_vm_tick`, `scd_vm.c:594`): der Re-Entry-Teil baute den
  Raum-Wiedereintritt mit `re15_actor_init()` nach, liess Slot 1 aber mit dem geparkten
  Thread des vorigen Spawns belegt. `scd_thread_start` verweigert einen belegten Slot
  (`scd_vm.c:433`), also lief Pruefung (5) NIE — sie hielt aus dem falschen Grund —, und
  der geparkte `pc` zeigte auf einen toten Stack-Puffer. Jetzt benutzt der Test die echte
  Kette `scd_room_reenter` (byte-true FUN_8001d600 -> FUN_800396fc -> FUN_8003ef6c); die
  Pruefung laeuft wirklich und haelt (400/400 sauber, ASan-frei).

## Neu in v0.1.1 (gegenueber v0.1)
- **Elliot-Intro (ROOM1170) byte-true:** Der Renn-Glide ist behoben, und Elliot laeuft
  jetzt vollstaendig ueber die NPC-Sub-VM mit seiner EIGENEN EM047-Animationsbank
  (Roadmap 7b komplett; inkl. eines nachgezogenen Original-Details: das
  SCD-Ankunfts-Flag des Turn-Subs @0x80051dd8).
- **Debug-Menue (SELECT) byte-true:** Original-Navigationsgefuehl (Auto-Repeat-Kadenz
  0/6/8/10), Kreuz/SELECT schliessen per Tasten-EDGE (gehaltene Renn-Taste schliesst
  nicht mehr sofort), 8x8-Original-Debugfont aus TEX.TIM statt der Spielschrift,
  subtraktive Hintergrund-Box, Raumnummern mit fuehrender Null. Bestaetigung = ▢.
- Test-Suite-Fix (Use-after-free, nur Tests; Linux-Gate deckte ihn auf).
Die Dateinamen unten heissen entsprechend `re15_port_v0.1.1_*`.

---

# v0.1 (Basis)

Erster eigenstaendiger PC-Release des Resident-Evil-1.5-Ports (MZD-Build-Basis).
Zwei Pakete (je ~135 MB gezippt, ~290 MB entpackt, Assets enthalten).
**Die ZIPs liegen als Standard-SPLIT-ZIPs direkt in diesem Repo-Ordner**
(Volumes `.z01` + `.zip`, je < 100 MB): einfach die `.zip` mit 7-Zip, WinRAR
oder (Steam Deck) Ark oeffnen — die Volumes werden automatisch als EIN Archiv
gelesen. Hinweis: der Windows-Explorer-eigene ZIP-Handler kann KEINE
Split-Zips — 7-Zip/WinRAR verwenden. Pruefsummen: `SHA256SUMS.txt`.

| Paket | Plattform | Start |
|---|---|---|
| `re15_port_v0.2_win64.zip` | Windows x64 | `Start_RE15_Port.bat` doppelklicken |
| `re15_port_v0.2_linux_steamdeck_x64.zip` | Linux x64 / Steam Deck (SteamOS 3.x) | `./run.sh` (Deck: als Non-Steam-Game hinzufuegen) |

Beide Pakete sind selbst-enthalten: SDL2 statisch, Assets unter `shared_assets/PSX`
(CD-Baum) plus `shared_assets/extracted_fx` (Effekt-Texturen), Savegames als
PSX-Memory-Card-Image `re15_card.mcr` neben der Anwendung. Gamepads via
SDL-GameController (Steam Deck nativ).
**Startskript benutzen, nicht das Binary direkt** — nur `run.sh` bzw.
`Start_RE15_Port.bat` setzen `RE15_ASSET_ROOT`/`RE15_CD_ROOT`, ohne die findet
der Port `extracted_fx/` nicht (Effekte fehlen dann still).

## Steuerung (Tastatur, DuckStation-Layout)
Pfeile/WASD laufen · K/Shift rennen (✕) · J Aktion (▢, haelt Texte schneller) ·
L Abbrechen (◯) · I Inventar (△) · E zielen (R1) + J Schuss · Enter Start ·
Backspace Select · 1/3 = L2/R2.

## Stand v0.1 (Auszug)
- Byte-true re-engineerte Kernsysteme: Spieler/Kampf/Gegner-KI (alle STAGE1-Typen
  inkl. ROOM1030-Kriechtor mit Hand-Lock-Root-Motion), AOT/SCD-VM, Kamera-Zonen,
  Tuersystem (inkl. Fahrstuhl), Save/Load mit Ortsnamen, Inventar/Item-Box,
  Effekt-Pipeline (additive/subtraktive Blends), Blut-Decals, BGM/SE.
- Verifikation: 123 Engine-Tests (ctest) + Hardware-Gegenmessungen (PCSX-Redux)
  + Savestate-Vergleiche; Details in `analysis/` und `ROADMAP_ROOMCHAIN.md`.

## Bekannte Luecken (ehrlich)
- STAGE-2-6-Inhalte in unterschiedlicher Tiefe; diverse dokumentierte OFFEN-Punkte
  (siehe ROADMAP_ROOMCHAIN.md §5 / UNTESTED_IMPLEMENTATIONS.md).
- Steam Deck: Tastatur-Layout ist Default — fuer Gamepad ggf. Steam-Input-Profil.
- v0.1 = Preview: Abstuerze/Regressionen bitte mit Raum + Schrittfolge melden.

## Build-Reproduktion
Windows: `cmake -S re15_port -B build -G Ninja -DRE15_BUILD_PC=ON -DCMAKE_BUILD_TYPE=Release`
(Binary nach `release/win_out/re15_pc.exe`).

Linux/Deck — **Debian-11-Basis (glibc 2.31), nicht ubuntu:22.04**:
```bash
release/build_linux_deck.sh                    # docker/podman, Image debian:11
release/build_linux_deck.sh --distrobox re15-build   # auf dem Deck selbst
```
Paketieren (beide Plattformen, mit den Gates oben):
```bash
release/make_package.sh --version v0.2               # --only linux | --only win
```
Ergebnis: `re15_port_v0.2_{linux_steamdeck_x64,win64}.{z01,zip}` + `SHA256SUMS.txt`.
