"""Phase 2 (2026-09-19): auswahl.json fuer ROOM10D0 Cut 1 und Cut 6 auf die Nutzer-Originale."""
import json, os, sys
os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
p = "analysis/esp_masken_2026-09-03/auswahl.json"
a = json.load(open(p, encoding="utf-8"))
a["ROOM10D0"]["1"]["objekte"] = [{
    "name": "Klapptisch hinten (Nutzer-Original 01.png)", "png": "pri/STAGE1/10D0/01.png",
    "x": 124, "y": 87, "massstab": 1, "oben": 0, "grow": 0,
    "grund": "Phase 2 (2026-09-19): das Nutzer-Original 01.png (54x61, 1021 px, Blob 13f2c9fb aus dem Commit der 27 neuen Masken) ersetzt meine Zerteilung 01_01/01_02. Lage nachgemessen: x=124 y=87, 100,0 %. Tiefe aus der Geometrie (geometrie.py), keine Handschluessel."}]
a["ROOM10D0"]["1"]["_warum"] = (a["ROOM10D0"]["1"].get("_warum", "") +
    " | PHASE 2 2026-09-19: Zerteilung zurueckgenommen, Nutzer-Original 01.png als EIN Objekt (Gegenpruefung: Nutzer-PNG hat Vorrang).")
neu = []
for o in a["ROOM10D0"]["6"]["objekte"]:
    if o.get("png", "").endswith("06_01.png"):
        neu.append({"name": "Stuhlkante (Nutzer-Original 06.png)", "png": "pri/STAGE1/10D0/06.png",
                    "x": 34, "y": 172, "massstab": 1, "oben": 0, "grow": 0, "aufrecht": True,
                    "grund": "Phase 2 (2026-09-19): Nutzer-Original 06.png (30x46, 245 px, Blob 2204ab8c) statt der 65x120-Fassung 06_01.png; die eingetragene Lage x=34 y=172 passt zum Original zu 100,0 % (Sonde und Nachmessung), zur 65x120-Fassung nur zu 56 %. NUTZER-ENTSCHEIDUNG: der Urheber der 65x120-Fassung ist nicht belegt (055374ab schweigt dazu)."})
    else:
        neu.append(o)
a["ROOM10D0"]["6"]["objekte"] = neu
a["ROOM10D0"]["6"]["_warum"] = (a["ROOM10D0"]["6"].get("_warum", "") +
    " | PHASE 2 2026-09-19: Stuhlkante = Nutzer-Original 06.png (30x46) an x=34,y=172. Die Pflanze 06_02.png ist MEINE Huelle (kein Nutzer-PNG) und bleibt als Nachfrage.")
json.dump(a, open(p, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
for c in ("1", "6", "7"):
    for o in a["ROOM10D0"][c]["objekte"]:
        print(c, o.get("name"), o.get("png"), o.get("quader"), o.get("x"), o.get("y"))
