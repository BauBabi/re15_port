# Runde 35 Spur N, Nachbesserung 1 (M1): simuliert "kuenftige Aenderungen" am Urteil release/gate_urteil.py.
# Je Eintrag genau EINE Textaenderung in einer Kopie, dann "<kopie> --selbsttest"; BEMERKT = Rueckgabe != 0.
# A* = woertlich die Aenderungen der Abnahme 0 (N_abnahme_0.md 3a), C* = eigene zusaetzliche Lockerungen.
# Aufruf: python urteil_aenderungen.py <gate_urteil.py> <arbeitsordner>
import os, subprocess, sys

AEND = [
    ("A2", "or 0 in baum", ""),
    ("A3", "or mb != b", ""),
    ("A4", " or f.group(6).strip()", ""),
    ("A5", r'r"(\d+)/(\d+) Faelle \(.*\)"', r'r"(\d+)/(\d+) Faelle.*"'),
    ("A6", r'r"   \S+\s+(\d+) Dateien"', r'r"   \S+\s+(\d+).*"'),
    ("A7", '"[FEHLER]" in z', '"[FEHLER!]" in z'),
    ("A8", ' or z.startswith("Traceback")', ""),
    ("A11", r'r"   TORSE\.VBS: Quelle (\d+) B, APK (\d+) B, sha256 gleich"', r'r"   TORSE\.VBS: Quelle (\d+) B, APK (\d+) B.*"'),
    ("C1", r'r"   Innere Proben: (\d+)/(\d+) \(.*\)"', r'r"   Innere Proben: (\d+)/(\d+).*"'),
    ("C2", r'r"   SUMME\s+(\d+)', r'r"   SUMME\s*(\d+)'),
    ("C3", r'(\d+) Zeilen / (\d+) Bytes', r'(\d+).* / (\d+) Bytes'),
    ("C4", r'APK (\d+), sha256 gleich (\d+)/(\d+)"', r'APK (\d+), .* (\d+)/(\d+)"'),
    ("C7", r'wie die Engine-Tabelle .*"', r'wie die .*"'),
    ("C9", r'r"(\d+) Dateien in (\d+) Baeumen, Tuer-Soll erfuellt"', r'r"(\d+) Dateien in (\d+) Baeumen.*"'),
    ("C10", r'r"   \S+\s+(\d+)\s+(\d+)"', r'r"   \S+\s+(\d+)\s+(\d+).*"'),
    ("C11", r'-(OK|FEHLER|ABWEICHUNG)\b"', r'-(OK|FEHLER|ABWEICHUNG)"'),
    ("C12", 'z.startswith("ABBRUCH")', 'z.startswith("ABBRUCH:")'),
    ("C14", r'r"(\d+) Dateien in (\d+) Baeumen bytegleich, Tuer', r'r"(\d+) Dateien in (\d+) Baeumen .*, Tuer'),
    ("C15", r'TORSE\.VBS: Quelle (\d+) B', r'TORSE\.VBS: Quelle .* B'),
    ("C16", r'r"(\d+) Dateien in (\d+) Baeumen bytegleich, nichts zusaetzlich"', r'r"(\d+) Dateien in (\d+) Baeumen bytegleich.*"'),
    ("C17", r'"== %s-OK: (.+) =="', r'"== %s-OK: (.+)"'),
    ("C18", r'r"   \[(ok|FEHLER)\] (\d+) (.*) rc=', r'r"   \[(ok|FEHLER)\] (\d+)(.*) rc='),
    ("C19", r'"   Tuer-Soll: .* (\d+)/(\d+) wie', r'"   Tuer-Soll: .*(\d+)/(\d+) wie'),
    ("C20", '"paket": "APK-ASSET-GATE-PAKET"', '"paket": "APK-ASSET-GATE-PAK"'),
    # D* = Aenderungen AUSSERHALB der Mutations-Operatoren (Gegenprobe: was faengt allein die Fallsammlung?)
    ("D1", 'or "[FEHLER]" in z:', 'or z.startswith("   [FEHLER]"):'),
    ("D2", "if not t or any(int(m.group(1))", "if not t or all(int(m.group(1))"),
    ("D5", r'-(OK|FEHLER|ABWEICHUNG)\b"', r'-(OK|FEHLER)\b"'),
    ("D6", "            t = [m for m in (re.fullmatch(muster, z) for z in zeilen) if m]",
           "            t = [m for m in (re.match(muster, z) for z in zeilen) if m]"),
    ("D7", r'm = re.fullmatch(r"(\d+) Dateien in (\d+) Baeumen, Tuer-Soll erfuellt", rest)',
           r'm = re.search(r"(\d+) Dateien in (\d+) Baeumen, Tuer-Soll erfuellt", rest)'),
    ("D8", '    text = text.replace("\\r", "")\n', ''),
    ("D9", '[z for z in text.split("\\n") if z.strip()]', '[z for z in text.split("\\n")]'),
    ("D10", r'"== %s-(FEHLER|ABWEICHUNG): (.+) =="', r'"== %s-(FEHLER): (.+) =="'),
]

def main():
    quelle, arbeit = sys.argv[1], sys.argv[2]
    os.makedirs(arbeit, exist_ok=True)
    text = open(quelle, encoding="utf-8", newline="").read()
    bemerkt = 0
    for nr, alt, neu in AEND:
        n = text.count(alt)
        if n != 1:
            print("%-4s UNGUELTIG (Muster %d-mal)" % (nr, n)); continue
        k = os.path.join(arbeit, "u_%s.py" % nr)
        open(k, "w", encoding="utf-8", newline="").write(text.replace(alt, neu))
        p = subprocess.run([sys.executable, k, "--selbsttest"], capture_output=True, text=True)
        zeilen = [z for z in p.stdout.splitlines() if z.strip()]
        letzte = zeilen[-1] if zeilen else "(keine Ausgabe)"
        fehl = [z.strip() for z in zeilen if z.startswith("   [FEHLER]")][:2]
        b = p.returncode != 0
        bemerkt += b
        print("%-4s %-11s rc=%d  %s" % (nr, "BEMERKT" if b else "NICHT", p.returncode, letzte[:150]))
        for z in fehl:
            print("         " + z[:170])
    print("SUMME: %d von %d bemerkt" % (bemerkt, len(AEND)))

main()
