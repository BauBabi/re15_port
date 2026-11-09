#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_karten_html.py - Auswahlbogen fuer die RE2-Karten, Bauweise wie
`uebersicht.html` der Dokument-Extraktion (re15_port/tools/re2_dokumente_extrakt.py).

Erwartet die Ausgabe von re2_karten_extrakt.py in build/extracted/re2_karten/.
"""
import os, sys, json, html, glob

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "build", "extracted", "re2_karten")

BIT2SHEET = {   # Bank-33-Bit -> (PIX-Slot, Titel im Blatt)  [Kopftabelle 0x800AAA38 +5/+6]
    0: (0, "CITY AREA"), 1: (1, "CITY AREA"),
    2: (4, "POLICE STATION 1F"), 3: (2, "POLICE STATION 2F"),
    4: (3, "POLICE STATION 3F"), 5: (5, "POLICE STATION B1"),
    6: (6, "SEWAGE DISPOSAL"), 7: (8, "SEWER B2"), 8: (7, "SEWER B1"),
    9: (9, "VACANT FACTORY B1"), 10: (10, "VACANT FACTORY 1F"),
    11: (11, "VACANT FACTORY 1F"), 12: (12, "VACANT FACTORY"),
    13: (13, "LABORATORY B1"), 14: (14, "LABORATORY B2"),
    15: (15, "LABORATORY B3"), 16: (16, "LABORATORY B4"),
    17: (17, "LABORATORY B5"), 18: (18, "TRANSPORT"), 19: (19, "TRAIN"),
}

# Fundtext je Karte: die beiden Message_on-Indizes des Fundblocks, EN-Bank (msg/subNN.msg)
FRAGE_ANTWORT = {
    "room20B0": (0, 17), "room2130": (0, 12), "room3060": (1, 2),
    "room4040": (10, 11), "room5040": (3, 4), "room5060": (1, 2),
    "room6120": (0, 14),
}

rec = json.load(open(os.path.join(OUT, "karten.json")))
zen = json.load(open(os.path.join(OUT, "zensus.json")))

CSS = """
body{background:#101014;color:#e8e8ee;font:15px/1.5 system-ui,sans-serif;margin:0;padding:24px}
h1{font-size:26px;margin:0 0 4px} h2{font-size:19px;margin:34px 0 10px;color:#9fd0ff}
.k{border:1px solid #2a2a36;border-radius:10px;background:#16161d;margin:16px 0;padding:14px}
.kopf{display:flex;gap:14px;align-items:baseline;flex-wrap:wrap}
.name{font-size:20px;font-weight:600;color:#ffe08a}
.meta{color:#9a9aab;font-size:13px}
.reihe{display:flex;gap:14px;flex-wrap:wrap;margin-top:12px}
.zelle{background:#0c0c11;border:1px solid #23232e;border-radius:8px;padding:8px;text-align:center}
.zelle img{display:block;image-rendering:pixelated;background:#000;border-radius:4px}
.zelle .bu{font-size:12px;color:#9a9aab;margin-top:5px}
.txt{background:#0c0c11;border-left:3px solid #4a7fb5;padding:8px 12px;margin-top:10px;
     white-space:pre-wrap;font-family:ui-monospace,monospace;font-size:13px;color:#cfe4ff}
table{border-collapse:collapse;margin-top:10px;font-size:13px}
td,th{border:1px solid #2a2a36;padding:4px 9px;text-align:left}
th{background:#1c1c26;color:#9fd0ff}
.warn{border-left:3px solid #e0a040;background:#1d1a12;padding:8px 12px;margin:10px 0;
      font-size:13px;color:#f0d8a8}
code{color:#a8e0a8}
"""


def bild(pfad, breite, bu):
    if not os.path.exists(os.path.join(OUT, pfad)):
        return '<div class="zelle"><div class="bu">%s<br>— fehlt —</div></div>' % html.escape(bu)
    return ('<div class="zelle"><img src="%s" width="%d"><div class="bu">%s</div></div>'
            % (html.escape(pfad), breite, bu))


p = []
p.append("<!doctype html><meta charset='utf-8'><title>RE2 — Karten</title><style>%s</style>" % CSS)
p.append("<h1>Resident Evil 2 (Leon) — die Karten</h1>")
p.append("<div class='meta'>Weltmodell, Kartenblatt, Name, Fundort. "
         "Erzeugt von <code>tools/re2_karten/re2_karten_extrakt.py</code> aus "
         "<code>info/re2leon/</code>.</div>")

p.append("<div class='warn'><b>Der wichtigste Unterschied zu den Dokumenten:</b> "
         "eine Karte ist in RE2 <b>kein Gegenstand</b>. Es gibt keinen Item-Slot, keinen "
         "Inventar-Namen und damit <b>kein Inventar-Symbol</b> — die vollstaendige "
         "Namensbank (Offsettabelle <code>0x8009EBAC</code>, Basis <code>0x8009E550</code>, "
         "140 Eintraege) enthaelt kein einziges &bdquo;Map&ldquo;. Der Fund setzt nur Bits in "
         "<b>Flag-Bank 33</b> = <code>0x800D4924</code> (Bank-Zeigertabelle "
         "<code>0x800A78C8</code>, gelesen <code>lw v1,30920(at)</code> @0x800543E8). "
         "Sichtbar in der Welt ist trotzdem ein 3D-Objekt — dasselbe Pool-Prinzip wie bei "
         "den Dokumenten (<code>0x800D0324</code>, Schrittweite <code>0x1F8</code>).</div>")

p.append("<h2>Zensus</h2><table>")
p.append("<tr><th>Frage</th><th>Antwort</th><th>Beleg</th></tr>")
p.append("<tr><td>Kartenblaetter insgesamt</td><td>20</td>"
         "<td>Slot-Tabelle 0x800A9414, 20 x 0x8000 = 655360 B = Dateigroesse MAPS.PIX</td></tr>")
p.append("<tr><td>Karten-<i>Items</i></td><td>0</td><td>140 Namen dekodiert, kein &bdquo;Map&ldquo;</td></tr>")
p.append("<tr><td>Inventar-Symbole</td><td>0</td><td>kein Item-Slot -> kein ITEMALL-Eintrag</td></tr>")
p.append("<tr><td>Fundstellen (Set bank=33)</td><td>%d in %d Raeumen</td>"
         "<td>opcode-exakter Walk ueber 2553 SCD-Bloecke; Brute-Force-Gegenprobe: %d</td></tr>"
         % (len(zen["sets"]), len(set(s["room"] for s in zen["sets"])), zen["stat"]["brute"]))
p.append("<tr><td>Karten mit Weltmodell</td><td>6 Platzierungen, <b>3 verschiedene Meshes</b></td>"
         "<td>md5 der MD1-Bytes</td></tr>")
p.append("<tr><td>ohne Weltmodell</td><td>room6120 (Konsole: &bdquo;Will you <i>file</i> the "
         "laboratory map?&ldquo;) + das Kinoereignis room3040/sub24</td><td>kein Work_set 04</td></tr>")
p.append("<tr><td>Etagen je Fund</td><td>1 bis 5 — <b>nicht pro Etage</b></td>"
         "<td>Bits je Fundblock, s.u.</td></tr>")
p.append("</table>")

for r in rec:
    bits = r["bits"]
    p.append("<div class='k'>")
    p.append("<div class='kopf'><span class='name'>%s</span>"
             "<span class='meta'>%s / %s &nbsp;·&nbsp; Bank-33-Bits %s</span></div>"
             % (html.escape(r["name"]), r["room"], r["block"],
                ", ".join(map(str, bits))))
    # Fundtext
    fa = FRAGE_ANTWORT.get(r["room"])
    if fa:
        for k in fa:
            t = r["msgs"].get(str(k)) or r["msgs"].get(k)
            if t:
                p.append("<div class='txt'>msg/sub%02d.msg: %s</div>" % (k, html.escape(t)))
    # Fundort
    tr = [t for t in r["trigger"] if t["kind"] in ("Aot_set", "Aot_set_4p")]
    if tr:
        t = tr[0]
        p.append("<div class='meta'>Ausloeser: %s aot=%d, sce=5, Mitte (x=%d, z=%d)%s "
                 "&nbsp;·&nbsp; %s+0x%04X</div>"
                 % (t["kind"], t["aot"], t["x"], t["z"],
                    (", Box %dx%d" % (t["w"], t["d"])) if t.get("w") else "",
                    t["ent"], t["off"]))
    else:
        p.append("<div class='meta'>Ausloeser: nicht gefunden (siehe Dossier, Abschnitt &bdquo;offen&ldquo;)</div>")
    if r["obj_model_set"]:
        o = r["obj_model_set"][0]
        p.append("<div class='meta'>Weltmodell: Objekt-Slot %d, gesetzt in %s+0x%04X, "
                 "Position (%d, %d, %d)</div>" % (r["slot"], o["ent"], o["off"], o["x"], o["y"], o["z"]))
    elif r["slot"] is not None:
        p.append("<div class='meta'>Weltmodell: Objekt-Slot %d wird vom Skript verschoben, "
                 "aber in PL0 nie mit <code>Obj_model_set</code> aufgestellt</div>" % r["slot"])
    for h_ in r["hide"]:
        if h_["kind"] == "Pos_set":
            p.append("<div class='meta'>Nach dem Fund: Slot %d -> Pos_set(%d, %d, %d)</div>"
                     % (h_["slot"], h_["x"], h_["y"], h_["z"]))
        else:
            p.append("<div class='meta'>Nach dem Fund: Slot %d -> Member_set 0x%02X = %d "
                     "(Member 0x0C schreibt obj+0x3C = das Y von Pos_set)</div>"
                     % (h_["slot"], h_["member"], h_["value"]))
    # Bilder
    p.append("<div class='reihe'>")
    if r["slot"] is not None:
        pre = "weltmodelle/%s_slot%02d" % (r["room"], r["slot"])
        mo = r["model"] or {}
        p.append(bild(pre + "_a.png", 240, "Weltmodell (md5 %s, %s B)"
                      % (mo.get("md5", "?")[:8], mo.get("md1_size", "?"))))
        p.append(bild(pre + "_b.png", 240, "Rueckansicht"))
    for b in bits:
        slot, titel = BIT2SHEET[b]
        p.append(bild("blaetter/blatt%02d.png" % slot, 240,
                      "Bit %d -> PIX-Slot %d<br>%s" % (b, slot, titel)))
    p.append("</div></div>")

p.append("<h2>Alle 20 Kartenblaetter</h2>")
p.append("<div class='reihe'>")
for b in range(20):
    slot, titel = BIT2SHEET[b]
    p.append(bild("blaetter/blatt%02d.png" % slot, 200, "Slot %d — %s" % (slot, titel)))
p.append("</div>")

p.append("<h2>RE1.5-Gegenprobe</h2>")
p.append("<div class='warn'>RE1.5 hat <b>13 fertig gezeichnete Kartenblaetter</b> "
         "(<code>DATA/MAP01..MAP0D.PIX</code>, je 0x8000 B, 256x256 4bpp — byte-gleiches "
         "Format wie ein RE2-Slot), aber <b>keinen Fund-Mechanismus</b>: kein Karten-Item "
         "(101 Namen in DEBUG.BIN, keiner ist eine Karte) und keine Flag-Abfrage. Die "
         "angezeigte Seite folgt allein der Raumnummer: <code>lh v1,0x800b0fe2</code> "
         "@0x8004B56C, <code>sltiu v0,v1,0x26</code> @0x8004B574, Sprungtabelle "
         "<code>0x8001103C</code> (38 Eintraege).</div>")
p.append("<div class='reihe'>")
for f in sorted(glob.glob(os.path.join(OUT, "re15_gegenprobe", "MAP*.png"))):
    n = os.path.basename(f)[:-4]
    p.append(bild("re15_gegenprobe/" + os.path.basename(f), 200, n))
p.append("</div>")

path = os.path.join(OUT, "uebersicht_modelle.html")
open(path, "w", encoding="utf-8").write("\n".join(p))
print("geschrieben:", path)
