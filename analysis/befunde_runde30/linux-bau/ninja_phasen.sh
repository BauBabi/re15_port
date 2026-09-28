#!/usr/bin/env bash
# Teilt eine .ninja_log (v5: start_ms end_ms mtime ausgabe hash) in Uebersetzen (*.o)
# und Linken/Archivieren (alles andere): Kanten, Summe der Einzeldauern, Wanduhr-Spanne.
# Aufruf: ninja_phasen.sh <.ninja_log>
awk '
    /^#/ { next }
    { s = $1; e = $2; o = $4
      k = (o ~ /\.o$/) ? "uebersetzen" : "linken"
      n[k]++; sum[k] += (e - s)
      if (!(k in a) || s < a[k]) a[k] = s
      if (!(k in b) || e > b[k]) b[k] = e
      if (!("alle" in a) || s < a["alle"]) a["alle"] = s
      if (!("alle" in b) || e > b["alle"]) b["alle"] = e }
    END {
      for (k in n) printf "%-12s %5d Kanten  Summe %8.1f s  Spanne %7.1f s (%.1f..%.1f)\n", k, n[k], sum[k]/1000, (b[k]-a[k])/1000, a[k]/1000, b[k]/1000
      printf "%-12s              Spanne %7.1f s\n", "gesamt", (b["alle"]-a["alle"])/1000 }
' "$1"
