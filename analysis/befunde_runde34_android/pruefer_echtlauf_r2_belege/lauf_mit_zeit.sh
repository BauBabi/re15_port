#!/usr/bin/env bash
# Pruefer echtlauf r2 (nach dem Vorbild von r1): fuehrt "$@" aus, stempelt jede Ausgabezeile mit
# $EPOCHREALTIME (Bash-Bordmittel), schreibt nach $LOG und haengt EXIT=<rc> an. Rueckgabe = rc des
# Kommandos. KEINE Pipe auf das Kommando selbst: die Ausgabe geht per Prozess-Substitution in den
# Stempler, $? ist der Rueckgabewert des Kommandos (nicht eines tail/grep).
: "${LOG:?LOG setzen}"
stempel() { local l; while IFS= read -r l || [[ -n "$l" ]]; do printf '%s %s\n' "$EPOCHREALTIME" "$l"; done; }
echo "$EPOCHREALTIME START $(date '+%F %T') :: $*" > "$LOG"
rc=0
"$@" > >(stempel >> "$LOG") 2>&1 || rc=$?
sleep 1   # Stempler leerlaufen lassen
echo "$EPOCHREALTIME ENDE $(date '+%F %T')" >> "$LOG"
echo "EXIT=$rc" >> "$LOG"
exit $rc
