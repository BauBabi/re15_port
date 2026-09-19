# Die acht ROOM1000- und die eine ROOM11F0-Freistellung auf den RICHTIGEN Cut und
# Massstab 1 umtragen (Messung: build/p3/mess_lage_null.py + mess_lage_suche.py).
import json, collections

P = "analysis/esp_masken_2026-09-03/auswahl.json"
d = json.load(open(P, encoding="utf-8"), object_pairs_hook=collections.OrderedDict)

BEFUND = (
    "⛔ PHASE 3 (2026-09-19): DIESE FREISTELLUNGEN LAGEN AM FALSCHEN CUT UND IM "
    "FALSCHEN MASSSTAB. Die alte Lagemessung nahm den BESTWERT der Farb-Uebereinstimmung "
    "ueber alle Verschiebungen — und der waechst zwangslaeufig, je kleiner das Objekt "
    "wird (gemessen build/p3/mess_lage_null.py: 03_01 bei s=1 54 %, s=2 80 %, s=3 87 %, "
    "s=4 89 %). Die Zahl war also eine selbstbestaetigende Metrik und hat 'Massstab 4' "
    "erzwungen. Gegen ein NULLMODELL gemessen (Verteilung der Uebereinstimmung ueber ALLE "
    "Lagen) sieht man es sofort: eine richtig sitzende 1x-Freistellung hat GENAU EINEN "
    "Gipfel (10E0/07_01: 1 Lage >= 95 % des Bestwerts von 10878, Gipfel 16,8 sigma; "
    "11F0/06_02: 1 von 10858, 14,7 sigma), waehrend die Massstab-4-Kandidaten 134 bis "
    "1709 gleich gute Lagen hatten und nur 1,9..2,9 sigma erreichten — ihre Lage war "
    "durch die Daten gar nicht bestimmt. Die Suche ueber ALLE Cuts des Raums bei "
    "Massstab 1 (build/p3/mess_lage_suche.py) findet sie eindeutig wieder, und die "
    "Sichtpruefung (build/p3/neulage_*.png) zeigt sie genau auf den Vordergrund-Moebeln."
)

# ---- ROOM1000 Cut 0: die vier 01_*-Freistellungen (Spindbloecke links/rechts + Bank)
LAGEN_C0 = [("01_01.png", 0, 47, 14061, "1 Gipfel-Umgebung mit 4 Lagen, 5,0 sigma, 100,0 %",
             "Spindblock links (Vordergrund)"),
            ("01_02.png", 99, 155, 2934, "EINE Lage >= 95 % des Bestwerts, 6,9 sigma, 100,0 %",
             "Bank vorne, linke Haelfte"),
            ("01_03.png", 152, 159, 2785, "EINE Lage >= 95 % des Bestwerts, 5,7 sigma, 100,0 %",
             "Bank vorne, rechte Haelfte"),
            ("01_04.png", 280, 112, 2409, "7 Lagen, 2,7 sigma, 100,0 %",
             "Spindblock rechts (Bildrand)")]
LAGEN_C2 = [("03_01.png", 0, 62, 10464, "2 Lagen, 5,3 sigma, 100,0 %", "Spindreihe links"),
            ("03_02.png", 147, 206, 1258, "4 Lagen, 9,0 sigma, 100,0 %", "Bank unten links"),
            ("03_03.png", 185, 204, 1386, "2 Lagen, 9,7 sigma, 100,0 %", "Bank unten rechts"),
            ("03_04.png", 259, 64, 5753, "2 Lagen, 3,5 sigma, 100,0 %", "Spindreihe rechts")]


def obj(datei, x, y, punkte, gipfel, was, ordner="1000"):
    return collections.OrderedDict([
        ("name", was),
        ("png", "pri/STAGE1/%s/%s" % (ordner, datei)),
        ("x", x), ("y", y), ("massstab", 1), ("oben", 0), ("grow", 0),
        ("grund", "Freistellung des Nutzers, Lage in Phase 3 neu gemessen: Massstab 1, "
                  "x=%d y=%d, %d Punkte, %s." % (x, y, punkte, gipfel)),
    ])


d["ROOM1000"]["0"] = collections.OrderedDict([
    ("_phase3", BEFUND + " Cut 0 trug bis hierher MEINE zwei Polygone (Spindblock vorne "
                "links/rechts, 14730 Punkte) — die vier Freistellungen des Nutzers "
                "standen faelschlich unter Cut 1 bei Massstab 4."),
    ("objekte", [obj(*a) for a in LAGEN_C0]),
])
d["ROOM1000"]["2"] = collections.OrderedDict([
    ("_phase3", BEFUND + " Cut 2 trug bis hierher MEINE drei Polygone (Spindreihe links/"
                "rechts, Bank unten, 29892 Punkte) — die vier Freistellungen des Nutzers "
                "standen faelschlich unter Cut 3 bei Massstab 4."),
    ("objekte", [obj(*a) for a in LAGEN_C2]),
])
d["ROOM11F0"]["0"] = collections.OrderedDict([
    ("_phase3", BEFUND + " Cut 0 trug bis hierher MEINE zwei Polygone (Tank/Sockel rechts "
                "unten, Rohr links unten) — die Freistellung des Nutzers stand "
                "faelschlich unter Cut 1 bei Massstab 4."),
    ("objekte", [obj("01.png", 191, 88, 12213, "3 Lagen, 4,1 sigma, 100,0 %",
                     "Schaltschraenke rechts (Vordergrund)", ordner="11F0")]),
])
for raum, cut in (("ROOM1000", "1"), ("ROOM1000", "3"), ("ROOM11F0", "1")):
    d[raum][cut] = collections.OrderedDict([
        ("_phase3", BEFUND + " Fuer diesen Cut gibt es damit KEINE Freistellung des Nutzers "
                    "mehr; die bisherige Sektion war aus den falsch verorteten PNGs gebaut "
                    "und wird ENTFERNT statt stehengelassen."),
        ("objekte", []),
    ])
json.dump(d, open(P, "w", encoding="utf-8"), indent=1, ensure_ascii=False)
print("umgetragen")
