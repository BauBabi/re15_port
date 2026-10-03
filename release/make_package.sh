#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Release-Pakete schnueren (Linux/Steam Deck + Windows)
# =============================================================================
# EINZIGE Quelle der Wahrheit fuer den Paketinhalt. Vorher wurden pkg-linux/
# und pkg-win/ von Hand zusammengestellt; die Ordner sind gitignoriert, also
# ging jede Korrektur am Startskript/Inhalt beim naechsten Release verloren.
# Genau so entstand das v0.1.1-Deck-Paket mit drei Defekten:
#   * kein shared_assets/extracted_fx  -> Blut/Muendungsfeuer/Rauch/Huelsen fehlten
#   * run.sh mit 53 Bytes (nur cd+exec) -> weisse Fades/Balken auf Mesa,
#                                          Effekt-Texturen unauffindbar
#   * Binary aus ubuntu:22.04           -> verlangte GLIBC_2.34
# Die Pruefungen unten (Abschnitt "Gates") lassen jeden dieser Faelle das
# Paketieren ABBRECHEN, statt ein kaputtes Paket auszuliefern.
#
# Aufruf:
#   release/make_package.sh --version v0.1.2                 # beide Plattformen
#   release/make_package.sh --version v0.1.2 --only linux    # nur Linux/Deck
#   release/make_package.sh --version v0.1.2 --no-zip        # nur Ordner bauen
#   release/make_package.sh --version v0.1.2 --ohne-android  # nur PC-Saetze; ein Android-Satz DIESER
#                                                            # Version wird entfernt (siehe Android unten)
#
# ⛔ PATH unter Windows (gemessen beim Paketbau v0.8.21, reai-v2-b5): zip liegt in /c/msys64/usr/bin -
# diesen Ordner HINTEN anhaengen, nicht vorn einsetzen:
#   PATH="$PATH:/c/msys64/usr/bin" bash release/make_package.sh --version vX.Y.Z
# Steht er VORN, startet "bash" das MSYS2-bash, und dabei gingen LOCALAPPDATA und sogar ein gesetztes
# ANDROID_SDK_ROOT verloren -> apk_pruefen.sh suchte das SDK unter /home/.../Android/Sdk und brach ab.
# (Seit dem Folgecommit erfragt apk_pruefen.sh den Ordner notfalls selbst bei Windows, CSIDL 28.)
#
# Eingaben (werden NICHT hier gebaut):
#   Linux  : release/linux_out/re15_pc    <- release/build_linux_deck.sh
#   Windows: release/win_out/re15_pc.exe  <- mingw64-Build, siehe RELEASE_NOTES
#   Android: release/re15_port_<version>_android.apk (optional) <- release/build_android.sh
#            liegt sie da, wird sie (nur beim Zippen) mit DERSELBEN Kette wie im Android-Bau
#            geprueft (release/apk_pruefen.sh) und in den Split-Satz gebracht; aus dem fertigen Satz
#            wird sie wieder entpackt und per sha256 mit der geprueften Kopie verglichen.
#            Ausgeliefert (SHA256SUMS.txt, git add) wird ein Android-Satz NUR, wenn er in DIESEM Lauf
#            aus einer gerade geprueften APK entstand (Runde 4, Gegenpruefung R3 B1). Fehlt die APK:
#              * kein re15_port_<version>_android.z* da -> kein Android-Satz (Hinweis beim Zippen);
#              * ein Satz DERSELBEN Version liegt da (frueherer Lauf, auch versioniert) -> ABBRUCH vor
#                den Kopierminuten. Dann entweder die APK neu bauen (build_android.sh) oder mit
#                --ohne-android nur die PC-Saetze schnueren: das entfernt den alten Satz (Datei und
#                git-Index), statt ihn ungeprueft mitzuliefern.
#            Aeltere Versionen (re15_port_<alt>_android.z*) bleiben wie bisher unangetastet.
#   SHA256SUMS.txt und git add (Nachbesserung R4-1, Gegenpruefung H3): nur eine POSITIVLISTE - die in diesem Lauf
#            gezippten Saetze, bewusst der Satz der anderen PC-Plattform derselben Version (frueherer Lauf, wird als
#            solcher gemeldet) und der Android-Satz aus diesem Lauf. Jede andere Datei <NAME>_*.z* (z.B.
#            re15_port_<v>_ANDROID.zip) bricht ab - vor den Kopierminuten und vor dem Schreiben der SUMS.
#   Asset-Gate (Nachbesserung R4-1, H1/H2): es laeuft nur als private Kopie mit der sha256 aus
#            release/apk_asset_gate.sha256, und jedes Urteil kommt aus Rueckgabe UND Ausgabe (release/apk_pruefen.sh
#            gate_laufen). Wer apk_asset_gate.py aendert, haelt den neuen Wert dort fest.
#
# Voraussetzungen (Runde 34a, Nachbesserung R2 - Gegenpruefung echtlauf B2; fehlt etwas, bricht
# das Skript mit Meldung ab, statt eine Pruefung auszulassen):
#   * Python >= 3.8, IMMER (release/python_finden.sh, nie der WindowsApps-Alias): Selbsttest des Gates,
#     Quellbaum- und Paketpruefung (release/apk_asset_gate.py --selbsttest / --quellbaum / --paket),
#     verify_split, zip_exec_bit.py
#   * zip (auch aus /c/msys64/usr/bin), unzip + sha256sum (Android-Satz entpacken und vergleichen),
#     strings/objdump fuer die Binary-Gates (sonst uebersprungen)
#   * NUR wenn die APK da ist und gezippt wird: Android-SDK mit build-tools 35.0.0 (aapt, zipalign,
#     lib/apksigner.jar), ein JDK (JAVA_HOME oder Adoptium 17), release/apk_signer.sha256
#     (erwarteter Signer) - siehe release/apk_pruefen.sh
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"

VERSION="v0.1.2"
ONLY="both"
DO_ZIP=1
ZIP_ONLY=0                        # nur zippen, vorhandene pkg-*/ wiederverwenden
OHNE_ANDROID=0                    # nur PC-Saetze; Android-Satz dieser Version entfernen (Runde 4, B1)
SPLIT="90m"                       # < 100 MB je Volume (GitHub-Dateigrenze)
LINUX_BIN="$HERE/linux_out/re15_pc"
WIN_BIN="$HERE/win_out/re15_pc.exe"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --version)   VERSION="$2"; shift 2 ;;
        --only)      ONLY="$2";    shift 2 ;;
        --linux-bin) LINUX_BIN="$2"; shift 2 ;;
        --win-bin)   WIN_BIN="$2";   shift 2 ;;
        --no-zip)    DO_ZIP=0; shift ;;
        --zip-only)  ZIP_ONLY=1; shift ;;
        --ohne-android) OHNE_ANDROID=1; shift ;;
        -h|--help)   awk 'NR == 1 { next } /^set -euo pipefail/ { exit } { print }' "${BASH_SOURCE[0]}"; exit 0 ;;
        *) echo "unbekannte Option: $1" >&2; exit 2 ;;
    esac
done

ASSETS="$REPO/re15_port/shared_assets/PSX"
FX="$REPO/re15_port/shared_assets/extracted_fx"
RE2="$REPO/re15_port/shared_assets/RE2"
RE15DOOR="$REPO/re15_port/shared_assets/RE15DOOR"
SYNCHRO="$REPO/synchro"
NAME="re15_port_${VERSION}"

# --- Gates ------------------------------------------------------------------
# Die vier Texturen, die platform/pc/main.c beim Start aus extracted_fx laedt
# (main.c, Tabelle "extracted_fx/effect*.tim"). Fehlt eine, rendert der
# zugehoerige Effekt nicht.
FX_REQUIRED=(effect0_blood.tim effect2_muzzle.tim effect3_smoke.tim effect4_shell.tim)

die() { echo "ABBRUCH: $*" >&2; exit 1; }

check_binary_paths() {   # $1 = Binary
    local bin="$1"
    # Der BSS-Lader muss den GROSS geschriebenen Baumnamen benutzen
    # (platform/pc/src/bg_pc.c, "BSS/ROOM%04X/BG%02d.BSS"). Ein Binary mit der
    # alten Kleinschreibung findet auf case-sensitiven Dateisystemen (ext4,
    # also SteamOS) KEINEN Raumhintergrund -> alles schwarz.
    if grep -aq 'BSS/%s/BG%02d.BSS' "$bin"; then
        die "$bin ist VERALTET: enthaelt den kleingeschriebenen BSS-Pfad
        ('BSS/%s/BG%02d.BSS'). Auf case-sensitiven Dateisystemen bleiben alle
        Raumhintergruende schwarz. Neu bauen (bg_pc.c-Fix im Baum?)."
    fi
    grep -aq 'BSS/ROOM%04X/BG%02d.BSS' "$bin" \
        || die "$bin enthaelt den erwarteten BSS-Pfad nicht — kein RE1.5-Binary?"
}

check_binary_optimiert() {   # $1 = Binary, $2 = Label
    # ⛔ BEINAHE-UNFALL 2026-09-04: Nach einem Fix habe ich das Binary aus
    # re15_port/build/platform/pc/ nach win_out/ kopiert und wollte paketieren.
    # re15_port/tools/local_build.sh laesst CMAKE_BUILD_TYPE **leer** - das ist ein
    # Entwicklungs-Build OHNE Optimierung. Release-Binaries kommen ausschliesslich aus
    #   release/docker_win_build.sh   (-DCMAKE_BUILD_TYPE=Release)
    #   release/build_linux_deck.sh
    # Aufgefallen ist es nur, weil die exe 1,4 MB groesser war als die vorige und das
    # nicht zu einer 50-Zeilen-Aenderung passte. KEINE der uebrigen Pruefungen haette
    # es gefangen - sie pruefen Subsystem, Laufzeit, Zeilenenden, x-Bit, Logs.
    #
    # MERKMAL, GEMESSEN (nicht gewaehlt): Release setzt -DNDEBUG, damit laesst SDL2
    # seine __FILE__-Pfade in Fehler-/Assert-Meldungen weg. Gezaehlt wurden die
    # Vorkommen der Zeichenkette "sdl2-src/src/" in drei Binaries desselben Tages:
    #     v0.5.8 Windows (Release, ausgeliefert) :  3
    #     v0.5.9 Linux   (Release)               :  4
    #     lokaler Entwicklungs-Build             : 54
    # Die Schranke liegt bei 20, also weit von beiden Messwerten entfernt; die
    # Trennung betraegt Faktor 13. Schlaegt sie an, ist die Frage NICHT die Schranke,
    # sondern welches Binary da liegt.
    local bin="$1" label="$2" n
    command -v strings >/dev/null 2>&1 || {
        echo "   (Optimierungs-Gate uebersprungen: 'strings' fehlt)"; return; }
    n="$(strings -a "$bin" 2>/dev/null | grep -c 'sdl2-src/src/' || true)"
    [[ -n "$n" ]] || return
    if (( n > 20 )); then
        die "$label ist KEIN Release-Build: $bin
        traegt $n SDL2-Quellpfade (Release-Binaries: 3-4). CMAKE_BUILD_TYPE war leer
        oder Debug - das Paket waere unoptimiert und damit langsamer.
        Richtig bauen:
          Windows: MSYS_NO_PATHCONV=1 docker run --rm -v \"$REPO\":/src debian:11 \
                       bash /src/release/docker_win_build.sh
          Linux  : release/build_linux_deck.sh
        Das Entwicklungs-Binary aus re15_port/build/ gehoert NICHT in ein Paket."
    fi
    echo "   Optimierungs-Gate: $label ist ein Release-Build ($n SDL2-Quellpfade)"
}

check_binary_fresh() {   # $1 = Binary, $2 = Label, [$3 ... = git-Pfade des Codes IN diesem Binary]
    # ⛔ v0.3.9-UNFALL (2026-08-21): Das Skript BAUT NICHT, es KOPIERT aus win_out/
    # bzw. linux_out/. Der Windows-Build lief nach release/wbuild/, win_out/ blieb
    # auf dem Stand von v0.3.8 — das ausgelieferte Paket enthielt KEINEN der fuenf
    # Fix-Commits (Beleg: 're15_climb' 0x im Paket-Binary, 10x im echten Build),
    # und der Nutzer hat vier bereits behobene Fehler erneut gemeldet.
    # Gate: das Binary muss NEUER sein als der letzte Commit, der SEINEN Code aendert.
    # Standard = PC-Binaries: engine, include, platform OHNE platform/android (Nachbesserung
    # Runde 34a, Gegenpruefung echtlauf B2: der PC-Bau haengt nicht an platform/android -
    # re15_port/CMakeLists.txt:92-101 - und ein reiner build.gradle-Commit erklaerte die
    # v0.8.19-PC-Binaries sonst fuer VERALTET). Die Android-APK uebergibt ihre eigenen Pfade.
    local bin="$1" label="$2" bin_t src_t
    shift 2
    local -a pfade=("$@")
    (( ${#pfade[@]} )) || pfade=(re15_port/engine re15_port/include re15_port/platform ':(exclude)re15_port/platform/android')
    # Nachbesserung R2 (Gegenpruefung B9): die drei Vorbedingungen brachen bis dahin mit '|| return'
    # (Status 1) unter set -e STUMM ab - fail closed, aber ohne Meldung. Jetzt mit Meldung, Ergebnis gleich.
    command -v git >/dev/null 2>&1 \
        || die "Frische-Gate ($label): git fehlt - ohne Commit-Zeit ist nicht pruefbar, ob $bin aktuell ist"
    src_t="$(git -C "$HERE/.." log -1 --format=%ct -- "${pfade[@]}" 2>/dev/null || true)"
    [[ -n "$src_t" ]] || die "Frische-Gate ($label): git log findet keinen Commit fuer ${pfade[*]}
        (kein Git-Repo, flacher Klon?) - nicht pruefbar, ob $bin aktuell ist"
    bin_t="$(stat -c %Y "$bin" 2>/dev/null || stat -f %m "$bin" 2>/dev/null || true)"
    [[ -n "$bin_t" ]] || die "Frische-Gate ($label): Zeitstempel von $bin nicht lesbar"
    if (( bin_t < src_t )); then
        die "$label ist VERALTET: $bin
        stammt von $(date -d "@$bin_t" '+%F %T' 2>/dev/null || date -r "$bin_t" '+%F %T'),
        der letzte Port-Code-Commit von $(date -d "@$src_t" '+%F %T' 2>/dev/null || date -r "$src_t" '+%F %T').
        Dieses Skript baut NICHT — es kopiert nur. Erst neu bauen und das Ergebnis
        nach $(dirname "$bin")/ kopieren, dann das Paket erzeugen."
    fi
}

check_glibc() {          # $1 = Linux-Binary
    command -v objdump >/dev/null 2>&1 || { echo "   (objdump fehlt — glibc-Gate uebersprungen)"; return; }
    local max
    max="$(objdump -T "$1" 2>/dev/null | grep -oE 'GLIBC_[0-9.]+' | sort -uV | tail -1)"
    [[ -n "$max" ]] || return
    echo "   hoechste glibc-Anforderung: $max"
    # SteamOS 3.4 liefert glibc 2.33; die Steam-Runtime-3.0-Basis ("sniper",
    # Debian 11) hat 2.31. Alles darueber schliesst Geraete aus.
    local n="${max#GLIBC_}"
    if [[ "$(printf '%s\n2.31\n' "$n" | sort -V | tail -1)" != "2.31" ]]; then
        die "Binary verlangt $max > GLIBC_2.31. Es wurde gegen ein zu neues
        System gebaut (ubuntu:22.04 o.ae.). Mit release/build_linux_deck.sh auf
        Debian-11-/sniper-Basis neu bauen."
    fi
}

check_lf() {             # $1 = fertiger Paketordner — nur fuer Linux-Pakete sinnvoll
    # Ein Shell-Skript mit CRLF ist auf dem Deck TOT: die Shell liest das \r als Teil des
    # Interpreter-Pfads ("/usr/bin/env bash\r: nicht gefunden") bzw. haengt es an jedes
    # Kommando. Gemeldet 2026-08-25 vom Nutzer, nachdem run.sh genau so ausgeliefert wurde.
    # Der Paketbau kopiert aus dem ARBEITSBAUM, wo `.gitattributes` (eol=lf) nicht greift —
    # deshalb hier am FERTIGEN Paket pruefen, nicht an der Quelle.
    local out="$1" bad=0 f
    while IFS= read -r f; do
        if LC_ALL=C grep -qU $'\r' "$f"; then
            echo "   CRLF in $f" >&2
            bad=1
        fi
    done < <(find "$out" -maxdepth 1 -type f \( -name '*.sh' -o -name '*.desktop' \))
    [[ $bad -eq 0 ]] || die "Shell-/Desktop-Datei mit CRLF im Linux-Paket — auf dem Deck
        scheitert das schon am Shebang. Quelle normalisieren (tr -d '\\r') und neu packen."
    echo "   Zeilenenden-Gate: alle Skripte im Paket sind LF"
}

check_tree() {           # $1 = fertiger Paketordner
    local out="$1"
    [[ -d "$out/shared_assets/PSX/STAGE1" ]] || die "Asset-Baum unvollstaendig in $out"
    for f in "${FX_REQUIRED[@]}"; do
        [[ -f "$out/shared_assets/extracted_fx/$f" ]] \
            || die "Effekt-Textur fehlt im Paket: shared_assets/extracted_fx/$f"
    done
    # Seit v0.2: OPTIONS->AI=RE2 laedt Gegner-Modelle/-Sounds aus shared_assets/RE2/
    # (platform/pc/main.c pc_re2_cdemd, audio_pc.c read_re2_enemse_vbs — beide ueber
    # re15_pc_read_re2(): env RE15_RE2_ASSET_ROOT, sonst <shared>/RE2/). Fehlen die
    # Dateien, faellt die Option still auf RE1.5 zurueck -> Gate statt Stille.
    for f in CDEMD0.EMS ENEMSE.VBS; do
        [[ -s "$out/shared_assets/RE2/$f" ]] \
            || die "RE2-Asset fehlt/leer im Paket: shared_assets/RE2/$f (RE2-AI-Option waere still tot)"
    done
    # Seit v0.8.16: die Tuersequenz des Tors ROOM1170 spielt den Ton des RE2-Gittertors aus
    # shared_assets/RE2/TORSE.VBS (audio_pc.c load_re2_tor_se_pc). Fehlt sie, laeuft die
    # Sequenz stumm - Gate statt Stille. Seit Runde 34a zusaetzlich bytegleich (cmp) mit der
    # Quelle, wie die Tuerarchive unten.
    [[ -s "$out/shared_assets/RE2/TORSE.VBS" ]]         || die "RE2-Asset fehlt/leer im Paket: shared_assets/RE2/TORSE.VBS (Tuersequenz waere stumm)"
    cmp -s "$RE2/TORSE.VBS" "$out/shared_assets/RE2/TORSE.VBS" \
        || die "RE2-Asset im Paket weicht vom Quellbaum ab: shared_assets/RE2/TORSE.VBS"
    # Seit Runde 34 Nacht (Spur C): die zwei gruenen Generator-Lampen ROOM11F0/11F1 lesen
    # shared_assets/RE2/LAMPE2130.TIM (panel_lampen_pc.c) - fehlt sie, bleiben sie still dunkel.
    [[ -s "$out/shared_assets/RE2/LAMPE2130.TIM" ]]     || die "RE2-Asset fehlt/leer im Paket: shared_assets/RE2/LAMPE2130.TIM (Generator-Lampen waeren dunkel)"
    # Seit Runde 35 (Spur B, Werfer): Flammenwerfer-, Raketen- und Leerschuss-Toene der RE2-Baenke
    # shared_assets/RE2/SOUND/ARMS10/ARMS11 (audio_pc.c re15_audio_re2_arms_se). Fehlen sie, bleiben
    # Flammenstrahl, Raketen-Explosion und der Leer-Klick des Raketenwerfers still - Gate statt Stille.
    local rs
    for rs in SOUND/ARMS10.EDH SOUND/ARMS10.VB SOUND/ARMS11.EDH SOUND/ARMS11.VB; do
        [[ -s "$out/shared_assets/RE2/$rs" ]] || die "RE2-Asset fehlt/leer im Paket: shared_assets/RE2/$rs (Werfer-Toene waeren stumm)"
        cmp -s "$RE2/$rs" "$out/shared_assets/RE2/$rs" || die "RE2-Asset im Paket weicht vom Quellbaum ab: shared_assets/RE2/$rs"
    done
    # Seit Runde 31: die RE2-Tuersequenzen der 184 abgedeckten Tuerseiten lesen ihr Archiv
    # UNVERAENDERT aus shared_assets/RE2/DOOR/DOORxx.DO2 (door_scene_pc.c re2_archiv_lesen,
    # Modellteil + Tonteil). Fehlt eine Datei, laeuft an diesen Tueren der RE1.5-Uebergang
    # ohne Sequenz - Gate statt Stille: jede DOORxx.DO2 des Quellbaums muss im Paket liegen,
    # darf nicht leer sein und muss (seit Runde 34a, wie RE15DOOR) bytegleich sein.
    local n_tuer=0 tf
    for tf in "$RE2"/DOOR/*.DO2; do
        [[ -e "$tf" ]] || die "Quellbaum ohne shared_assets/RE2/DOOR/*.DO2 (Tuersequenzen haetten kein Modell)"
        [[ -s "$out/shared_assets/RE2/DOOR/$(basename "$tf")" ]] \
            || die "RE2-Asset fehlt/leer im Paket: shared_assets/RE2/DOOR/$(basename "$tf") (Tuersequenz ohne Modell)"
        cmp -s "$tf" "$out/shared_assets/RE2/DOOR/$(basename "$tf")" \
            || die "RE2-Tuerarchiv im Paket weicht vom Quellbaum ab: shared_assets/RE2/DOOR/$(basename "$tf")"
        n_tuer=$((n_tuer + 1))
    done
    echo "   Tuerarchive im Paket: $n_tuer x shared_assets/RE2/DOOR/*.DO2"
    # Seit Runde 33: PORT-EIGENE Tuerarchive im RE2-Aufbau (tools/tueren/tuer_archiv_bauen.py,
    # door_scene_pc.c re2_archiv_lesen mit eigen != 0, gen/re15_tuer_eigen.inc prueft Groesse +
    # FNV-1a). Fehlt eine Datei, laeuft an diesen Tueren der RE1.5-Uebergang - Gate statt Stille.
    local n_eig=0
    for tf in "$RE15DOOR"/*.DO2; do
        [[ -e "$tf" ]] || die "Quellbaum ohne shared_assets/RE15DOOR/*.DO2 (Port-Tuerarchive fehlen)"
        [[ -s "$out/shared_assets/RE15DOOR/$(basename "$tf")" ]] \
            || die "Port-Tuerarchiv fehlt/leer im Paket: shared_assets/RE15DOOR/$(basename "$tf") (Tuersequenz ohne Modell)"
        cmp -s "$tf" "$out/shared_assets/RE15DOOR/$(basename "$tf")" \
            || die "Port-Tuerarchiv im Paket weicht vom Quellbaum ab: shared_assets/RE15DOOR/$(basename "$tf")"
        n_eig=$((n_eig + 1))
    done
    echo "   Port-Tuerarchive im Paket: $n_eig x shared_assets/RE15DOOR/*.DO2"
    # Voiceover: der Port laedt NICHT aus shared_assets/PSX/VOICE, sondern aus
    # synchro/STAGE<n>/room<id>/main<nn>.wav (audio_pc.c re15_voice_load_clip).
    # Seit 2026-08-24 ueber die BASIS-Wurzelliste (asset_root_pc.c): synchro/ muss
    # neben der exe liegen. Bis v0.3.16 fehlte synchro/ in JEDEM Paket — auf dem
    # Dev-Rechner traf die alte ../../..-Probe das Repo, im ausgelieferten Paket
    # (Deck) blieb das Voiceover stumm.
    [[ -s "$out/synchro/STAGE1/room1170/main00.wav" ]] \
        || die "Voiceover fehlt im Paket: synchro/STAGE1/room1170/main00.wav"
    # Ueber ALLE Stages zaehlen (nicht nur STAGE1) — sonst faellt eine neu angelegte
    # STAGE2..6-Aufnahme stillschweigend aus dem Paket, ohne dass das Gate anschlaegt.
    local want got
    want="$(find "$SYNCHRO"/STAGE* -name '*.wav' 2>/dev/null | wc -l)"
    got="$(find "$out/synchro" -name '*.wav' 2>/dev/null | wc -l)"
    (( got == want )) || die "Voiceover unvollstaendig im Paket: $got/$want WAVs unter synchro/"
    # Nachbesserung R2 (Gegenpruefung B6): copy_common ist eine DRITTE Liste neben build.gradle und
    # BAEUME in apk_asset_gate.py - niemand verglich sie. Jetzt prueft das Gate den fertigen Paketordner
    # gegen dieselbe Liste wie die APK: jede Datei der Asset-Baeume mit gleicher Groesse und sha256,
    # unter shared_assets/ und synchro/ nichts sonst (auch Tuer-Soll + Wurzel des Quellbaums).
    # Runde 4 (Gegenpruefung R3 B4): $GATE = die private, oben selbstgetestete Kopie des Gates.
    # Nachbesserung R4-1 (H1): Urteil aus Rueckgabe UND Ausgabe (gate_laufen, release/apk_pruefen.sh).
    local rc=0
    gate_laufen paket "$GATE" --repo "$(apk_nativ "$REPO")" --paket "$(apk_nativ "$out")" || rc=$?
    case "$rc" in
        0) ;;
        1) die "Paket $out weicht von der Asset-Liste ab (Befunde oben) - APK und PC-Pakete muessen dieselben Assets tragen" ;;
        *) die "Paketpruefung: keine Aussage moeglich (Urteil $rc, Meldung oben)" ;;
    esac
}

# =============================================================================
# check_runtime_assets — DAS LAUFZEIT-GATE (2026-08-24)
# =============================================================================
# $1 = fertiger Paketordner, $2 = Name der Binary darin ("re15_pc.exe"/"re15_pc")
#
# WARUM ES DAS GIBT — der 0.3.19-Fehler ging an ALLEN bisherigen Gates vorbei:
# check_tree prueft, ob Dateien im Paketordner LIEGEN. Sie lagen alle korrekt da.
# Der Fehler war, dass die exe sie zur LAUFZEIT nicht FAND: sie suchte am
# einkompilierten Pfad, der im Docker-Cross-Build auf den Container zeigt
# ("/src/re15_port/shared_assets/PSX") und beim Nutzer nicht existiert. Ergebnis
# beim Doppelklick: keine Spielschrift -> keine CONFIG-Labels, keine Untertitel,
# keine Dialoge, keine Item-Namen; dazu stumme Musik und fehlende Effekte.
# "Liegt da" und "wird gefunden" sind also ZWEI verschiedene Fragen — bisher hat
# das Gate nur die erste gestellt. Dieses hier stellt die zweite.
#
# ES REICHT NICHT, NUR "GEFUNDEN?" ZU FRAGEN. Auf der Entwickler-Maschine zeigt
# der einkompilierte Default INS REPO und rettet jedes noch so kaputte Paket.
# GEMESSEN 2026-08-24: Paketordner komplett ohne shared_assets/ -> die exe meldet
# trotzdem "RESULT ok=26 missing=0", weil alle 26 Treffer aus
# C:/workspace/git/reAi_v2/re15_port/shared_assets/ kamen. Ein Gate, das nur den
# Rueckgabewert prueft, waere dabei GRUEN gewesen.
# DESHALB ist die HERKUNFT die eigentliche Pruefung: jeder Treffer muss aus dem
# PAKETORDNER stammen. Genau das meldet der Selbsttest hinter "<-".
#
# ZWEI LAEUFE, weil es zwei verschiedene Nutzer-Situationen sind:
#   (1) cwd = Paketordner   -> der Doppelklick im Explorer
#   (2) cwd = fremdes Verzeichnis -> Verknuepfung, Steam, "Ausfuehren in ..."
# (2) ist der schaerfere Test: nur eine exe-relative Aufloesung besteht ihn.
#
# Die Umgebung wird GELEERT (env -u ...): das Paket muss aus eigener Kraft
# laufen. Sonst wuerde ausgerechnet die Variable, die den Fehler verdeckt hat,
# den Test bestehen lassen.
#
# Linux-Pakete werden nur geprueft, wenn das Binary hier ueberhaupt laufen kann
# (der Paketbau laeuft unter Windows/Git-Bash) — sonst uebersprungen mit Hinweis.
# =============================================================================
check_runtime_assets() {
    local out="$1" bin="$2"
    local exe="$out/$bin"
    [[ -x "$exe" || -f "$exe" ]] || die "Binary fehlt im Paket: $exe"

    # Kann diese Maschine das Binary starten? .exe nur unter Windows/MSYS/Cygwin,
    # ELF nur unter Linux. Kein Wunschdenken: lieber ehrlich ueberspringen.
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*) [[ "$bin" == *.exe ]] || {
            echo "   (Laufzeit-Gate uebersprungen: $bin ist kein Windows-Binary)"; return; } ;;
        Linux)                [[ "$bin" != *.exe ]] || {
            echo "   (Laufzeit-Gate uebersprungen: $bin laeuft nicht unter Linux)"; return; } ;;
        *) echo "   (Laufzeit-Gate uebersprungen: unbekanntes Host-System)"; return ;;
    esac

    # Der Paketordner als absoluter, normalisierter Pfad — damit ist er mit den
    # Pfaden vergleichbar, die der Selbsttest meldet (der schreibt immer '/').
    local pkg_abs
    pkg_abs="$(cd "$out" && pwd)"
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*) pkg_abs="$(cd "$out" && pwd -W 2>/dev/null || echo "$pkg_abs")" ;;
    esac
    pkg_abs="${pkg_abs%/}"

    local scratch run_dir log rc
    scratch="$(mktemp -d)"
    trap 'rm -rf "$scratch"' RETURN

    local pass
    for pass in in_pkg foreign_cwd; do
        if [[ "$pass" == in_pkg ]]; then
            run_dir="$out"                       # (1) Doppelklick: cwd = Paketordner
        else
            run_dir="$scratch/cwd"; mkdir -p "$run_dir"   # (2) Verknuepfung/Steam
        fi
        log="$run_dir/debug.log"
        rm -f "$log"

        # Der Selbsttest schreibt seinen Bericht nach stderr; stderr geht im Port
        # in debug.log im AKTUELLEN Arbeitsverzeichnis. Deshalb wird der Bericht
        # von dort gelesen und nicht von stdout (eine GUI-exe hat unter Git-Bash
        # oft gar keine Konsole, an die sie sich haengen koennte).
        #
        # MIT ZEITGRENZE UND AUFRAEUMEN, und zwar aus Erfahrung: kennt das Binary
        # den Schalter NICHT (alter Build), startet es statt des Selbsttests das
        # ganze SPIEL — ein Fenster, das nie von allein zurueckkehrt. GEMESSEN
        # 2026-08-24 mit dem 0.3.19-Binary: der Aufruf kehrte zurueck, der Prozess
        # lief WEITER. Ein haengender re15_pc blockiert danach den Linker.
        # Deshalb: im Hintergrund starten, auf den Selbsttest warten (er braucht
        # ~50 ms), und die PID am Ende in jedem Fall abraeumen.
        rc=0
        ( cd "$run_dir" && env -u RE15_ASSET_ROOT -u RE15_CD_ROOT -u RE15_RE2_ASSET_ROOT \
              RE15_ASSET_SELFTEST=1 "$(cd "$out" && pwd)/$bin" ) >/dev/null 2>&1 &
        local probe_pid=$! waited=0
        while (( waited < 300 )); do                       # 300 x 0.1 s = 30 s Deckel
            kill -0 "$probe_pid" 2>/dev/null || break
            grep -q '^\[selftest\] RESULT' "$log" 2>/dev/null && break
            sleep 0.1; waited=$((waited + 1))
        done
        if kill -0 "$probe_pid" 2>/dev/null; then
            kill -9 "$probe_pid" 2>/dev/null || true
            rc=124
        else
            wait "$probe_pid" 2>/dev/null || rc=$?
        fi
        # Sicherheitsnetz: der GUI-Prozess kann den Shell-Job ueberleben (genau so
        # geschehen). Was dieser Lauf gestartet hat, wird hier zuverlaessig beendet.
        # ⛔ NUR die exe AUS DIESEM Lauf-Ordner beenden (Runde 32, 2026-09-29): das fruehere
        # "taskkill //F //IM $bin" beendete jede gleichnamige exe der Maschine — auch Messlaeufe
        # paralleler Agenten/Sitzungen (dieselbe Falle wie local_build.sh, Commit 53bf3f3a).
        case "$(uname -s)" in
            MINGW*|MSYS*|CYGWIN*)
                local _pw; _pw="$(cygpath -w "$(cd "$out" && pwd)/$bin" 2>/dev/null || true)"
                if [[ -n "$_pw" ]] && command -v powershell >/dev/null 2>&1; then
                    powershell -NoProfile -Command \
                      "Get-Process -ErrorAction SilentlyContinue | Where-Object { \$_.Path -and \$_.Path -ieq '$_pw' } | Stop-Process -Force" \
                      >/dev/null 2>&1 || true
                fi ;;
        esac

        [[ -f "$log" ]] || die "Laufzeit-Gate ($pass): das Binary hat keine debug.log
        geschrieben. Entweder ist es sofort abgestuerzt, oder es kennt
        RE15_ASSET_SELFTEST nicht — dann stammt es aus einem Build VOR dem
        2026-08-24-Fix und gehoert nicht in dieses Paket."

        local result
        result="$(grep -m1 '^\[selftest\] RESULT' "$log" || true)"
        [[ -n "$result" ]] || die "Laufzeit-Gate ($pass): keine '[selftest] RESULT'-Zeile in
        $log. Das Binary kennt RE15_ASSET_SELFTEST nicht -> es ist ein alter Build."

        # (a) Wurde alles gefunden?
        if [[ "$result" != *"missing=0"* ]]; then
            echo "--- fehlende Assets ($pass) ---" >&2
            grep '^\[selftest\] MISSING' "$log" >&2 || true
            die "Laufzeit-Gate ($pass): $result
        Das Paket startet beim Nutzer, findet aber die oben gelisteten Assets
        NICHT. Genau das war der 0.3.19-Fehler."
        fi

        # (b) DIE EIGENTLICHE PRUEFUNG: kam auch alles aus dem PAKET?
        #     Ein Treffer aus dem Repo bedeutet, dass das Paket beim Nutzer —
        #     der dieses Repo nicht hat — an derselben Stelle scheitern wird.
        local strays
        strays="$(grep '^\[selftest\] OK' "$log" | sed 's/.*<- //' \
                  | grep -v -F "$pkg_abs/" || true)"
        if [[ -n "$strays" ]]; then
            echo "--- Treffer AUSSERHALB des Pakets ($pass) ---" >&2
            echo "$strays" | sort -u >&2
            die "Laufzeit-Gate ($pass): das Paket laedt Assets von ausserhalb
        ($pkg_abs). Auf DIESER Maschine faellt das nicht auf — beim Nutzer
        gibt es diese Pfade nicht. Das ist exakt die Taeuschung, die den
        0.3.19-Fehler bis zur Auslieferung durchgelassen hat."
        fi

        (( rc == 0 )) || die "Laufzeit-Gate ($pass): Selbsttest-Exitcode $rc trotz $result"

        local n
        n="$(grep -c '^\[selftest\] OK' "$log")"
        echo "   Laufzeit-Gate $pass: $n/$n Assets, alle aus dem Paket (cwd=$(basename "$run_dir"))"
        rm -f "$log"
    done
}

# --- Gemeinsames Einsammeln --------------------------------------------------
[[ -d "$ASSETS" ]] || die "Asset-Baum fehlt: $ASSETS"
[[ -d "$FX"     ]] || die "Effekt-Texturen fehlen: $FX"
[[ -s "$RE2/CDEMD0.EMS" && -s "$RE2/ENEMSE.VBS" && -s "$RE2/TORSE.VBS" && -s "$RE2/CORE00.ESP" && -s "$RE2/TEX.TIM" ]] || die "RE2-Assets fehlen: $RE2 (CORE00.ESP/TEX.TIM = RE2-FX der Granaten, Runde 34)"
[[ -s "$SYNCHRO/STAGE1/room1170/main00.wav" ]] || die "Voiceover-Quelle fehlt: $SYNCHRO/STAGE1"

# --- Python (Quellbaum/Paket-Pruefung, verify_split, zip_exec_bit.py) --------
# ⛔ NIE blind "python3"/"python" (Runde 34a): unter Git-Bash ist "python3" zuerst der
# WindowsApps-Alias - der Aufruf in verify_split hat in v0.8.17 ungefragt Python 3.14
# installiert, v0.8.19 lief nur mit einem Shim im PATH. python_finden.sh verwirft den Alias,
# ohne ihn zu starten, und setzt PY. Seit Nachbesserung R2 IMMER noetig (--paket/--quellbaum,
# auch mit --no-zip) - VOR den Kopierminuten suchen.
# shellcheck source=python_finden.sh
source "$HERE/python_finden.sh" \
    || die "kein echtes Python >= 3.8 (release/python_finden.sh) - Quellbaum-/Paketpruefung, verify_split und zip_exec_bit.py brauchen es"
# shellcheck source=apk_pruefen.sh
source "$HERE/apk_pruefen.sh"                # apk_nativ, apk_kennung, apk_pruefen (Android-Satz unten)

# --- Asset-Gate: private Kopie, Selbsttest VOR der ersten Nutzung (Runde 4, Gegenpruefung R3 B4) ----
# Bis dahin lief --selbsttest nur in apk_pruefen, also nur mit APK: der PC-Pfad (--quellbaum hier, --paket
# in check_tree) benutzte ein Gate, dessen Selbsttest in diesem Lauf niemand gefahren hatte. Ein Gate ohne
# Paket-sha-Vergleich (Mutant MUP, sein eigener Selbsttest: FEHLER 3/202) liess so ein Paket mit veraenderter
# Datei durch (Pruefer R3: echter Lauf --zip-only --only linux EXIT 0). Jetzt: EINE Kopie des Gates in einem
# privaten Temp-Ordner, Selbsttest auf ihr, und genau sie macht alle Pruefungen dieses Laufs (--quellbaum,
# --paket und ueber APK_GATE_DATEI auch apk_pruefen) - ein waehrend der Kopierminuten getauschtes
# release/apk_asset_gate.py wirkt nicht mehr. Die EXIT-Falle raeumt Gate-Kopie und APK-Pruefkopie ab (auch bei
# Abbruch). ⛔ Den Temp-Ordner als WINDOWS-Pfad (cygpath -m) festhalten: der Zip-Abschnitt stellt /c/msys64/usr/bin
# vorn in den PATH, danach ist "rm" das rm von MSYS2 - dessen /tmp ist C:/msys64/tmp, nicht der Temp-Ordner von
# Git-Bash. Mit dem /tmp-Pfad liess die EXIT-Falle nach JEDEM erfolgreichen Zip-Lauf die Gate-Kopie liegen (gemessen:
# 8 Reste, nur Laeufe mit Zippen; Abbrueche und --no-zip raeumten ab).
# Nachbesserung R4-1 (Gegenpruefung H1): die Kopie muss dem Pin release/apk_asset_gate.sha256 gleichen (gate_festhalten),
# und JEDES Gate-Urteil kommt aus Rueckgabe UND Ausgabe (gate_laufen, release/apk_pruefen.sh) - bis dahin lieferte ein
# 0-Byte-Gate oder eines mit 'main()' ohne sys.exit hier ein Paket mit veraenderter Datei aus (SHA256SUMS + git add).
MP_TMP="$(mktemp -d "${TMPDIR:-/tmp}/re15_make_package.XXXXXX")" || die "kein Temp-Ordner fuer die Gate-Kopie"
MP_TMP="$(apk_nativ "$MP_TMP")"
mp_aufraeumen() {
    apk_pruefen_aufraeumen
    if [[ -n "${MP_TMP:-}" && -d "$MP_TMP" ]]; then rm -rf "$MP_TMP"; fi
}
trap mp_aufraeumen EXIT
echo "== Asset-Gate: festhalten und Selbsttest der privaten Kopie (release/apk_asset_gate.py --selbsttest) =="
gate_festhalten "$MP_TMP"
GATE="$GATE_KOPIE"
APK_GATE_KOPIE="$GATE"                       # apk_pruefen.sh (Schritt 5) nimmt dieselbe Kopie (nie aus der Umgebung)
rc_selbst=0
gate_laufen selbsttest "$GATE" --selbsttest || rc_selbst=$?
(( rc_selbst == 0 )) || die "Selbsttest des Asset-Gates nicht bestanden (Urteil $rc_selbst, Meldung oben) - dem Gate ist
        nicht zu trauen; Quellbaum-, APK- und Paketpruefung unterbleiben, nichts wird kopiert oder gezippt"

# --- Quellbaum: Asset-Liste + Tuer-Soll (Nachbesserung R2, Gegenpruefung B2/B6) -------
# VOR den Kopierminuten: fehlt ein Tuerarchiv, das die Engine-Tabellen verlangen (29/30), ist eines
# 0 Byte oder passt es nicht zu Groesse/FNV-1a, oder liegt unter re15_port/shared_assets ein Ordner,
# den keine Liste kennt, waeren ALLE Pakete falsch - check_tree verglich bisher nur Paket gegen Quelle.
echo "== Quellbaum: Asset-Liste, Tuer-Soll (release/apk_asset_gate.py --quellbaum) =="
rc_quelle=0
gate_laufen quellbaum "$GATE" --repo "$(apk_nativ "$REPO")" --quellbaum || rc_quelle=$?
case "$rc_quelle" in
    0) ;;
    1) die "Quellbaum weicht von der Asset-Liste bzw. den Tuer-Tabellen der Engine ab (Befunde oben)" ;;
    *) die "Quellbaum-Pruefung: keine Aussage moeglich (Urteil $rc_quelle, Meldung oben)" ;;
esac

# --- Android-APK: DIESELBE Pruefkette wie in build_android.sh ------------------
# (Runde 34a) Zwischen Android-Bau und Paket kann sich der Quellbaum geaendert haben (neues
# Asset, neues Tuerarchiv) - die APK waere dann veraltet und wuerde trotzdem gezippt. Deshalb
# hier noch einmal gegen den AKTUELLEN Stand, VOR den Kopierminuten. Fehlt die APK: kein
# Android-Satz aus diesem Lauf - und ein alter derselben Version wird nicht mitgeliefert (unten, B1).
# Nachbesserung R1 (Gegenpruefung B2/B4/B5, echtlauf B4): bis dahin nur die Assets. Jetzt auch
#   * Frische: die APK muss neuer sein als der letzte Commit an IHREM Code (engine, include,
#     platform/pc - der Android-Bau uebersetzt dieselben Plattformquellen, jni/CMakeLists.txt:42 -
#     und platform/android) - wie check_binary_fresh fuer die PC-Binaries;
#   * versionName = --version, Paketname, ABIs (aapt) und eine gueltige v2/v3-Signatur (apksigner);
#   * Identitaet: sha256/CRC32/Groesse der GEPRUEFTEN Bytes werden festgehalten; gezippt wird nur,
#     wenn die Datei beim Zippen noch genau diese ist, und aus dem fertigen Split-Satz wird die APK wieder
#     entpackt: ihre sha256 muss die der geprueften Kopie sein (Runde 4, Gegenpruefung R3 B3 - vorher nur
#     CRC32 + Groesse aus dem Katalog). Vorher lagen zwischen Pruefen (hier) und Zippen (unten)
#     Minuten Kopieren - eine in der Zeit getauschte oder erst dann abgelegte APK ging ungeprueft durch.
# (apk_pruefen.sh ist oben geladen: apk_werkzeuge_finden, apk_pruefen, apk_kennung)
APK_PRUEF="$HERE/${NAME}_android.apk"
APK_KENNUNG=""
if [[ $DO_ZIP -eq 1 && $OHNE_ANDROID -eq 0 && -f "$APK_PRUEF" ]]; then
    echo "== Android-APK: Frische, Version, Ausrichtung, Signatur, volle Asset-Pruefung (release/apk_pruefen.sh) =="
    check_binary_fresh "$APK_PRUEF" "Android-APK" \
        re15_port/engine re15_port/include re15_port/platform/pc re15_port/platform/android
    # (Pruefkopie ~360 MB: die EXIT-Falle mp_aufraeumen oben raeumt sie auch bei Abbruch ab)
    apk_werkzeuge_finden
    apk_pruefen "$APK_PRUEF" "$VERSION" "$REPO"
    # Nachbesserung R2 (Gegenpruefung B3): die Kennung ist die der PRUEFKOPIE, an der JEDER Schritt lief -
    # bis dahin nahm dieses Skript sie erst NACH der Pruefung vom Pfad, und eine waehrend des Selbsttests
    # getauschte (unsignierte) APK wurde so zur "geprueften". apk_pruefen hat am Ende auch verglichen,
    # dass unter dem Pfad noch dieselben Bytes liegen; beim Zippen wird unten noch einmal verglichen.
    APK_KENNUNG="$APK_GEPRUEFT_KENNUNG"
    apk_pruefen_aufraeumen
    [[ "$APK_KENNUNG" =~ ^[0-9a-f]{64}\ [0-9a-f]{8}\ [0-9]+$ ]] || die "Kennung der APK unlesbar: '$APK_KENNUNG'"
    echo "   gepruefte APK (sha256 crc32 Bytes): $APK_KENNUNG"
fi

# --- Android-Satz NUR aus diesem Lauf (Runde 4, Gegenpruefung R3 B1) ----------------------------------
# Bis dahin nahmen SHA256SUMS.txt (sha256sum "${NAME}"_*.z*) und git add JEDEN vorhandenen
# <NAME>_android.z* mit - auch den eines frueheren Laufs derselben Version, dessen APK dieses Skript im
# Lauf davor selbst als "weicht vom Quellbaum ab" abgelehnt hatte (Pruefer R3, r3_mp_veraltet.sh: Lauf ohne
# APK EXIT 0, der alte Satz in SHA256SUMS.txt und git-vorgemerkt). Jetzt gilt: ausgeliefert wird ein
# Android-Satz nur, wenn er in DIESEM Lauf aus der gerade geprueften APK entsteht (ANDROID_GEZIPPT, Zippen
# unten). Liegt ohne gepruefte APK ein Satz derselben Version da, bricht das Skript HIER ab, vor den
# Kopierminuten. Warum Abbruch und nicht still entfernen: der Satz einer Version liegt nach dem
# Release-Commit versioniert im Repo (v0.8.19: git ls-files) - ein Lauf, dem nur die APK fehlt, soll ihn
# weder ungefragt loeschen noch ungeprueft mitliefern. Wer bewusst nur die PC-Saetze will, sagt
# --ohne-android; dann entfernt das Skript den alten Satz (Datei hier beim Zippen, git-Index im Git-Schritt).
android_satz_da() {          # Volumes <NAME>_android.z* in release/ (Namen, je Zeile)
    local f
    for f in "$HERE/${NAME}_android".z*; do
        if [[ -e "$f" ]]; then printf '%s\n' "$(basename "$f")"; fi
    done
    return 0
}
ANDROID_ALT="$(android_satz_da)"
if [[ $DO_ZIP -eq 1 && $OHNE_ANDROID -eq 0 && -z "$APK_KENNUNG" && -n "$ANDROID_ALT" ]]; then
    die "Android-Satz DIESER Version liegt vor, aber in diesem Lauf gibt es keine gepruefte APK
        ($(basename "$APK_PRUEF") fehlt): $(echo $ANDROID_ALT)
        Er stammt aus einem frueheren Lauf und passt nicht nachweislich zum jetzigen Quellbaum - ungeprueft kommt
        er weder in SHA256SUMS.txt noch in git. Entweder die APK neu bauen:
            release/build_android.sh --version $VERSION      (danach prueft und zippt dieses Skript sie neu)
        oder nur die PC-Saetze schnueren und den alten Satz entfernen lassen:
            release/make_package.sh --version $VERSION --ohne-android ..."
fi
if [[ $OHNE_ANDROID -eq 1 ]]; then
    if [[ -n "$ANDROID_ALT" ]]; then
        echo "== --ohne-android: keine APK-Pruefung, kein Android-Satz; der Satz dieser Version wird entfernt: $(echo $ANDROID_ALT) =="
    else
        echo "== --ohne-android: keine APK-Pruefung, kein Android-Satz (keiner dieser Version vorhanden) =="
    fi
fi

# --- Fremde Dateien mit dem Versions-Praefix (Nachbesserung R4-1, Gegenpruefung H3) ----------------------------------
# B1 schuetzte nur den kanonischen Namen <NAME>_android.z* (Glob, gross/klein-genau). SHA256SUMS.txt und git add nahmen
# per Glob "${NAME}"_*.z* aber JEDE Datei mit dem Praefix mit - ein re15_port_<v>_ANDROID.zip oder _android.apk.zip
# (Pruefer: je ein Zip mit einer Text-"APK") stand danach in SHA256SUMS.txt und war git-vorgemerkt, auch mit
# --ohne-android. Jetzt: ausgeliefert wird nur eine Positivliste (Zippen unten), und jede Datei <NAME>_*.z*, die keinem
# Satz dieses Skripts gehoert (<NAME>_{linux_steamdeck_x64,win64,android}.zip/.zNN, gross/klein-genau), bricht ab - hier
# vor den Kopierminuten und noch einmal vor dem Schreiben von SHA256SUMS.txt.
fremde_versionsdateien() {   # Namen (je Zeile) der Dateien <NAME>_*.z* in release/, die keinem Satz gehoeren
    local f b rest
    for f in "$HERE/${NAME}"_*.z*; do
        [[ -e "$f" ]] || continue
        b="$(basename "$f")"
        rest="${b#"${NAME}_"}"
        [[ "$rest" =~ ^(linux_steamdeck_x64|win64|android)\.(zip|z[0-9][0-9]+)$ ]] && continue
        printf '%s\n' "$b"
    done
    return 0
}
fremde_abbruch() {           # $1 = Zeitpunkt (Text)
    local fremd
    fremd="$(fremde_versionsdateien)"
    [[ -z "$fremd" ]] || die "fremde Datei(en) mit dem Versions-Praefix in release/ ($1): $(echo $fremd)
        Sie gehoeren zu keinem Satz dieses Skripts (${NAME}_linux_steamdeck_x64|win64|android als .zip/.zNN) und kaemen
        sonst ungeprueft in SHA256SUMS.txt bzw. git. Entfernen oder umbenennen, dann neu starten."
}
if [[ $DO_ZIP -eq 1 ]]; then fremde_abbruch "vor den Kopierminuten"; fi

copy_common() {          # $1 = Paketordner
    local out="$1"
    echo "   Assets kopieren (shared_assets/PSX, ~283 MB) ..."
    mkdir -p "$out/shared_assets"
    cp -r "$ASSETS" "$out/shared_assets/PSX"
    echo "   Effekt-Texturen kopieren (shared_assets/extracted_fx) ..."
    cp -r "$FX" "$out/shared_assets/extracted_fx"
    echo "   RE2-Assets kopieren (shared_assets/RE2, ~18 MB, fuer OPTIONS->AI=RE2) ..."
    cp -r "$RE2" "$out/shared_assets/RE2"
    echo "   Port-Tuerarchive kopieren (shared_assets/RE15DOOR, Runde 33) ..."
    cp -r "$RE15DOOR" "$out/shared_assets/RE15DOOR"
    echo "   Voiceover kopieren (synchro/STAGE*, ohne unused/) ..."
    # Paket-Wurzel, NICHT unter shared_assets: re15_pc_read_base() sucht
    # "<Basis-Wurzel>/synchro/..." und die Basis-Wurzel ist das exe-Verzeichnis.
    # synchro/unused/ bleibt draussen — kein Codepfad liest es.
    # ALLE Stages, nicht nur STAGE1: der Loader baut den Pfad aus dem Raum
    # (audio_pc.c re15_voice_load_clip: "synchro/STAGE%u/room%04X/main%02d.wav",
    # Stage = room>>12). Mit der alten STAGE1-Kopie waere jede Aufnahme fuer
    # STAGE2..6 im Repo gelandet und im Paket verschwunden — genau die Falle, die
    # bis v0.3.16 schon einmal das GANZE synchro-Verzeichnis gekostet hat.
    mkdir -p "$out/synchro"
    for stagedir in "$SYNCHRO"/STAGE*; do
        [[ -d "$stagedir" ]] || continue
        cp -r "$stagedir" "$out/synchro/$(basename "$stagedir")"
    done
}

render_readme() {        # $1 = Vorlage, $2 = Ziel
    sed "s/@VERSION@/${VERSION#v}/g" "$1" > "$2"
}

# Split-Zips lassen sich NICHT mit `zip -T` pruefen ("cannot update a split
# archive") und `unzip -t` bricht auf den Volumes ab. Geprueft wird deshalb der
# zentrale Katalog des LETZTEN Volumes: er listet alle Eintraege des gesamten
# Satzes und nennt die Volume-Nummer jedes Eintrags — daran haengt, ob der Satz
# vollstaendig ist.
verify_split() {         # $1 = .zip (letztes Volume), $2 = erwartete Dateizahl
    "$PY" - "$1" "$2" <<'PY'
import struct, sys, glob, os
last, want = sys.argv[1], int(sys.argv[2])
d = open(last, 'rb').read()
i = d.rfind(b'PK\x05\x06')
if i < 0: sys.exit("kein End-of-Central-Directory gefunden")
disk, cd_disk, here, total = struct.unpack('<HHHH', d[i+4:i+12])
vols = sorted(glob.glob(os.path.splitext(last)[0] + '.z*'))
print(f"   Volumes: {len(vols)} (letztes = Nr. {disk+1}), Eintraege im Katalog: {total}")
missing = [n for n in range(1, disk+1)
           if not os.path.exists(f"{os.path.splitext(last)[0]}.z{n:02d}")]
if missing: sys.exit(f"fehlende Volumes: {missing}")
# Runde 4 (B1): auch KEINE fremden - vor dem Zippen wurde <satz>.z* geloescht; was jetzt mehr da ist als die
# Volumes 1..disk und das letzte, stammt nicht aus diesem zip-Lauf und kaeme sonst in SHA256SUMS.txt/git add
soll = {os.path.normcase(f"{os.path.splitext(last)[0]}.z{n:02d}") for n in range(1, disk+1)} | {os.path.normcase(last)}
fremd = [v for v in vols if os.path.normcase(v) not in soll]
if fremd: sys.exit(f"fremde Dateien neben dem Satz (nicht aus diesem zip-Lauf): {fremd}")
if total < want: sys.exit(f"Katalog listet nur {total} Eintraege, erwartet >= {want}")
PY
}

# Nachbesserung R1 (B5): steckt im fertigen Split-Satz wirklich die GEPRUEFTE APK? Stufe 1: der Katalog im
# letzten Volume nennt genau den einen Eintrag, CRC32 und Groesse muessen zur Kennung passen, die vor den
# Kopierminuten von genau den geprueften Bytes genommen wurde.
# Runde 4 (Gegenpruefung R3 B3): CRC32 + Groesse allein halten eine CRC-gleiche Faelschung nicht fest
# (r3_zip_kennung_sonde.sh: 1 Byte + 4 Ausgleichsbytes in RE15DOOR/P07G.DO2, CRC32/Groesse gleich, sha256
# anders -> "= gepruefte APK", apksigner DOES NOT VERIFY). Stufe 2: den Satz zusammenfuehren (zip -s 0 -
# unzip liest keine Split-Saetze), die APK mit unzip ENTPACKEN (prueft dabei ihre CRC32) und ihre sha256 mit
# der der geprueften Kopie vergleichen. Zip/unzip bekommen Windows-Pfade (cygpath -m): das zip aus
# /c/msys64/usr/bin hat eine eigene Einhaengetabelle, in der /tmp NICHT der Temp-Ordner von Git-Bash ist
# (gemessen: "zip error: Nothing to do!"). Temp-Platz ~ Groesse des Satzes; Rueckgabe != 0 bei jedem Fehler.
verify_apk_im_zip() {    # $1 = .zip (letztes Volume), $2 = Eintragsname, $3 = "sha256 crc32 groesse"
    local satz="$1" name="$2" kennung="$3" tmp tmp_n rc=0 liste="" sha_ist=""
    "$PY" - "$satz" "$name" "$kennung" <<'PY' || return 1
import struct, sys
last, name, kennung = sys.argv[1], sys.argv[2].encode("utf-8"), sys.argv[3].split()
crc_soll, n_soll = int(kennung[1], 16), int(kennung[2])
d = open(last, "rb").read()
i = d.rfind(b"PK\x05\x06")
if i < 0: sys.exit("APK-Satz: kein End-of-Central-Directory")
disk, cd_disk, here, total, cd_size, cd_off = struct.unpack("<HHHHII", d[i + 4:i + 20])
if cd_disk != disk or here != total: sys.exit("APK-Satz: Katalog nicht vollstaendig im letzten Volume")
p, treffer = cd_off, []
for _ in range(total):
    if d[p:p + 4] != b"PK\x01\x02": sys.exit("APK-Satz: Katalog kaputt")
    crc, _cs, usize = struct.unpack("<III", d[p + 16:p + 28])
    nlen, xlen, klen = struct.unpack("<HHH", d[p + 28:p + 34])
    treffer.append((d[p + 46:p + 46 + nlen], crc, usize))
    p += 46 + nlen + xlen + klen
if len(treffer) != 1 or treffer[0][0] != name:
    sys.exit("APK-Satz: erwartet genau den Eintrag %r, Katalog: %r" % (name, [t[0] for t in treffer]))
_n, crc, usize = treffer[0]
if (crc, usize) != (crc_soll, n_soll):
    sys.exit("APK-Satz: Eintrag hat CRC32 %08x / %d B, gepruefte APK %08x / %d B - NICHT die gepruefte Datei"
             % (crc, usize, crc_soll, n_soll))
print("   APK-Satz, Katalog: genau %s, CRC32 %08x, %d B = gepruefte Kennung" % (name.decode("utf-8"), crc, usize))
PY
    tmp="$(mktemp -d "${TMPDIR:-/tmp}/re15_apk_satz.XXXXXX")" || { echo "APK-Satz: kein Temp-Ordner" >&2; return 1; }
    # ab hier NUR der Windows-Pfad (zip, unzip, rm): mktemp/cygpath/rm koennen aus verschiedenen MSYS-Laufzeiten stammen
    tmp_n="$tmp"
    if command -v cygpath >/dev/null 2>&1; then tmp_n="$(cygpath -m "$tmp")" || tmp_n="$tmp"; fi
    zip -q -s 0 "$satz" --out "$tmp_n/ganz.zip" || rc=$?
    if (( rc == 0 )); then liste="$(unzip -Z1 "$tmp_n/ganz.zip")" || rc=$?; fi
    if (( rc == 0 )) && [[ "$liste" != "$name" ]]; then
        rm -rf "$tmp_n"
        echo "APK-Satz: zusammengefuehrt enthaelt er [$(echo $liste)] statt genau $name" >&2
        return 1
    fi
    # alle Eintraege nach stdout = genau der eine (eben geprueft) - kein Namensmuster, das unzip auswerten wuerde
    if (( rc == 0 )); then sha_ist="$(unzip -p "$tmp_n/ganz.zip" | sha256sum)" || rc=$?; fi
    rm -rf "$tmp_n"
    if (( rc != 0 )); then
        echo "APK-Satz: zusammenfuehren (zip -s 0) oder entpacken (unzip) fehlgeschlagen (rc=$rc)" >&2
        return 1
    fi
    sha_ist="${sha_ist%% *}"
    if [[ "$sha_ist" != "${kennung%% *}" ]]; then
        echo "APK-Satz: die entpackte APK hat sha256 $sha_ist, die gepruefte ${kennung%% *} - NICHT die gepruefte Datei" >&2
        return 1
    fi
    echo "   APK im Split-Satz = gepruefte APK (entpackt: sha256 ${sha_ist:0:16}... = Pruefkopie)"
}

# --- Linux / Steam Deck ------------------------------------------------------
if [[ "$ONLY" == "both" || "$ONLY" == "linux" ]]; then
    [[ -f "$LINUX_BIN" ]] || die "Linux-Binary fehlt: $LINUX_BIN (release/build_linux_deck.sh)"
    echo "== Linux/Steam-Deck-Paket: $NAME =="
    check_binary_paths "$LINUX_BIN"
    check_binary_fresh "$LINUX_BIN" "Linux-Binary"
    check_binary_optimiert "$LINUX_BIN" "Linux-Binary"
    check_glibc        "$LINUX_BIN"

    OUT="$HERE/pkg-linux/$NAME"
    if [[ $ZIP_ONLY -eq 0 ]]; then
        rm -rf "$HERE/pkg-linux"; mkdir -p "$OUT"
        install -m 755 "$LINUX_BIN"            "$OUT/re15_pc"
        # ⛔ ZEILENENDEN NORMALISIEREN (2026-08-25, Nutzer-Report "du baust das shell Skript
        # run.sh in crlf statt in lf"): .gitattributes hat zwar `*.sh text eol=lf`, das
        # normalisiert aber nur, was git SPEICHERT — eine vom Editor/Werkzeug mit CRLF
        # geschriebene Datei bleibt im ARBEITSBAUM CRLF, und genau von dort kopiert dieses
        # Skript. Auf dem Deck scheitert so ein run.sh schon am Shebang
        # ("/usr/bin/env bash\r: Datei oder Verzeichnis nicht gefunden"). Deshalb hier beim
        # Kopieren hart auf LF ziehen statt sich auf den Arbeitsbaum zu verlassen.
        tr -d '\r' < "$HERE/pkg_files/linux/run.sh" > "$OUT/run.sh"
        chmod 755 "$OUT/run.sh"
        render_readme "$HERE/pkg_files/linux/README.txt.in" "$OUT/README_${VERSION#v}.txt"
        copy_common "$OUT"
    fi
    check_tree  "$OUT"
    check_lf    "$OUT"
    check_runtime_assets "$OUT" "re15_pc"
    LINUX_FILES=$(find "$OUT" -type f | wc -l)
    echo "   OK: $(du -sh "$OUT" | cut -f1), $LINUX_FILES Dateien"
fi

# --- Windows -----------------------------------------------------------------
if [[ "$ONLY" == "both" || "$ONLY" == "win" ]]; then
    [[ -f "$WIN_BIN" ]] || die "Windows-Binary fehlt: $WIN_BIN"
    echo "== Windows-Paket: $NAME =="
    check_binary_paths "$WIN_BIN"
    check_binary_fresh "$WIN_BIN" "Windows-Binary"
    check_binary_optimiert "$WIN_BIN" "Windows-Binary"

    OUT="$HERE/pkg-win/$NAME"
    if [[ $ZIP_ONLY -eq 0 ]]; then
        rm -rf "$HERE/pkg-win"; mkdir -p "$OUT"
        install -m 755 "$WIN_BIN"                          "$OUT/re15_pc.exe"
        install -m 644 "$HERE/pkg_files/win/Start_RE15_Port.bat" "$OUT/Start_RE15_Port.bat"
        # Diagnose-Starter: setzt die Trace-Haken und sichert das Log unter eigenem Namen.
        # Damit kann der Nutzer einen gemeldeten Fehler selbst belegen, statt dass ich
        # seinen Spielzustand raten muss.
        install -m 644 "$HERE/pkg_files/win/Diagnose_ROOM1090.bat" "$OUT/Diagnose_ROOM1090.bat"
        render_readme "$HERE/pkg_files/win/README.txt.in"   "$OUT/README_${VERSION#v}.txt"
        copy_common "$OUT"
    fi
    check_tree  "$OUT"
    check_runtime_assets "$OUT" "re15_pc.exe"
    WIN_FILES=$(find "$OUT" -type f | wc -l)
    echo "   OK: $(du -sh "$OUT" | cut -f1), $WIN_FILES Dateien"
fi

# --- Zippen (Split-Volumes) --------------------------------------------------
AUSLIEFERN=()                     # PC-Volumes fuer SHA256SUMS.txt + git add (Positivliste, Nachbesserung R4-1 H3)
ANDROID_VOLUMES=()                # Android-Volumes aus DIESEM Lauf (nur mit ANDROID_GEZIPPT)
if [[ $DO_ZIP -eq 1 ]]; then
    # ⛔ zip LIEGT IN msys64/usr/bin, NICHT IN mingw64/bin (CLAUDE.md, Build-Kapitel).
    # Aus einer Shell, die nur mingw64/bin im PATH hat, starb der Paketlauf am
    # 2026-09-05 mit "ABBRUCH: zip fehlt" - NACHDEM beide Ordner samt aller Gates
    # fertig waren. Der Pfad wird deshalb hier nachgetragen, statt ihn beim Aufruf
    # setzen zu muessen.
    if ! command -v zip >/dev/null 2>&1 && [[ -x /c/msys64/usr/bin/zip ]]; then
        export PATH="/c/msys64/usr/bin:$PATH"
    fi
    command -v zip >/dev/null 2>&1 || die "zip fehlt (auch nicht in /c/msys64/usr/bin)"
    cd "$HERE"
    if [[ "$ONLY" == "both" || "$ONLY" == "linux" ]]; then
        echo "== Zippen: ${NAME}_linux_steamdeck_x64 =="
        rm -f "${NAME}_linux_steamdeck_x64".z*
        ( cd pkg-linux && zip -q -s "$SPLIT" -r "../${NAME}_linux_steamdeck_x64.zip" "$NAME" )
        verify_split "${NAME}_linux_steamdeck_x64.zip" "$LINUX_FILES"
        # ⛔ AUSFUEHRUNGSBIT — auf einem Windows-Bauhost kommt es NICHT von allein.
        # Gemessen am 2026-09-03: re15_pc lag mit 0644 im Archiv, weil MSYS das Bit ohne
        # ACLs aus Shebang/Endung ableitet (run.sh kommt darum zufaellig richtig heraus,
        # ein ELF-Binary nie) und `chmod`/`install -m` still wirkungslos bleiben. Wer
        # entpackt und das Binary direkt startet, bekam "Permission denied".
        # Gesetzt wird im Zentralverzeichnis des letzten Volumes, danach wird
        # zurueckgelesen: ohne das Gate kehrt der Fehler beim naechsten Bau still wieder.
        "$PY" "$HERE/zip_exec_bit.py" setzen \
            "${NAME}_linux_steamdeck_x64.zip" re15_pc run.sh \
            || die "x-Bit konnte nicht gesetzt werden"
        "$PY" "$HERE/zip_exec_bit.py" pruefen \
            "${NAME}_linux_steamdeck_x64.zip" re15_pc run.sh \
            || die "Gate: Linux-Archiv liefert eine Datei ohne Ausfuehrungsbit"
    fi
    if [[ "$ONLY" == "both" || "$ONLY" == "win" ]]; then
        echo "== Zippen: ${NAME}_win64 =="
        rm -f "${NAME}_win64".z*
        ( cd pkg-win && zip -q -s "$SPLIT" -r "../${NAME}_win64.zip" "$NAME" )
        verify_split "${NAME}_win64.zip" "$WIN_FILES"
    fi
    # --- Android: die APK in denselben Split-Satz bringen ---------------------
    # Nutzer-Vorgabe 2026-09-19: "Beim apk, splitte es ebenfalls in mehrere Teile,
    # das du es ins repo laden kannst." Die APK selbst ist ~358 MB und liegt damit
    # weit ueber GitHubs Dateigrenze (100 MB hart, 50 MB Warnung); als Split-Zip mit
    # derselben Volume-Groesse wie die anderen Pakete passt sie hinein.
    # Sie wird NICHT von diesem Skript gebaut (das macht release/build_android.sh) —
    # fehlt sie, wird das gesagt und uebersprungen, nicht stillschweigend ausgelassen.
    # -j (junk paths) legt die APK in die Archivwurzel, damit beim Entpacken keine
    # release/-Schachtel entsteht.
    APK="$HERE/${NAME}_android.apk"
    if [[ $OHNE_ANDROID -eq 1 ]]; then
        # --ohne-android (Runde 4, B1): kein Android-Satz aus diesem Lauf, der dieser Version wird entfernt
        # (hier die Dateien; den git-Index raeumt der Git-Schritt unten ueber behalten())
        if [[ -f "$APK" ]]; then
            echo "   (--ohne-android: $(basename "$APK") bleibt ungeprueft liegen und wird nicht gezippt)"
        fi
        for f in "${NAME}_android".z*; do
            [[ -e "$f" ]] || continue
            rm -f "$f" || die "alter Android-Satz nicht entfernbar: $f"
            echo "   (--ohne-android: alter Android-Satz entfernt: $f)"
        done
    elif [[ -f "$APK" ]]; then
        # Nur die GEPRUEFTE APK (Nachbesserung R1, Gegenpruefung B5): dieselbe Datei wie oben?
        [[ -n "$APK_KENNUNG" ]] || die "Android-APK $APK ist erst NACH der Pruefung aufgetaucht -
        ungeprueft wird nichts gezippt. make_package.sh neu starten."
        command -v unzip >/dev/null 2>&1 && command -v sha256sum >/dev/null 2>&1 \
            || die "unzip/sha256sum fehlen - ohne sie ist nicht pruefbar, ob der Android-Satz die gepruefte APK enthaelt"
        apk_jetzt="$(apk_kennung "$APK")" || die "Android-APK beim Zippen nicht lesbar: $APK"
        [[ "$apk_jetzt" == "$APK_KENNUNG" ]] || die "Android-APK wurde nach der Pruefung veraendert oder ersetzt:
        geprueft: $APK_KENNUNG
        jetzt:    $apk_jetzt
        Nichts gezippt. make_package.sh neu starten (prueft dann die jetzige Datei)."
        echo "== Zippen: ${NAME}_android (APK $(du -h "$APK" | cut -f1), = gepruefte Datei) =="
        rm -f "${NAME}_android".z*
        zip -q -s "$SPLIT" -j "${NAME}_android.zip" "$APK"
        verify_split "${NAME}_android.zip" 1
        # Runde 4 (B1): festhalten, WAS ausgeliefert wird - die Volumes dieses zip-Laufs mit ihrer sha256, genommen
        # VOR und NACH der Inhaltspruefung (gleich = geprueft wurde genau das). SHA256SUMS.txt bekommt fuer Android
        # genau diese Zeilen, git add genau diese Dateien; hat sich bis dahin etwas geaendert, Abbruch.
        android_vorher="$(sha256sum "${NAME}_android".z*)" || die "Android-Satz nicht lesbar"
        verify_apk_im_zip "${NAME}_android.zip" "$(basename "$APK")" "$APK_KENNUNG" \
            || die "Android-Satz ${NAME}_android.z* enthaelt NICHT die gepruefte APK (Meldung oben) - nicht ausgeliefert"
        ANDROID_SUMS="$(sha256sum "${NAME}_android".z*)" || die "Android-Satz nicht lesbar"
        [[ "$ANDROID_SUMS" == "$android_vorher" ]] || die "Android-Satz hat sich WAEHREND der Inhaltspruefung veraendert:
        vorher: $(echo $android_vorher)
        nachher: $(echo $ANDROID_SUMS)"
        for f in "${NAME}_android".z*; do
            if [[ -f "$f" ]]; then ANDROID_VOLUMES+=("$f"); fi
        done
        ANDROID_GEZIPPT=1
    elif [[ -n "$APK_KENNUNG" ]]; then
        die "Android-APK verschwand zwischen Pruefung und Zippen: $APK"
    else
        echo "   (kein Android-Paket: $(basename "$APK") fehlt — release/build_android.sh laeuft getrennt)"
    fi
    # Ausgeliefert wird nur, was in DIESEM Lauf entstand (Runde 4, B1): ein Android-Satz dieser Version ohne
    # ANDROID_GEZIPPT ist ein Rest (auch einer, der erst waehrend der Kopierminuten auftauchte) -> Abbruch,
    # statt ihn in SHA256SUMS.txt und git aufzunehmen. Die Liste unten laesst ihn zusaetzlich aus.
    if [[ -z "${ANDROID_GEZIPPT:-}" && -n "$(android_satz_da)" ]]; then
        die "Android-Satz ohne gepruefte APK aus diesem Lauf: $(echo $(android_satz_da)) - nicht ausgeliefert
        (release/build_android.sh --version $VERSION, oder --ohne-android)"
    fi
    if [[ -n "${ANDROID_GEZIPPT:-}" ]]; then
        android_jetzt="$(sha256sum "${NAME}_android".z*)" || die "Android-Satz nicht lesbar"
        [[ "$android_jetzt" == "$ANDROID_SUMS" ]] || die "Android-Satz wurde nach der Pruefung veraendert oder ergaenzt - nicht ausgeliefert:
        geprueft: $(echo $ANDROID_SUMS)
        jetzt:    $(echo $android_jetzt)"
    fi
    # Nachbesserung R4-1 (Gegenpruefung H3): SHA256SUMS.txt und git add aus einer POSITIVLISTE statt aus dem Glob
    # "${NAME}"_*.z*: die in DIESEM Lauf gezippten PC-Saetze, bewusst der kanonische Satz der anderen PC-Plattform
    # derselben Version (frueherer Lauf; verify_split: vollstaendig, nichts Fremdes) und der Android-Satz nur mit
    # ANDROID_GEZIPPT (die festgehaltenen Volumes). Jede andere <NAME>_*.z* bricht ab (fremde_abbruch).
    fremde_abbruch "vor dem Schreiben von SHA256SUMS.txt"
    for p in linux_steamdeck_x64 win64; do
        satz=()
        for f in "${NAME}_${p}".z*; do
            if [[ -f "$f" ]]; then satz+=("$f"); fi
        done
        (( ${#satz[@]} )) || continue
        if [[ "$ONLY" == "both" || ( "$ONLY" == "linux" && "$p" == linux_steamdeck_x64 ) || ( "$ONLY" == "win" && "$p" == win64 ) ]]; then
            herkunft="aus diesem Lauf"                # oben gezippt und mit verify_split geprueft
        else
            [[ -f "${NAME}_${p}.zip" ]] || die "Satz ${NAME}_${p}.z* ohne letztes Volume ${NAME}_${p}.zip - unvollstaendig"
            verify_split "${NAME}_${p}.zip" 1 || die "Satz der anderen Plattform ${NAME}_${p}.z* ist unvollstaendig oder hat Fremdes"
            herkunft="andere Plattform aus einem frueheren Lauf - in diesem Lauf nicht neu geprueft, unveraendert mitgefuehrt"
        fi
        AUSLIEFERN+=("${satz[@]}")
        echo "   Satz ${NAME}_${p}: ${#satz[@]} Volume(s), $herkunft"
    done
    (( ${#AUSLIEFERN[@]} )) || [[ -n "${ANDROID_GEZIPPT:-}" ]] || die "keine Split-Volumes fuer SHA256SUMS.txt"
    pc_sums=""
    if (( ${#AUSLIEFERN[@]} )); then pc_sums="$(sha256sum "${AUSLIEFERN[@]}")" || die "sha256sum der PC-Volumes fehlgeschlagen"; fi
    # nur Bash selbst schreibt die Datei (sha256sum aus /c/msys64/usr/bin laeuft unter einer anderen MSYS-Laufzeit)
    { if [[ -n "${ANDROID_GEZIPPT:-}" ]]; then printf '%s\n' "$ANDROID_SUMS"; fi
      if [[ -n "$pc_sums" ]]; then printf '%s\n' "$pc_sums"; fi; } > SHA256SUMS.txt
    echo
    zeigen=("${AUSLIEFERN[@]}" "${ANDROID_VOLUMES[@]}")
    ls -la "${zeigen[@]}"
    echo "== SHA256SUMS.txt geschrieben ($(grep -c '' SHA256SUMS.txt) Volumes${ANDROID_GEZIPPT:+, Android-Satz aus diesem Lauf}, Positivliste) =="
fi

# --- Git: NUR die aktuelle Version im Repo halten ----------------------------
# Nutzer-Vorgabe 2026-08-22: "zukuenftig bitte immer das neuste Package mit
# hochladen, und alte Packages vom Repo loeschen."
# Hintergrund: bis v0.3.8 wurde JEDE Version eingecheckt und keine je entfernt —
# 86 Paketdateien mit 5,9 GB steckten in der Historie und blaehten das Repo auf
# 8,6 GB auf. Die Historie wurde einmalig bereinigt (git filter-repo, 2,4 GB);
# damit das nicht zurueckkehrt, macht dieses Skript den Austausch selbst.
if command -v git >/dev/null 2>&1 && git -C "$HERE/.." rev-parse --git-dir >/dev/null 2>&1; then
    echo "== Git: alte Pakete austauschen =="
    # ⛔ NUR DIE EIGENE PLATTFORM AUFRAEUMEN.
    # Die Schleife loeschte bis v0.3.81 JEDE getrackte Datei release/re15_port_v0*, die
    # nicht mit dem aktuellen NAME beginnt - und NAME traegt die Version, nicht die
    # Plattform. Ein Lauf mit `--only win` hat damit jedes Mal auch das LINUX-/DECK-Paket
    # aus dem Repo geworfen, obwohl es gar nicht neu gebaut wurde. Genau so verschwand
    # re15_port_v0.3.69_linux_steamdeck_x64.* am 2026-08-31 in 47391207 (v0.3.70), und
    # seitdem hat jede reine Windows-Auslieferung den Zustand fortgeschrieben.
    # Jetzt raeumt ein Lauf nur die Pakete SEINER Plattformen weg; die andere bleibt
    # unangetastet stehen, bis sie selbst neu gebaut wird.
    plattformen=()
    [[ "$ONLY" == "both" || "$ONLY" == "linux" ]] && plattformen+=("linux_steamdeck_x64")
    [[ "$ONLY" == "both" || "$ONLY" == "win"   ]] && plattformen+=("win64")
    # Android zaehlt nur als eigene "Plattform", wenn in DIESEM Lauf auch ein
    # Android-Satz entstanden ist — sonst wuerde ein Lauf ohne APK den vorhandenen
    # Android-Satz aus dem Repo werfen (dieselbe Falle wie frueher bei --only win).
    [[ -n "${ANDROID_GEZIPPT:-}" ]] && plattformen+=("android")
    behalten() {                       # 0 = darf bleiben
        local b="$1" p
        case "$b" in
            "${NAME}_android".z*)                     # Android-Satz DIESER Version (Runde 4, B1):
                [[ $OHNE_ANDROID -eq 1 && -z "${ANDROID_GEZIPPT:-}" ]] && return 1   # --ohne-android: weg
                return 0 ;;
            "${NAME}"_*) return 0 ;;                  # die AKTUELLE Version bleibt
        esac
        for p in "${plattformen[@]}"; do
            case "$b" in *_"$p".z*) return 1 ;; esac  # eigene Plattform, alte Version
        done
        return 0                                      # fremde Plattform: NICHT anfassen
    }
    alt=0
    while IFS= read -r f; do
        [[ -n "$f" ]] || continue
        behalten "$(basename "$f")" && continue
        git -C "$HERE/.." rm --cached --quiet -- "$f" 2>/dev/null && alt=$((alt+1))
        rm -f "$HERE/../$f"                   # auch lokal weg, sonst waechst release/ endlos
    done < <(git -C "$HERE/.." ls-files -- 'release/re15_port_v0*')
    # Auch UNGETRACKTE Altpakete entfernen — sonst waechst release/ lokal endlos
    # weiter (die Schleife oben sieht nur, was Git kennt).
    for f in "$HERE"/re15_port_v0*.z*; do
        [[ -f "$f" ]] || continue
        behalten "$(basename "$f")" && continue
        rm -f "$f" && alt=$((alt+1))
    done
    neu=0
    # Nachbesserung R4-1 (Gegenpruefung H3): vorgemerkt wird GENAU die Positivliste aus SHA256SUMS.txt (Zippen oben):
    # PC-Saetze dieses Laufs + bewusst der Satz der anderen PC-Plattform, Android nur aus diesem Lauf (Runde 4, B1:
    # nur die festgehaltenen Volumes). Bis dahin nahm der Glob "${NAME}"_*.z* jede Datei mit dem Versions-Praefix mit.
    # Mit --no-zip entsteht keine Liste - dann wird nichts vorgemerkt (vorhandene Saetze bleiben, wie sie sind).
    for f in "${AUSLIEFERN[@]}" "${ANDROID_VOLUMES[@]}"; do
        b="$(basename "$f")"
        case "$b" in
            "${NAME}_android".z*)
                [[ -n "${ANDROID_GEZIPPT:-}" ]] || continue
                [[ $'\n'"${ANDROID_SUMS:-}"$'\n' == *"$b"$'\n'* ]] || die "Android-Volume $b steht nicht in den festgehaltenen Summen" ;;
        esac
        [[ -f "$HERE/$b" ]] || die "Volume der Positivliste verschwand vor git add: release/$b"
        git -C "$HERE/.." add -- "release/$b" || die "git add fehlgeschlagen: release/$b"
        neu=$((neu+1))
    done
    echo "   $alt alte Paketdatei(en) aus dem Repo entfernt, $neu neue vorgemerkt"
    echo "   (noch nicht committet — das macht der Release-Commit)"
fi

echo "== Fertig =="
