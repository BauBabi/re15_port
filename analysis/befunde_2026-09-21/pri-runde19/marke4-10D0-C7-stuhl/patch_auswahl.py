# -*- coding: utf-8 -*-
"""Traegt den blaugrauen Kasten am Tischende als EIGENES Tiefen-Objekt in
analysis/esp_masken_2026-09-03/auswahl.json (ROOM10D0 Cut 7) ein und zieht seine
18 Punkte von der Nutzer-Freistellung ab.

Aufruf: patch_auswahl.py [zelle|profil|zurueck]
"""
import json
import sys

P = "analysis/esp_masken_2026-09-03/auswahl.json"
VARIANTE = sys.argv[1] if len(sys.argv) > 1 else "profil"
d = json.load(open(P, encoding="utf-8"))
e = d["ROOM10D0"]["7"]
obj = e["objekte"]

# vorherigen Eintrag entfernen (idempotent)
obj[:] = [o for o in obj if o.get("png") != "pri/STAGE1/10D0/07_03.png"]
for o in obj:
    o.pop("minus_png", None)

if VARIANTE == "zurueck":
    json.dump(d, open(P, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
    print("zurueckgenommen")
    raise SystemExit(0)

stuhl = [o for o in obj if o.get("png") == "pri/STAGE1/10D0/07_01.png"][0]
stuhl["minus_png"] = [{"png": "pri/STAGE1/10D0/07_03.png", "x": 46, "y": 126}]

kasten = {
    "name": "Blaugrauer Kasten am Tischende",
    "png": "pri/STAGE1/10D0/07_03.png",
    "x": 46,
    "y": 126,
    "massstab": 1,
    "oben": 0,
    "grow": 0,
    "aufrecht": True,
    "grund": (
        "Silhouette aus dem HINTERGRUND (build/bg_ppm/ROOM10D07.ppm), Regel gemessen: "
        "Fenster x46..72/y124..159, b-r >= 0 UND r+g+b >= 120 -> Boden-Fehlalarm 0 von 609 "
        "Referenzpunkten (x72..100/y150..170), Treffer 69,1 %; groesste 8er-Komponente "
        "(262 von 270 Kernpunkten), 3x3 geschlossen und Loecher gefuellt (+11 Punkte = 4 %). "
        "273 Punkte, 0 % davon bodenartig (b-r < -4), Helligkeit Median 171, b-r Median +18. "
        "WARUM EIGENES OBJEKT: die Nutzer-Freistellung 07_01.png ist EINE "
        "Zusammenhangskomponente ueber beide Gegenstaende und bekommt mit 'aufrecht' EINEN "
        "Standpunkt am tiefsten Punkt (Bildzeile 238, Spalte 38,2) -> Welt(-1665,25993), "
        "Tiefe 53..62. Der Kasten steht aber GEMESSEN bei Welt z 27854..27981 (Sockel "
        "Bildzeile 154..158, Rueckprojektion hin und zurueck exakt), Kamera-z 7450..7787, "
        "Tiefe 116..121. Nullmodell: ein Gegenstand am Fuss des Spielers (vz 6073) haette "
        "seinen Sockel auf Bildzeile 179, der Kasten hat ihn 21..25 Zeilen hoeher. "
        "Ueber den gemeinsamen Standpunkt wurden 18 seiner Punkte (x58..62, y134..159, alle "
        "hell, Helligkeit Median 341) mit Tiefe 53..55 VOR den Spieler gezeichnet - der "
        "helle Splitter, den der Nutzer an Marke 4 (F3843) als 'fehlende Stuecke vom Stuhl' "
        "gemeldet hat. 'aufrecht': ein Kasten steht senkrecht, sein tiefster "
        "Silhouettenpunkt IST sein Bodenkontakt."
    ),
}
kasten["grund"] += (
    " Kein 'zelle'-Eingriff: gemessen (beide Varianten gebaut) liefert das Werkzeug hier "
    "von sich aus das Profil, Tiefe 109..114 - die Zellen-Zuordnung greift nicht, ein "
    "Override waere unbelegt."
)
obj.append(kasten)
json.dump(d, open(P, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
print("eingetragen, Variante %s; Objekte jetzt: %s"
      % (VARIANTE, [o.get("name") for o in obj]))
