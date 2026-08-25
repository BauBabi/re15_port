#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# local_build.sh — reproduzierbarer LOKALER Build des RE1.5-C-Ports (PC-Target)
#
# WARUM ES DIESES SKRIPT GIBT
# ---------------------------------------------------------------------------
# Am 2026-08-23 starb der lokale Build mit "cc1.exe endet 0xC0000139
# STATUS_ENTRYPOINT_NOT_FOUND" und die (falsche) Diagnose lautete
# "Host-mingw defekt -> nur noch Docker".
#
# Die Toolchain war INTAKT. Die echte Ursache (gemessen 2026-08-24):
#
#   cc1.exe liegt NICHT in mingw64/bin, sondern in
#     C:/msys64/mingw64/lib/gcc/x86_64-w64-mingw32/<ver>/cc1.exe
#   und findet seine DLLs daher AUSSCHLIESSLICH ueber den PATH — anders als
#   ld/as/gdb, die in mingw64/bin liegen und dadurch vom Verzeichnis-Vorrang
#   der Windows-DLL-Suche geschuetzt sind (deshalb "liefen die weiter").
#
#   Git-Bash injiziert /mingw64/bin  =  C:/Program Files/Git/mingw64/bin
#   an PATH-Position 2 — SELBST, nicht aus dem Windows-PATH. C:/msys64/mingw64/bin
#   steht erst an Position ~40. Der erste Treffer gewinnt, also laedt cc1
#   Gits DLLs.
#
#   Gits libwinpthread-1.dll (60798 B, 2024-07-29, Git for Windows 2.46.0)
#   stammt von VOR der mingw-w64 64-bit-time_t-Umstellung und exportiert
#   clock_gettime64 NICHT. GCC 15.2 cc1 importiert genau diesen Namen.
#   -> STATUS_ENTRYPOINT_NOT_FOUND.
#
#   Ein-Variablen-Kausalexperiment (cc1 + alle 8 DLLs in ein Testverzeichnis,
#   nur EINE Datei je Lauf getauscht):
#     alles aus Git .......................... 0xC0000139
#     nur libwinpthread-1.dll aus msys64 ..... 0x00000000  <- geheilt
#     nur libgcc_s_seh-1.dll  aus msys64 ..... 0xC0000139  (nicht schuld)
#     nur libzstd.dll         aus msys64 ..... 0xC0000139  (nicht schuld)
#
# Weil Git-Bash /mingw64/bin selbst injiziert, heilt eine Aenderung der
# Windows-Umgebungsvariable Git-Bash NICHT. Der PATH muss IN der Shell bzw.
# im Build-Wrapper korrigiert werden — genau das tut dieses Skript.
#
# PowerShell ist NICHT betroffen (gemessen): dessen PATH enthaelt
# "C:\Program Files\Git\cmd" (enthaelt keine DLLs), aber NICHT
# "C:\Program Files\Git\mingw64\bin". Dort laeuft `cmake --build` direkt.
#
# BENUTZUNG
# ---------------------------------------------------------------------------
#   bash re15_port/tools/local_build.sh            # = all
#   bash re15_port/tools/local_build.sh configure
#   bash re15_port/tools/local_build.sh build
#   bash re15_port/tools/local_build.sh test
#   bash re15_port/tools/local_build.sh clean      # Build-Verzeichnis loeschen
#   bash re15_port/tools/local_build.sh env        # nur PATH/Toolchain zeigen
#
# Umgebungs-Overrides (alle optional):
#   RE15_MSYS_BIN   Standard: C:/msys64/mingw64/bin
#   RE15_CMAKE      Standard: aus dem geerbten PATH ermittelt
#   RE15_NINJA      Standard: aus dem geerbten PATH ermittelt
#   RE15_BUILD_DIR  Standard: <repo>/re15_port/build
#   RE15_JOBS       Standard: ninja-Automatik
#   RE15_CTEST_TIMEOUT  Standard: 30
#   RE15_FRESH=1    Configure erzwingt frischen Cache
#   RE15_TESTS      Standard: ON  (-DRE15_BUILD_TESTS)
#   RE15_TOOLS      Standard: OFF (-DRE15_BUILD_TOOLS, alte API)
#   RE15_MIN_TESTS  Standard: 235 (untere Schranke gegen eine KOLLABIERTE Suite,
#                   nicht nur gegen 0 Tests. Stand 2026-08-27 = 235 Tests (+5:
#                   integration_item_name_census, integration_fx_region_cull,
#                   unit_re2z_bandlock_pin, unit_re2z_rise_hittable, unit_writher_kill_flag); wird
#                   die Suite absichtlich kleiner, hier BEWUSST senken. WER TESTS
#                   HINZUFUEGT, HEBT DIESE ZAHL MIT — sonst waechst die Suite und die
#                   Wache bleibt zurueck.)
#
# FALLE, die dieses Skript bewusst schliesst
# ---------------------------------------------------------------------------
#   re15_port/CMakeLists.txt:100 -> option(RE15_BUILD_TESTS ... OFF)
#   Ohne -DRE15_BUILD_TESTS=ON wird tests/ gar nicht erst konfiguriert; ctest
#   meldet dann "No tests were found!!!" und beendet mit EXIT 0. Das ist ein
#   falsches Gruen. do_test() verlangt deshalb eine ctest-Summenzeile mit
#   mindestens RE15_MIN_TESTS Tests, sonst bricht es ab.
# ---------------------------------------------------------------------------

set -euo pipefail

# --- Ausgabe-Helfer --------------------------------------------------------
step() { printf '\n=== [local_build] %s\n' "$*"; }
info() { printf '    %s\n' "$*"; }
die()  { printf '\n!!! [local_build] FEHLER: %s\n' "$*" >&2; exit 1; }

# --- Repo-Wurzel aus dem Skript-Ort ableiten (kein cwd-Rateversuch) --------
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "$SCRIPT_DIR/../.." && pwd)"
[ -f "$REPO_ROOT/re15_port/CMakeLists.txt" ] \
  || die "re15_port/CMakeLists.txt nicht gefunden unter $REPO_ROOT"

# --- Werkzeuge NOCH aus dem geerbten PATH ermitteln, BEVOR er ersetzt wird -
# (cmake und ninja liegen auf dieser Maschine ausserhalb von msys64:
#  cmake = C:/Program Files/CMake/bin, ninja = C:/Python310/Scripts (pip).
#  msys64 hat weder das eine noch das andere.)
CMAKE_BIN="${RE15_CMAKE:-$(command -v cmake || true)}"
NINJA_BIN="${RE15_NINJA:-$(command -v ninja || true)}"
CTEST_BIN="${RE15_CTEST:-$(command -v ctest || true)}"
[ -n "$CMAKE_BIN" ] || die "cmake nicht im PATH gefunden (RE15_CMAKE setzen)"
[ -n "$NINJA_BIN" ] || die "ninja nicht im PATH gefunden (RE15_NINJA setzen)"
[ -n "$CTEST_BIN" ] || CTEST_BIN="$(dirname -- "$CMAKE_BIN")/ctest"

CMAKE_DIR="$(cd -- "$(dirname -- "$CMAKE_BIN")" && pwd)"
NINJA_DIR="$(cd -- "$(dirname -- "$NINJA_BIN")" && pwd)"

# --- Sauberer, minimaler PATH ---------------------------------------------
# Reihenfolge ist die eigentliche Reparatur — msys64 MUSS vorn stehen.
# Gits /mingw64/bin taucht bewusst gar nicht mehr auf.
# /usr/bin (Git-Bash-Coreutils: rm, mkdir, sed, sh) haengt HINTEN, aber noch
# VOR System32 — sonst loest `bash`/`sh`/`find`/`sort` auf die Windows- bzw.
# WSL-Gleichnamigen auf (gemessen: `bash` wurde zu WSL-bash, execvpe schlug fehl).
# /usr/bin enthaelt keine der von cc1 importierten DLLs (geprueft), kann den
# Compiler also von Position 4 aus nicht mehr schatten.
#
# ⚠️ Dieser PATH gilt NUR fuer den Build-Prozess — NICHT interaktiv uebernehmen:
# er ist ein ERSATZ, kein Prepend. Darin fehlen git, docker, node und npm (je
# exit 127), und `python` ist msys64s 3.12 statt C:/Python310s 3.10 — also OHNE
# numpy/torch/zstandard/PIL/vgamepad/capstone, an denen die RE-Werkbank haengt
# (parity_net, Savestate-Leser, room-capture, psx-disasm). Fuer den Build selbst
# folgenlos: der CMake-Graph ruft kein Python (geprueft), und pip-ninja traegt
# seinen Interpreter-Pfad fest im Exe-Header. Wer interaktiv nur den Compiler
# braucht, nimmt das Prepend aus scripts/green.sh:15 — das genuegt ebenfalls.
MSYS_BIN_WIN="${RE15_MSYS_BIN:-C:/msys64/mingw64/bin}"
MSYS_BIN_POSIX="/$(printf '%s' "$MSYS_BIN_WIN" | sed -e 's#^\([A-Za-z]\):#\L\1#' -e 's#\\#/#g')"
[ -x "$MSYS_BIN_POSIX/gcc.exe" ] \
  || die "gcc.exe nicht gefunden unter $MSYS_BIN_POSIX (RE15_MSYS_BIN setzen)"

CLEAN_PATH="$MSYS_BIN_POSIX:$CMAKE_DIR:$NINJA_DIR:/usr/bin:/c/Windows/System32:/c/Windows:/c/Windows/System32/Wbem"
export PATH="$CLEAN_PATH"

# --- Compiler explizit festnageln (nicht der CMake-Suche ueberlassen) ------
export CC="$MSYS_BIN_WIN/gcc.exe"
export CXX="$MSYS_BIN_WIN/g++.exe"

# --- Preflight: cc1 MESSEN, nicht annehmen --------------------------------
# Das ist die Versicherung gegen "morgen legt wieder jemand DLLs in den PATH".
preflight() {
    step "Preflight — Toolchain messen"
    info "PATH   = $PATH"
    info "CC     = $CC"
    info "cmake  = $CMAKE_BIN"
    info "ninja  = $NINJA_BIN"
    info "ctest  = $CTEST_BIN"

    local cc1 rc
    cc1="$("$CC" -print-prog-name=cc1 2>/dev/null || true)"
    [ -n "$cc1" ] || die "gcc konnte cc1 nicht benennen — Toolchain unbrauchbar"
    info "cc1    = $cc1"

    # Ohne Pipe messen: ein Exit-Code nach einer Pipe waere der von head/tee.
    set +e
    "$cc1" --version >/dev/null 2>&1
    rc=$?
    set -e
    if [ "$rc" -ne 0 ]; then
        printf '\n!!! cc1 startet nicht (exit=%s).\n' "$rc" >&2
        printf '!!! Typisch: eine fremde DLL wird vor %s gefunden.\n' "$MSYS_BIN_POSIX" >&2
        printf '!!! Erst-Treffer der von cc1 importierten DLLs im aktuellen PATH:\n' >&2
        local d p e
        local -a entries=()
        IFS=':' read -r -a entries <<< "$PATH"
        for d in libgcc_s_seh-1.dll libgmp-10.dll libisl-23.dll libmpc-3.dll \
                 libmpfr-6.dll libwinpthread-1.dll zlib1.dll libzstd.dll; do
            p=""
            for e in "${entries[@]}"; do
                if [ -f "$e/$d" ]; then p="$e/$d"; break; fi
            done
            printf '!!!   %-22s -> %s\n' "$d" "${p:-NICHT GEFUNDEN}" >&2
        done
        die "cc1-Preflight fehlgeschlagen — PATH-Shadowing pruefen"
    fi
    info "cc1 --version EXIT=0  -> Toolchain OK"

    # Echter Mini-Compile: beweist, dass nicht nur cc1 startet, sondern der
    # ganze Treiber (cc1 + as + collect2 + ld) durchlaeuft.
    # Im temporaeren Verzeichnis mit RELATIVEN Namen arbeiten: sonst wandelt
    # Git-Bash die POSIX-Pfade in den Argumenten des nativen gcc.exe um.
    local tmpdir back
    tmpdir="$(mktemp -d)" || die "mktemp fehlgeschlagen"
    back="$PWD"
    cd -- "$tmpdir"
    printf 'int main(void){return 0;}\n' > probe.c
    set +e
    "$CC" probe.c -o probe.exe 2>err.txt
    rc=$?
    set -e
    if [ "$rc" -ne 0 ] || [ ! -f probe.exe ]; then
        cat err.txt >&2 || true
        cd -- "$back"
        rm -rf -- "$tmpdir"
        die "Mini-Compile fehlgeschlagen (exit=$rc)"
    fi
    info "Mini-Compile OK ($(stat -c %s probe.exe) Bytes exe)"
    cd -- "$back"
    rm -rf -- "$tmpdir"
}

# --- Verzeichnisse ---------------------------------------------------------
BUILD_DIR_ABS="${RE15_BUILD_DIR:-$REPO_ROOT/re15_port/build}"
# Relativ zur Repo-Wurzel arbeiten: Git-Bash mangelt sonst POSIX-Pfade in
# den Argumenten des nativen cmake.exe.
cd -- "$REPO_ROOT"
SRC_REL="re15_port"
case "$BUILD_DIR_ABS" in
  "$REPO_ROOT"/*) BUILD_REL="${BUILD_DIR_ABS#"$REPO_ROOT"/}" ;;
  *)              BUILD_REL="$BUILD_DIR_ABS" ;;
esac

# --- Schritte --------------------------------------------------------------
do_clean() {
    step "clean — $BUILD_REL loeschen"
    rm -rf -- "$BUILD_REL"
    info "geloescht"
}

TESTS_OPT="${RE15_TESTS:-ON}"
TOOLS_OPT="${RE15_TOOLS:-OFF}"

do_configure() {
    step "configure — $SRC_REL -> $BUILD_REL (Ninja, PC=ON TESTS=$TESTS_OPT TOOLS=$TOOLS_OPT)"
    local cache="$BUILD_REL/CMakeCache.txt"
    local reset=0
    if [ "${RE15_FRESH:-0}" = "1" ]; then
        reset=1
        info "RE15_FRESH=1 -> Cache wird zurueckgesetzt"
    elif [ -f "$cache" ]; then
        # Ein Cache aus einem FEHLGESCHLAGENEN Configure ist unbrauchbar und
        # haelt CMake beim naechsten Lauf bei genau demselben Fehler fest.
        # ACHTUNG (gemessen 2026-08-24): ein per -D OHNE Typ-Suffix uebergebener
        # Eintrag landet als ":STRING=" im Cache, NICHT als ":FILEPATH=" —
        #   CMakeCache.txt:86  CMAKE_C_COMPILER:STRING=C:/msys64/mingw64/bin/gcc.exe
        # Die fruehere Abfrage auf ":FILEPATH=" traf deshalb NIE: jeder configure
        # warf Cache + CMakeFiles weg (~74 s statt Sekunden) und setzte dabei
        # still auch ein zuvor gesetztes CMAKE_BUILD_TYPE zurueck. Also: Typ offen
        # lassen, nur den WERT vergleichen (Backslashes normalisiert).
        local cached
        cached="$(sed -n 's#^CMAKE_C_COMPILER:[A-Za-z]*=##p' -- "$cache" | tail -1 | tr '\\' '/')"
        if [ "$cached" != "$(printf '%s' "$CC" | tr '\\' '/')" ]; then
            reset=1
            info "Cache traegt Compiler '${cached:-<keiner>}' statt '$CC' -> Reset"
        fi
    fi
    if [ "$reset" = "1" ] && [ -d "$BUILD_REL" ]; then
        rm -f  -- "$BUILD_REL/CMakeCache.txt"
        rm -rf -- "$BUILD_REL/CMakeFiles"
    fi

    "$CMAKE_BIN" -S "$SRC_REL" -B "$BUILD_REL" -G Ninja \
        -DRE15_BUILD_PC=ON \
        -DRE15_BUILD_TESTS="$TESTS_OPT" \
        -DRE15_BUILD_TOOLS="$TOOLS_OPT" \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_CXX_COMPILER="$CXX" \
        -DCMAKE_MAKE_PROGRAM="$NINJA_BIN" \
        || die "cmake configure fehlgeschlagen"
    info "configure OK"
}

do_build() {
    step "build — $BUILD_REL"
    [ -f "$BUILD_REL/CMakeCache.txt" ] \
      || die "kein CMakeCache in $BUILD_REL — erst 'configure' laufen lassen"
    # Bekannte Falle: eine noch laufende re15_pc.exe haelt das Link-Ziel
    # gesperrt, der Link scheitert mit "Permission denied".
    if command -v taskkill >/dev/null 2>&1; then
        taskkill //F //IM re15_pc.exe >/dev/null 2>&1 || true
    fi
    if [ -n "${RE15_JOBS:-}" ]; then
        "$CMAKE_BIN" --build "$BUILD_REL" -j "$RE15_JOBS" \
            || die "cmake --build fehlgeschlagen"
    else
        "$CMAKE_BIN" --build "$BUILD_REL" \
            || die "cmake --build fehlgeschlagen"
    fi
    info "build OK"
}

do_test() {
    step "test — ctest in $BUILD_REL"
    # Ein Testlauf zaehlt nur, wenn der Build davor nachweislich lief.
    [ -f "$BUILD_REL/build.ninja" ] \
      || die "kein build.ninja in $BUILD_REL — erst 'configure'/'build' laufen lassen"

    local log="$BUILD_REL/local_build_ctest.log"
    local rc
    set +e
    "$CTEST_BIN" --test-dir "$BUILD_REL" --timeout "${RE15_CTEST_TIMEOUT:-30}" \
        --output-on-failure > "$log" 2>&1
    rc=$?
    set -e
    cat -- "$log"
    [ "$rc" -eq 0 ] || die "ctest fehlgeschlagen (exit=$rc), Log: $log"

    # ctest beendet mit 0, wenn es GAR KEINE Tests gibt ("No tests were
    # found!!!"). Das waere ein falsches Gruen -> Summenzeile erzwingen.
    # Summenzeile hat die Form: "100% tests passed, 0 tests failed out of 224"
    local summary failed total passed
    summary="$(grep -oE '[0-9]+ tests failed out of [0-9]+' -- "$log" | tail -1 || true)"
    [ -n "$summary" ] \
      || die "ctest lieferte KEINE Summenzeile — vermutlich 0 Tests konfiguriert (RE15_BUILD_TESTS=OFF?). Log: $log"
    failed="${summary%% *}"
    total="${summary##* }"
    passed=$(( total - failed ))
    # Nicht nur "0 Tests" abfangen: auch eine auf wenige Tests KOLLABIERTE Suite
    # ist ein falsches Gruen (genau die Klasse, die hier schon einmal ein
    # erfundenes "224/224" erzeugt hat). Untergrenze deshalb = volle Suite.
    [ "$total" -ge "${RE15_MIN_TESTS:-235}" ] \
      || die "nur $total Tests gefunden, erwartet >= ${RE15_MIN_TESTS:-235} — Suite kollabiert? (RE15_MIN_TESTS setzen, wenn das ABSICHT ist)"
    [ "$failed" -eq 0 ] || die "$failed von $total Tests ROT. Log: $log"
    info "test OK — $passed/$total bestanden"
    TEST_SUMMARY="$passed/$total"
}

# --- Dispatch --------------------------------------------------------------
TEST_SUMMARY=""
CMD="${1:-all}"
preflight
case "$CMD" in
    env)        step "env — nur Diagnose, nichts gebaut" ;;
    clean)      do_clean ;;
    configure)  do_configure ;;
    build)      do_build ;;
    test)       do_test ;;
    all)        do_configure && do_build && do_test ;;
    *)          die "unbekannter Unterbefehl '$CMD' (configure|build|test|all|clean|env)" ;;
esac

if [ -n "$TEST_SUMMARY" ]; then
    printf '\n=== LOCAL-BUILD-OK (%s) — Tests %s\n' "$CMD" "$TEST_SUMMARY"
else
    printf '\n=== LOCAL-BUILD-OK (%s)\n' "$CMD"
fi
