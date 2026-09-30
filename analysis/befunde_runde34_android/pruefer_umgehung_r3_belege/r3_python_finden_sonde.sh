#!/usr/bin/bash
# Gegenpruefung R3: python_finden.sh unter feindlichen PATH-Lagen, OHNE einen Kandidaten zu starten.
# Sonde = nachbesserung_r2_belege/python_finden_SONDE_neu.sh (identisch mit release/python_finden.sh bis auf die
# zwei Startzeilen 112/114 -> "SONDE: WUERDE STARTEN"; vorher per diff belegt). Jede Lage in eigenem
# /usr/bin/bash (absolut - ein blankes "bash" koennte ueber den neuen PATH WindowsApps\bash.exe = WSL treffen).
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
SONDE=analysis/befunde_runde34_android/nachbesserung_r2_belege/python_finden_SONDE_neu.sh
echo "diff release/python_finden.sh <-> Sonde (nur die zwei Startzeilen duerfen abweichen):"
diff release/python_finden.sh "$SONDE" | grep -E '^[<>]' | cut -c1-110
WA_LANG="$(cygpath -u "$LOCALAPPDATA")/Microsoft/WindowsApps"
WA_KURZ="$(cygpath -u "$(cygpath -d "$WA_LANG")")"
SONDE_ABS="$(pwd)/$SONDE"
LAUF_CWD="$(pwd)"
lage() {   # $1 = Etikett, Rest = env-Zuweisungen; Arbeitsverzeichnis = $LAUF_CWD
    local tag="$1"; shift
    local out rc=0
    out="$(cd "$LAUF_CWD" && env -i SYSTEMROOT="$SYSTEMROOT" OSTYPE=msys "$@" /usr/bin/bash "$SONDE_ABS" 2>&1)" || rc=$?
    echo "--- $tag (rc=$rc, cwd=$LAUF_CWD)"; echo "$out" | sed 's/^/   /' | cut -c1-170
}
lage "a PATH = nur WindowsApps (Langname), NUR_PATH"      PATH="$WA_LANG" RE15_PYTHON_NUR_PATH=1
lage "b PATH = nur WindowsApps (8.3), NUR_PATH"           PATH="$WA_KURZ" RE15_PYTHON_NUR_PATH=1
lage "c PATH = 8.3 + /usr/bin (readlink, cygpath da)"     PATH="$WA_KURZ:/usr/bin" RE15_PYTHON_NUR_PATH=1
lage "d RE15_PYTHON = Alias (Langname .exe), PATH leer"   PATH="" RE15_PYTHON="$WA_LANG/python3.exe"
lage "e RE15_PYTHON = Alias (8.3 ohne .exe), PATH leer"   PATH="" RE15_PYTHON="$WA_KURZ/python3"
lage "f RE15_PYTHON = Alias gross geschrieben"            PATH="/usr/bin" RE15_PYTHON="$(echo "$WA_LANG" | tr a-z A-Z)/PYTHON.EXE"
lage "g kein Python im PATH, NUR_PATH (fail closed)"      PATH="/usr/bin" RE15_PYTHON_NUR_PATH=1
lage "h RE15_PYTHON = fehlt"                               PATH="/usr/bin" RE15_PYTHON="/c/gibt/es/nicht/python.exe"
# relativer PATH-Eintrag '.', Arbeitsverzeichnis = WindowsApps: der Pfad-String enthaelt "windowsapps" NICHT
LAUF_CWD="$WA_LANG"
lage "i PATH = '.', cwd = WindowsApps, NUR_PATH"            PATH="." RE15_PYTHON_NUR_PATH=1
lage "j PATH = '.:/usr/bin', cwd = WindowsApps, NUR_PATH"   PATH=".:/usr/bin" RE15_PYTHON_NUR_PATH=1
lage "k RE15_PYTHON = python3 (relativ), cwd = WindowsApps" PATH="/usr/bin" RE15_PYTHON="python3"
echo FERTIG
