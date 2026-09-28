# r30_hundtod_watch.gdb — WER schreibt das hp-Feld des Spielers?
# Aufruf (Git-Bash, PATH mit /c/msys64/mingw64/bin voran):
#   gdb -q -batch -x analysis/befunde_runde30/hund-tod_tools/r30_hundtod_watch.gdb \
#       --args re15_port/build_r30_hund-tod/tests/unit/probe_r30_hund_tod.exe lauf 1 20 400 -7878 -17384 0
# Haelt an jedem Schreibzugriff auf g_actors[0].hp und zeigt Alt/Neu + die Aufrufkette.
set pagination off
set confirm off
set print thread-events off
break lauf
run
# erst ab hier zaehlen: das Hochfahren (hp=100) liegt davor
watch -l g_actors[0].hp
commands
  silent
  printf "=== hp-SCHREIBER: neuer Wert %d\n", g_actors[0].hp
  bt 5
  continue
end
continue
