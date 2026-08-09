# RE1.5 Port — v0.1.1 (Early Preview)

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
| `re15_port_v0.1.1_win64.zip` | Windows x64 | `re15_pc.exe` doppelklicken (oder `Start_RE15_Port.bat`) |
| `re15_port_v0.1.1_linux_steamdeck_x64.zip` | Linux x64 / Steam Deck (SteamOS 3.5+) | `./run.sh` (Deck: als Non-Steam-Game hinzufuegen) |

Beide Pakete sind selbst-enthalten: SDL2 statisch, Assets unter `shared_assets/PSX`
relativ zum Startordner, Savegames als PSX-Memory-Card-Image `re15_card.mcr` neben
der Anwendung. Gamepads via SDL-GameController (Steam Deck nativ).

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
Linux/Deck: `release/docker_linux_build.sh` (ubuntu:22.04-Container, SDL2 2.28.5 statisch).
