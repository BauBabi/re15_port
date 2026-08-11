# Steam-Deck-Ziel — Bauen, Paketieren, Eintragen

> **Kurzfassung:** Läuft. Die **unveränderte** Basis baut unter Linux zu einem
> x86-64-Binary, das nur `libc/libm/libdl/libpthread` braucht (SDL2 steckt statisch drin)
> und maximal `GLIBC_2.29` verlangt — also auf SteamOS und praktisch jedem modernen Linux.
> Im Container verifiziert: startet, lädt Assets, rendert den Titelbildschirm.

## 1. Bauen

### Empfohlen: im Container (reproduzierbar, unabhängig vom Host)

```bash
docker build -t re15-deck re15_port/targets/steamdeck
docker run --rm -v "$PWD:/src" re15-deck
```

Ergebnis: `re15_port/targets/steamdeck/build/platform/pc/re15_pc`

Basis ist das offizielle **Steam-Runtime-3.0-SDK („sniper")** — Debian 11, GCC 10.3,
glibc 2.31. Das ist genau die Bibliotheksbasis, gegen die Steam-Spiele auf dem Deck laufen.
Wer gegen ein neueres System baut, bekommt ein Binary, das auf SteamOS mit
`GLIBC_2.3x not found` abbricht.

### Direkt auf dem Deck

SteamOS hat ein schreibgeschütztes Wurzeldateisystem, deshalb die Bauabhängigkeiten in einer
`distrobox` installieren (nicht `steamos-readonly disable` — das überlebt kein Systemupdate):

```bash
distrobox create --name build --image debian:11
distrobox enter build
sudo apt install build-essential cmake ninja-build git \
     libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev libxss-dev \
     libwayland-dev libxkbcommon-dev wayland-protocols \
     libasound2-dev libpulse-dev libgl1-mesa-dev libegl1-mesa-dev
./re15_port/targets/steamdeck/build.sh
```

## 2. Paketieren und aufs Gerät bringen

```bash
./re15_port/targets/steamdeck/package.sh
# -> targets/steamdeck/dist/re15-deck/  (Binary + Assets + Startskript, ~285 MB)
```

Den Ordner aufs Deck kopieren, z. B. nach `/home/deck/Games/re15-deck` (SD-Karte geht auch).

## 3. In Steam eintragen

1. Desktop-Modus → Steam → Bibliothek → **Nicht-Steam-Spiel hinzufügen**
2. **Durchsuchen** → `re15-deck.sh` auswählen — **nicht** `re15_pc` direkt
3. Zurück in den Spielmodus; der Eintrag erscheint in der Bibliothek

Warum das Skript statt des Binaries: Es setzt die Asset-Wurzel (`RE15_ASSET_ROOT`) und
wechselt in ein **beschreibbares** Verzeichnis. Der Port legt Spielstand, Log und Screenshots
im Arbeitsverzeichnis an — auf SteamOS ist der Programmordner unter Umständen nicht
beschreibbar, und auf einer SD-Karte wären die Spielstände beim Kartenwechsel weg.

| Was | Wohin |
|---|---|
| Spielstand `re15_card.mcr` (echtes PSX-Kartenformat) | `~/.local/share/re15/` |
| `debug.log` | `~/.local/share/re15/` |
| Screenshots | `~/.local/share/re15/shots/` |

## 4. Steuerung

SDL2 erkennt das Deck-Gamepad; Steam Input übernimmt die Zuordnung. Nach der
Original-Belegung von RE1.5:

| Aktion | PSX | Deck |
|---|---|---|
| Bestätigen / Aktion / Tür | Quadrat | **X** (links) |
| Abbrechen / Rennen | Kreuz | **A** (unten) |
| Inventar | Start | **Start** |
| Zielen | R1 | **R1** |

RE1.5 bestätigt game-weit mit **Quadrat**, nicht mit Kreuz — das ist original so und
weicht bewusst von der RE2-Gewohnheit ab. Wer lieber mit Kreis bestätigt: im Spiel
OPTIONS → Tastenbelegung Typ B oder C.

## 5. Was verifiziert wurde

| Prüfung | Ergebnis |
|---|---|
| Konfiguration + Übersetzung der unveränderten Basis unter Linux | erfolgreich |
| Binary-Typ | ELF 64-bit x86-64, dynamisch |
| Laufzeit-Abhängigkeiten | nur `libc`, `libm`, `libdl`, `libpthread` |
| höchste geforderte glibc-Version | `GLIBC_2.29` (SteamOS liefert 2.36+) |
| Start + Asset-Laden + Dauerlauf | 90 s ohne Absturz |
| Bildausgabe | Titelbildschirm korrekt gerendert (`shots/steamdeck_title_proof.png`) |

### 5.1 Auf echter Hardware gefunden (2026-08-05)

Der erste Lauf auf einem echten Deck hat drei Dinge aufgedeckt, die im Container nicht
sichtbar waren. Alle drei sind behoben; die Fixes stecken in `package.sh` (Paketinhalt),
`re15-deck.sh` (Startumgebung) und `platform/pc/src/bg_pc.c` (Pfad):

| Symptom auf dem Gerät | Ursache | Fix |
|---|---|---|
| **Alle Raumhintergründe schwarz** | Der BSS-Lader baute den Pfad mit `room%04x` (klein), der Asset-Baum heißt `BSS/ROOM1170/`. Windows ist case-**in**sensitiv, ext4 nicht — auf dem Deck fand der Lader nie eine Datei. | `bg_pc.c` schreibt `BSS/ROOM%04X/BG%02d.BSS` groß, genau wie der PRI-Lader zwei Zeilen darüber |
| **Fades, Cutscene-Balken und Schatten weiß statt schwarz** | Der Port zeichnet sie mit `SDL_BLENDOPERATION_REV_SUBTRACT`. Das per Default gewählte SDL-Backend `opengl` kann das auf Mesa nicht: `SDL_SetRenderDrawBlendMode()` schlägt still fehl, der alte BLEND-Modus bleibt stehen, die helle Graustufen-Quelle wird deckend gezeichnet. Gemessen mit `RE15_PSELECT_DIAG`: nur `opengles2` liefert den SUBTRACT-Pfad. | `re15-deck.sh` setzt `SDL_RENDER_DRIVER=opengles2` (überschreibbar) |
| **Kein Blut, kein Mündungsfeuer, kein Rauch, keine Hülsen** | Diese Texturen liegen im Original nur im VRAM; der Port lädt sie aus `shared_assets/extracted_fx/` (`platform/pc/main.c`, Tabelle `extracted_fx/effect*.tim`), gesucht als `$RE15_CD_ROOT/../extracted_fx/`. Das Paket enthielt den Ordner nicht. | `package.sh` kopiert `extracted_fx/` als Geschwister von `assets/` |

Dieselben drei Defekte steckten im Release-Paket `re15_port_v0.1.1_linux_steamdeck_x64.zip`
(plus ein vierter: gegen `ubuntu:22.04` gebaut → `GLIBC_2.34`, startet auf SteamOS 3.4 nicht).
Das Release-Paketieren prüft sie jetzt aktiv — siehe `release/make_package.sh`
(`check_binary_paths`, `check_tree`, `check_glibc`) und `release/RELEASE_NOTES.md`.

**Weiterhin nicht verifiziert:** Gamescope im Spielmodus, die tatsächliche
Steam-Input-Zuordnung und die Bildwiederholrate.

Ebenfalls offen: Der Port skaliert intern auf 320×240 (PSX-Auflösung) und rendert im Fenster
640×480. Wie das im Spielmodus auf dem 1280×800-Display des Decks skaliert, ist ungetestet —
Gamescope streckt es vermutlich, ein Vollbild-/Seitenverhältnis-Schalter wäre der nächste
Schritt für ein sauberes Bild.
