#!/usr/bin/env bash
# Pruefer echtlauf r3 (Vorbild: pruefer_echtlauf_r2_belege/lauf_mit_zeit.sh, gleiche Logik): fuehrt "$@" aus,
# stempelt jede Ausgabezeile mit $EPOCHREALTIME, schreibt nach $LOG und haengt EXIT=<rc> an. Rueckgabe = rc
# des Kommandos. KEINE Pipe auf das Kommando: Ausgabe per Prozess-Substitution in den Stempler, $? ist der
# Rueckgabewert des Kommandos selbst (nicht eines tail/grep).
: "${LOG:?LOG setzen}"
stempel() { local l; while IFS= read -r l || [[ -n "$l" ]]; do printf '%s %s\n' "$EPOCHREALTIME" "$l"; done; }
echo "$EPOCHREALTIME START $(date '+%F %T') :: $*" > "$LOG"
rc=0
"$@" > >(stempel >> "$LOG") 2>&1 || rc=$?
sleep 1   # Stempler leerlaufen lassen
echo "$EPOCHREALTIME ENDE $(date '+%F %T')" >> "$LOG"
echo "EXIT=$rc" >> "$LOG"
exit $rc
