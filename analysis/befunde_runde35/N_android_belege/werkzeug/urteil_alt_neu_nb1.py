# Runde 35 Spur N, Nachbesserung 1: Urteil @b19c39fa (alt) gegen das neue Urteil auf ALLEN Faellen des neuen
# Selbsttests - aendert die Nachbesserung ein Urteil? (nur der Grundtext darf anders sein)
# Aufruf: python urteil_alt_neu_nb1.py <gate_urteil_alt.py> <gate_urteil_neu.py>
import importlib.util, sys

def lade(name, pfad):
    spec = importlib.util.spec_from_file_location(name, pfad)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m

alt, neu = lade("alt", sys.argv[1]), lade("neu", sys.argv[2])
gleich = anders = 0
for titel, modus, text, rc, ein, soll in neu._faelle():
    a = alt.urteil_rufen(alt.urteil, modus, text, rc, apk_eintraege=ein)[0]
    n = neu.urteil_rufen(neu.urteil, modus, text, rc, apk_eintraege=ein)[0]
    if a == n:
        gleich += 1
    else:
        anders += 1
        print("ANDERS %-60s alt %d neu %d (soll %d)" % (titel[:60], a, n, soll))
print("SUMME: %d Faelle, %d gleiches Urteil, %d anders" % (gleich + anders, gleich, anders))
